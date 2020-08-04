#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>
#include <table_formatter.hpp>

namespace locc
{
namespace
{
struct fill_str
{
	char c;
	uint16_t count;
	bool trailing_newline;
};

std::ostream& operator<<(std::ostream& out_str, fill_str const& fill)
{
	if (fill.count > 0)
	{
		out_str << std::setw(fill.count) << std::setfill(fill.c) << fill.c << std::setfill(out_str.widen(' '));
		if (fill.trailing_newline)
		{
			out_str << "\n";
		}
	}
	return out_str;
}
} // namespace

int16_t table_formatter::add_column(std::string header, bool left_aligned, uint8_t precision)
{
	m_cols.push_back({std::move(header), 0, precision, left_aligned});
	m_cols.back().width = (uint8_t)m_cols.back().header.size();
	return (int16_t)(m_cols.size() - 1);
}

template <typename F>
void table_formatter::serialise_row(std::ostringstream& out_str, char fill, F cell_value) const
{
	out_str << fill_str{m_row_prefix.first, m_row_prefix.second, false};
	for (std::size_t index = 0; index < m_cols.size(); ++index)
	{
		auto const& col = m_cols.at(index);
		auto const& value = cell_value(col, index);
		auto const pad = (uint16_t)((std::size_t)col.width - value.size());
		out_str << m_col_separator << fill;
		if (col.left_aligned)
		{
			out_str << value << fill_str{fill, (uint16_t)(pad + 1), false};
		}
		else
		{
			out_str << fill_str{fill, pad, false} << value << fill;
		}
	}
	out_str << m_col_separator << "\n";
}

uint16_t table_formatter::table_width() const
{
	uint16_t tw = (uint16_t)m_col_separator.size();
	for (std::size_t index = 0; index < m_cols.size(); ++index)
	{
		auto const& col = m_cols.at(index);
		tw += ((uint16_t)m_col_separator.size() + 2 + col.width);
	}
	return tw;
}

void table_formatter::clear()
{
	m_rows.clear();
	m_cols.clear();
	m_write_head = 0;
}

bool table_formatter::sort(uint8_t col_index, bool descending)
{
	if (col_index > 0 && col_index < (int8_t)m_cols.size())
	{
		std::vector<row> rows;
		rows.reserve(m_rows.size());
		std::move(m_rows.begin(), m_rows.end(), std::back_inserter(rows));
		auto index = (std::size_t)col_index;
		std::sort(rows.begin(), rows.end(), [index, descending](row const& lhs, row const& rhs) -> bool {
			return descending ? lhs.at(index) > rhs.at(index) : lhs.at(index) < rhs.at(index);
		});
		std::move(rows.begin(), rows.end(), m_rows.begin());
		return true;
	}
	return false;
}

std::string table_formatter::to_string() const
{
	if (m_cols.empty())
	{
		return {};
	}
	std::ostringstream str;
	uint16_t tw = table_width();
	static std::string const blank_str;
	serialise_row(str, m_header_pad, [](col const& col, std::size_t) -> std::string const& { return col.header; });

	if (m_show_header_separator)
	{
		str << fill_str{m_row_prefix.first, m_row_prefix.second, false} << fill_str{m_header_separator, tw, true};
	}
	for (auto const& row : m_rows)
	{
		serialise_row(str, m_col_pad, [&row](col const&, std::size_t index) -> std::string const& { return index >= row.size() ? blank_str : row.at(index); });
	}
	return str.str();
}
} // namespace locc
