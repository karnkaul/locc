#include <locc/row.hpp>
#include <algorithm>
#include <array>
#include <charconv>

namespace locc {
namespace {
template <template <typename> typename Comp>
void do_sort_by_metric(std::span<Row> rows, LineCount::Metric metric) {
	metric = std::clamp(metric, LineCount::Metric{}, LineCount::COUNT_);
	auto const pred = [metric](Row const& a, Row const& b) {
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		return Comp{}(a.line_count.metrics[metric], b.line_count.metrics[metric]);
	};
	std::ranges::sort(rows, pred);
}

template <template <typename> typename Comp>
void do_sort_by_file_type(std::span<Row> rows) {
	std::ranges::sort(rows, [](Row const& a, Row const& b) { return Comp{}(a.file_type, b.file_type); });
}
} // namespace

void Row::aggregate(std::span<Row const> rows) {
	for (auto const& in : rows) { line_count += in.line_count; }
}
} // namespace locc

auto locc::beautify(std::uint64_t const num) -> std::string {
	auto buf = std::array<char, 16>{};
	std::to_chars(buf.data(), buf.data() + buf.size(), num);
	auto view = std::string_view{buf.data()};
	auto ret = std::string{};
	auto digit = 0;
	while (!view.empty()) {
		auto const c = view.back();
		view = view.substr(0, view.size() - 1);
		ret += c;
		if (!view.empty() && ++digit % 3 == 0) { ret += ','; }
	}
	std::ranges::reverse(ret);
	return ret;
}

void locc::sort_by_metric(std::span<Row> rows, LineCount::Metric metric, SortDir const sort_dir) {
	if (sort_dir == SortDir::Ascending) { return do_sort_by_metric<std::less>(rows, metric); }
	return do_sort_by_metric<std::greater>(rows, metric);
}

void locc::sort_by_file_type(std::span<Row> rows, SortDir const sort_dir) {
	if (sort_dir == SortDir::Ascending) { return do_sort_by_file_type<std::less>(rows); }
	return do_sort_by_file_type<std::greater>(rows);
}
