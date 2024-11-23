#include <locc/row.hpp>
#include <algorithm>
#include <array>
#include <charconv>

namespace locc {
auto Row::aggregate(std::span<Row const> rows, std::string file_type) -> Row {
	auto ret = Row{.file_type = std::move(file_type)};
	for (auto const& in : rows) { ret.line_count += in.line_count; }
	return ret;
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

void locc::sort_by_metric(std::span<Row> rows, LineCount::Metric metric) {
	metric = std::clamp(metric, LineCount::Metric{}, LineCount::COUNT_);
	auto const pred = [metric](Row const& a, Row const& b) {
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		return a.line_count.metrics[metric] > b.line_count.metrics[metric];
	};
	std::ranges::sort(rows, pred);
}

void locc::sort_by_file_type(std::span<Row> rows) {
	std::ranges::sort(rows, [](Row const& a, Row const& b) { return a.file_type < b.file_type; });
}
