#include <locc/count_lines.hpp>
#include <locc/line_counter.hpp>

#include <ktask/queue.hpp>
#include <lut.hpp>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <ranges>

namespace locc {

namespace {
namespace fs = std::filesystem;

void to_lowercase(std::string& out) {
	for (char& c : out) { c = char(std::tolower(static_cast<unsigned char>(c))); }
}

struct AtomicProgress {
	std::atomic<State> state{State::Waiting};
	std::atomic<std::uint64_t> counted{};
	std::atomic<uint64_t> total{};

	[[nodiscard]] auto to_progress() const -> Progress { return Progress{.state = state, .counted = counted, .total = total}; }

	operator Progress() const { return to_progress(); }
};

using Sorter = bool (*)(Row const&, Row const&);

struct Workspace {
	AtomicProgress progress{};
	std::unordered_map<std::string, Row> rows{};
	mutable std::mutex mutex{};

	void accumulate(std::string const& file_type, LineCount count) {
		++progress.counted;
		auto lock = std::scoped_lock{mutex};
		auto& entry = rows[file_type];
		++entry.line_count.metrics[LineCount::Files];
		entry.line_count += count;
	}

	auto to_rows() const -> std::vector<Row> {
		auto ret = std::vector<Row>{};
		auto lock = std::scoped_lock{mutex};
		ret.reserve(rows.size());
		for (auto const& [file_type, in_row] : rows) {
			auto row = Row{
				.file_type = file_type,
				.line_count = in_row.line_count,
			};
			ret.push_back(std::move(row));
		}
		return ret;
	}
};

struct File {
	fs::path path{};
	std::string file_type{};
	Grammar const* grammar{};
};

struct Counter : ktask::Task {
	explicit Counter(Workspace& workspace, File file) : m_workspace(workspace), m_file(std::move(file)) { assert(m_file.grammar); }

  private:
	void execute() final {
		auto lc = LineCount{};
		if (auto file = std::ifstream{m_file.path}) { lc = count_lines(file, *m_file.grammar); }
		m_workspace.accumulate(m_file.file_type, lc);
	}

	Workspace& m_workspace;
	File m_file;
};
} // namespace

struct LineCounter::Impl {
	explicit Impl(ktask::Queue& queue, Query const& query) : m_queue(queue), m_query(query), m_path(query.path) {}

	[[nodiscard]] auto get_progress() const -> AtomicProgress const& { return m_workspace.progress; }

	[[nodiscard]] auto to_rows() const -> std::vector<Row> { return m_workspace.to_rows(); }

	void execute() {
		build_lut();
		collect_files();
		count_lines();
	}

  private:
	void build_lut() {
		m_workspace.progress.state = State::Initializing;
		m_lut = Lut::build(m_query.grammars);
	}

	void collect_files() {
		m_workspace.progress.state = State::Collecting;
		auto ec = std::error_code{};
		if (fs::is_regular_file(m_path, ec)) {
			add_file(m_path);
		} else if (fs::is_directory(m_path, ec)) {
			collect_dir(m_path);
		}
		m_workspace.progress.total = m_files.size();
	}

	void collect_dir(fs::path const& dir) {
		for (auto const& it : fs::directory_iterator{dir}) {
			if (it.is_regular_file()) {
				add_file(it.path());
			} else if (it.is_directory()) {
				collect_dir(it.path());
			}
		}
	}

	void add_file(fs::path const& path) {
		auto const skip = [p = path.generic_string()](std::string_view const s) { return p.contains(s); };
		if (m_query.filter != nullptr && !m_query.filter->should_count(path.generic_string())) { return; }
		auto extension = path.extension().string();
		to_lowercase(extension);
		auto const filename = path.filename().string();
		auto const* entry = m_lut.get_entry(extension, filename);
		if (entry == nullptr) { return; }
		auto file = File{.path = path, .grammar = entry->grammar};
		if (!entry->type_name.empty()) {
			file.file_type = entry->type_name;
		} else {
			file.file_type = std::move(extension);
		}
		m_files.push_back(std::move(file));
	}

	void count_lines() {
		m_workspace.progress.state = State::Counting;
		for (auto& file : m_files) {
			m_counters.push_back(std::make_unique<Counter>(m_workspace, std::move(file)));
			m_tasks.push_back(m_counters.back().get());
		}
		m_files.clear();
		m_files.shrink_to_fit();
		auto const status = m_queue.fork_join(m_tasks);
		m_tasks.clear();
		m_tasks.shrink_to_fit();
		if (status == Status::Dropped) {
			m_workspace.progress.state = State::Dropped;
		} else {
			m_workspace.progress.state = State::Completed;
		}
	}

	void wait_for_counts() {
		for (auto const& counter : m_counters) {
			counter->wait();
			if (counter->get_status() == Status::Dropped) {
				m_workspace.progress.state = State::Dropped;
				return;
			}
		}
	}

	ktask::Queue& m_queue;
	Query m_query;

	Lut m_lut{};
	Workspace m_workspace{};
	fs::path m_path{};
	std::vector<File> m_files{};
	std::vector<std::unique_ptr<Counter>> m_counters{};
	std::vector<ktask::Task*> m_tasks{};
};

void LineCounter::Deleter::operator()(Impl* ptr) const noexcept { std::default_delete<Impl>{}(ptr); }

LineCounter::LineCounter(ktask::Queue& queue, Query query) {
	if (query.grammars.empty()) { return; }
	if (query.path.empty()) { query.path = "."; }
	m_impl.reset(new Impl(queue, query)); // NOLINT(cppcoreguidelines-owning-memory)
}

auto LineCounter::get_progress() const -> Progress {
	if (!m_impl) { return {}; }
	return m_impl->get_progress();
}

auto LineCounter::to_rows() const -> std::vector<Row> {
	if (!m_impl) { return {}; }
	return m_impl->to_rows();
}

void LineCounter::execute() {
	if (!m_impl) { return; }
	m_impl->execute();
}
} // namespace locc
