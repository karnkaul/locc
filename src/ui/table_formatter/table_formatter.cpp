#include <algorithm>
#include <iomanip>
#include <vector>
#include <ui/table_formatter/table_formatter.hpp>

namespace kt {
std::ostream& operator<<(std::ostream& out_str, table_formatter::fill const& fill) {
	if (fill.second > 0) {
		out_str << std::setw(fill.second) << std::setfill(fill.first) << fill.first << std::setfill(out_str.widen(' '));
	}
	return out_str;
}

std::int8_t table_formatter::add_column(std::string header, bool left_aligned, std::uint8_t precision) {
	m_data.cols.push_back({std::move(header), 0, precision, left_aligned});
	m_data.cols.back().width = (std::uint8_t)m_data.cols.back().header.size();
	return (std::int8_t)(m_data.cols.size() - 1);
}

template <typename F>
void table_formatter::serialise_row(std::ostringstream& out_str, char c, F cell_value) const {
	out_str << fill{m_info.row_prefix.first, m_info.row_prefix.second};
	for (std::size_t index = 0; index < m_data.cols.size(); ++index) {
		auto const& col = m_data.cols.at(index);
		auto const& value = cell_value(col, index);
		auto const pad = (std::uint16_t)((std::size_t)col.width - value.size());
		out_str << m_info.col_separator << c;
		if (col.left_aligned) {
			out_str << value << fill{c, (std::uint16_t)(pad + 1)};
		} else {
			out_str << fill{c, pad} << value << c;
		}
	}
	out_str << m_info.col_separator << "\n";
}

std::uint16_t table_formatter::table_width() const {
	std::uint16_t tw = (std::uint16_t)m_info.col_separator.size();
	for (std::size_t index = 0; index < m_data.cols.size(); ++index) {
		auto const& col = m_data.cols.at(index);
		tw += ((std::uint16_t)m_info.col_separator.size() + 2 + col.width);
	}
	return tw;
}

void table_formatter::clear() { m_data = {}; }

bool table_formatter::sort(std::uint8_t col_index, bool descending) {
	if (col_index < (std::int8_t)m_data.cols.size()) {
		std::vector<row> rows;
		rows.reserve(m_data.rows.size());
		std::move(m_data.rows.begin(), m_data.rows.end(), std::back_inserter(rows));
		auto index = (std::size_t)col_index;
		std::sort(rows.begin(), rows.end(), [index, descending](row const& lhs, row const& rhs) -> bool {
			auto const& l = lhs.at(index);
			auto const& r = rhs.at(index);
			if (auto pl = std::get_if<std::uint64_t>(&l.number); auto pr = std::get_if<std::uint64_t>(&r.number)) {
				return descending ? *pl > *pr : *pl < *pr;
			} else if (auto pl = std::get_if<std::int64_t>(&l.number); auto pr = std::get_if<std::int64_t>(&r.number)) {
				return descending ? *pl > *pr : *pl < *pr;
			} else if (auto pl = std::get_if<double>(&l.number); auto pr = std::get_if<double>(&r.number)) {
				return descending ? *pl > *pr : *pl < *pr;
			}
			return descending ? l.text > r.text : l.text < r.text;
		});
		std::move(rows.begin(), rows.end(), m_data.rows.begin());
		return true;
	}
	return false;
}

std::string table_formatter::to_string() const {
	if (m_data.cols.empty()) {
		return {};
	}
	std::ostringstream str;
	std::uint16_t tw = table_width();
	static std::string const blank_str;
	serialise_row(str, m_info.header_pad, [](col const& col, std::size_t) -> std::string const& { return col.header; });

	if (m_info.show_header_separator) {
		str << fill{m_info.row_prefix.first, m_info.row_prefix.second} << fill{m_info.header_separator, tw} << "\n";
	}
	for (auto const& row : m_data.rows) {
		auto per_row = [&row](col const&, std::size_t index) -> std::string const& { return index >= row.size() ? blank_str : row.at(index).text; };
		serialise_row(str, m_info.col_pad, per_row);
	}
	return str.str();
}
} // namespace kt
