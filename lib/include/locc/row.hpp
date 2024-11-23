#pragma once
#include <locc/line_count.hpp>
#include <span>
#include <string>

namespace locc {
struct Row {
	std::string file_type{};
	LineCount line_count{};

	[[nodiscard]] static auto aggregate(std::span<Row const> rows, std::string file_type = "Total") -> Row;
};

[[nodiscard]] auto beautify(std::uint64_t num) -> std::string;

void sort_by_metric(std::span<Row> rows, LineCount::Metric metric);
void sort_by_file_type(std::span<Row> rows);
} // namespace locc
