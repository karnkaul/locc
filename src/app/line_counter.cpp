#include <cctype>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <app/config.hpp>
#include <app/line_counter.hpp>
#include <ui/ui.hpp>

namespace {
void trim_leading_spaces(std::string_view& out_line) noexcept {
	while (!out_line.empty() && std::isspace(out_line.at(0))) {
		out_line = out_line.substr(1);
	}
}

bool line_comment_at_start(locc::comment_info const& info, std::string_view sub_line) noexcept {
	for (auto& str : info.comment_lines) {
		if (auto const search = sub_line.find(str); search == 0) {
			return true;
		}
	}
	return false;
}

void trim_leading_comment_blocks(locc::comment_info const& info, locc::comment_block const* open_block, std::string_view& out_line) noexcept {
	if (!out_line.empty()) {
		if (!open_block) {
			for (auto const& block : info.comment_blocks) {
				if (auto begin = out_line.find(block.first); begin != locc::null_index) {
					open_block = &block;
					out_line = out_line.substr(begin + block.first.size());
					trim_leading_comment_blocks(info, open_block, out_line);
				}
			}
		} else {
			if (auto end = out_line.find(open_block->second); end != locc::null_index) {
				out_line = out_line.substr(end + open_block->second.size());
				open_block = nullptr;
				trim_leading_comment_blocks(info, open_block, out_line);
			}
		}
	}
	return;
}

void count_line(locc::comment_info const& info, locc::file_t& out_file, locc::comment_block const* open_block, std::string_view line) noexcept {
	++out_file.lines.total;
	trim_leading_spaces(line);
	if (!open_block) {
		if (line_comment_at_start(info, line)) {
			++out_file.lines.comments;
		} else {
			trim_leading_comment_blocks(info, open_block, line);
			if (!line.empty()) {
				++out_file.lines.code;
			} else {
				open_block ? ++out_file.lines.comments : ++out_file.lines.blank;
			}
		}
	} else {
		trim_leading_comment_blocks(info, open_block, line);
		if (!line.empty()) {
			++out_file.lines.comments;
		} else {
			open_block ? ++out_file.lines.comments : ++out_file.lines.blank;
		}
	}
}

void count_lines(locc::file_t& out_file) {
	if (auto file = std::ifstream(stdfs::absolute(out_file.path), std::ios::in)) {
		out_file.lines.code = out_file.lines.total = 1;
		std::string line;
		locc::comment_block const* open_block = nullptr;
		auto const& info = cfg::g_settings.find_comment_info(out_file.ext);
		while (std::getline(file, line)) {
			count_line(info, out_file, open_block, line);
		}
	}
}
} // namespace

namespace locc {
struct async_counter::worker {
	worker() = default;
	explicit worker(async_queue* queue) {
		thread = kt::kthread([queue]() {
			while (auto task = queue->pop()) {
				if (!task) {
					break;
				}
				(*task)();
			}
		});
	}

  private:
	kt::kthread thread;
};

async_counter::async_counter(result_t* result) : m_result(result) {
	assert(m_result != nullptr && "required parameter null");
	int const thread_count = cfg::test(cfg::flag::one_thread) ? 0 : std::thread::hardware_concurrency() - 1;
	m_workers.clear();
	m_workers.reserve(thread_count);
	if (thread_count > 0) {
		locc::log_if(cfg::test(cfg::flag::debug), "  -- launching {} worker threads\n", thread_count);
		for (int count = thread_count; count > 0; --count) {
			m_workers.push_back(worker(&m_queue));
		}
	}
}

async_counter::~async_counter() {
	auto residue = m_queue.clear();
	m_queue.active(false);
	bool const threads = !m_workers.empty();
	m_workers.clear();
	locc::log_if(cfg::test(cfg::flag::debug) && threads, "  -- worker threads completed\n");
	for (auto& task : residue) {
		task();
	}
	locc::log_if(cfg::test(cfg::flag::debug), "\n");
	for (auto& [_, data] : m_result->dist) {
		data.ratio.divide(data.counts.lines, m_result->totals);
	}
}

void async_counter::count(file_t file, bool blanks) {
	if (m_workers.empty()) {
		count_impl(std::move(file), blanks);
	} else {
		m_queue.push([this, f = std::move(file), blanks]() { count_impl(std::move(f), blanks); });
	}
}

void async_counter::count_impl(file_t file, bool blanks) {
	count_lines(file);
	auto lock = m_mutex.lock();
	m_result->totals.total += file.lines.total;
	m_result->totals.code += file.lines.code;
	auto& data = m_result->dist[file.id];
	++data.counts.files;
	if (blanks) {
		m_result->totals.code += file.lines.blank;
		data.counts.lines.code += file.lines.blank;
	}
	data.counts.lines.code += file.lines.code;
	data.counts.lines.comments += file.lines.comments;
	data.counts.lines.total += file.lines.total;
	m_result->files.push_back(std::move(file));
}
} // namespace locc
