#include <table.hpp>
#include <cassert>
#include <iomanip>
#include <sstream>

namespace locc {
void Table::add_row(Row row) {
	if (m_cols[0].cells.empty()) { set_header(); }
	m_cols[0].cells.push_back(std::move(row.file_type));
	auto index = std::size_t{1};
	for (auto const metric : row.line_count.metrics) { m_cols.at(index++).cells.push_back(beautify(metric)); }
	update_widths();
}

auto Table::serialize() -> std::string {
	if (m_cols[0].cells.empty()) { return {}; }

	auto str = std::ostringstream{};
	str << std::left;

	auto const row_width = [this] {
		auto ret = std::size_t{};
		for (auto& col : m_cols) { ret += 2 + col.width + 1; } // "| x "
		ret += 1;											   // "|"
		return ret;
	}();

	auto const add_separator = [&] {
		str << std::setfill('-');
		str << std::setw(int(row_width)) << '-' << "\n";
		str << std::setfill(' ');
	};

	add_separator();
	for (std::size_t i = 0; i < m_cols[0].cells.size(); ++i) {
		str << std::left;
		auto first = true;
		for (auto& col : m_cols) {
			auto const& cell = col.cells.at(i);
			if (!first) { str << std::right; }
			str << "| " << std::setw(int(col.width)) << cell << " ";
			first = false;
		}
		str << "|\n";
		if (i == 0) { add_separator(); }
	}
	add_separator();

	for (auto& col : m_cols) {
		col.cells.clear();
		col.width = 0;
	}
	return str.str();
}

void Table::set_header() {
	m_cols = {
		Col{.cells = {file_type_name_v.data()}},
		Col{.cells = {metric_name_v[LineCount::Files].data()}},
		Col{.cells = {metric_name_v[LineCount::Lines].data()}},
		Col{.cells = {metric_name_v[LineCount::Code].data()}},
		Col{.cells = {metric_name_v[LineCount::Comments].data()}},
		Col{.cells = {metric_name_v[LineCount::Empty].data()}},
	};
	update_widths();
}

void Table::update_widths() {
	for (auto& col : m_cols) {
		assert(!col.cells.empty());
		col.width = std::max(col.width, col.cells.back().size());
	}
}
} // namespace locc
