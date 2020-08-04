#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>
#include "table_formatter.hpp"

namespace kt
{
std::ostream& operator<<(std::ostream& out_str, table_formatter::fill const& fill)
{
	if (fill.second > 0)
	{
		out_str << std::setw(fill.second) << std::setfill(fill.first) << fill.first << std::setfill(out_str.widen(' '));
	}
	return out_str;
}

int8_t table_formatter::add_column(std::string header, bool left_aligned, uint8_t precision)
{
	m_data.cols.push_back({std::move(header), 0, precision, left_aligned});
	m_data.cols.back().width = (uint8_t)m_data.cols.back().header.size();
	return (int8_t)(m_data.cols.size() - 1);
}

template <typename F>
void table_formatter::serialise_row(std::ostringstream& out_str, char c, F cell_value) const
{
	out_str << fill{m_info.row_prefix.first, m_info.row_prefix.second};
	for (std::size_t index = 0; index < m_data.cols.size(); ++index)
	{
		auto const& col = m_data.cols.at(index);
		auto const& value = cell_value(col, index);
		auto const pad = (uint16_t)((std::size_t)col.width - value.size());
		out_str << m_info.col_separator << c;
		if (col.left_aligned)
		{
			out_str << value << fill{c, (uint16_t)(pad + 1)};
		}
		else
		{
			out_str << fill{c, pad} << value << c;
		}
	}
	out_str << m_info.col_separator << "\n";
}

uint16_t table_formatter::table_width() const
{
	uint16_t tw = (uint16_t)m_info.col_separator.size();
	for (std::size_t index = 0; index < m_data.cols.size(); ++index)
	{
		auto const& col = m_data.cols.at(index);
		tw += ((uint16_t)m_info.col_separator.size() + 2 + col.width);
	}
	return tw;
}

void table_formatter::clear()
{
	m_data = {};
}

bool table_formatter::sort(uint8_t col_index, bool descending)
{
	if (col_index >= 0 && col_index < (int8_t)m_data.cols.size())
	{
		std::vector<row> rows;
		rows.reserve(m_data.rows.size());
		std::move(m_data.rows.begin(), m_data.rows.end(), std::back_inserter(rows));
		auto index = (std::size_t)col_index;
		std::sort(rows.begin(), rows.end(), [index, descending](row const& lhs, row const& rhs) -> bool {
			return descending ? lhs.at(index) > rhs.at(index) : lhs.at(index) < rhs.at(index);
		});
		std::move(rows.begin(), rows.end(), m_data.rows.begin());
		return true;
	}
	return false;
}

std::string table_formatter::to_string() const
{
	if (m_data.cols.empty())
	{
		return {};
	}
	std::ostringstream str;
	uint16_t tw = table_width();
	static std::string const blank_str;
	serialise_row(str, m_info.header_pad, [](col const& col, std::size_t) -> std::string const& { return col.header; });

	if (m_info.show_header_separator)
	{
		str << fill{m_info.row_prefix.first, m_info.row_prefix.second} << fill{m_info.header_separator, tw} << "\n";
	}
	for (auto const& row : m_data.rows)
	{
		auto per_row = [&row](col const&, std::size_t index) -> std::string const& { return index >= row.size() ? blank_str : row.at(index); };
		serialise_row(str, m_info.col_pad, per_row);
	}
	return str.str();
}
} // namespace kt
