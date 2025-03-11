#include <counter.hpp>
#include <klib/text_table.hpp>
#include <table.hpp>
#include <print>
#include <thread>

namespace locc::cli {
namespace {
void print_rows(std::span<Row const> rows) {
	std::println();
	auto builder = klib::TextTable::Builder{};
	builder.add_column("File Type");
	for (auto i = LineCount::Metric{}; i < LineCount::Metric::COUNT_; i = LineCount::Metric(int(i + 1))) {
		builder.add_column(std::string{metric_name_v.at(i)}, klib::TextTable::Align::Right);
	}
	auto table_row = std::vector<std::string>{};
	auto table = builder.build();
	for (auto const& row : rows) {
		table_row.clear();
		table_row.push_back(row.file_type);
		for (auto const metric : row.line_count.metrics) { table_row.push_back(beautify(metric)); }
		table.push_row(std::move(table_row));
	}
	std::println("{}", table.serialize());
}

constexpr auto by_file_type = LineCount::Metric{100};

constexpr auto to_metric(std::string_view const sort_by) -> LineCount::Metric {
	if (sort_by == Table::file_type_name_v) { return by_file_type; }
	for (std::size_t i = 0; i < LineCount::COUNT_; ++i) {
		auto const metric = LineCount::Metric{i};
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		if (sort_by == metric_name_v[metric]) { return metric; }
	}
	return LineCount::Lines;
}
} // namespace

Counter::Counter(Params const& params, InstanceCreateInfo ici)
	: m_params(params), m_queue(klib::task::QueueCreateInfo{.thread_count = klib::task::ThreadCount{params.thread_count}}), m_locc(m_queue, std::move(ici)) {}

void Counter::run() {
	if (m_params.verbose) { print_params(); }

	auto line_counter = m_locc.start_count(m_params.path);
	if (!line_counter || line_counter->get_status() == LineCounter::Status::None) { return; }

	if (!m_params.no_progress) {
		std::println();
		m_progress_bar.resize(progress_bar_length_v, '-');
		print_progress();
	}

	if (m_params.no_progress) {
		line_counter->wait();
	} else {
		print_progress_until_ready(*line_counter);
	}

	auto rows = line_counter->to_rows();
	auto const sort_by = to_metric(m_params.sort_column);
	auto const sort_dir = m_params.sort_ascending ? SortDir::Ascending : SortDir::Descending;
	if (sort_by == by_file_type) {
		sort_by_file_type(rows, sort_dir);
	} else {
		sort_by_metric(rows, sort_by, sort_dir);
	}
	auto totals = Row{.file_type = "Total"};
	totals.aggregate(rows);
	rows.push_back(std::move(totals));

	print_rows(rows);
}

void Counter::print_params() const {
	std::println("params:");
	auto const print_param = [](std::string_view left, auto const& right) { std::println("  {}: {}", left, right); };
	print_param("sort by\t", m_params.sort_column);
	print_param("exclude\t", m_params.exclude_patterns);
	print_param("grammars\t", m_params.grammars_json);
	print_param("threads\t", m_params.thread_count);
	print_param("sort ascend\t", m_params.sort_ascending);
	print_param("no progress\t", m_params.no_progress);
	print_param("verbose\t", m_params.verbose);
	print_param("path\t\t", m_params.path);
	std::println();
}

void Counter::print_progress() {
	for (std::size_t i = 0; i < m_progress_length; ++i) { std::print(" "); }
	std::print("\r");
	auto const n = std::size_t(m_progress.as_float() * progress_bar_length_v);
	for (std::size_t i = 0; i < n; ++i) { m_progress_bar.at(i) = '='; }
	m_progress_length = std::formatted_size("[{}] {} ({}/{})", m_progress_bar, to_str(m_progress.state), m_progress.counted, m_progress.total);
	std::print("[{}] {} ({}/{})", m_progress_bar, to_str(m_progress.state), m_progress.counted, m_progress.total);
	std::fflush(stdout);
}

void Counter::print_progress_until_ready(LineCounter const& line_counter) {
	std::print("\r");
	while (line_counter.is_busy()) {
		auto const progress = line_counter.get_progress();
		if (progress != m_progress) {
			m_progress = progress;
			print_progress();
			std::print("\r");
		}
		std::this_thread::yield();
	}
	m_progress = line_counter.get_progress();
	print_progress();
	std::println();
}
} // namespace locc::cli
