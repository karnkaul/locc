#include <iomanip>
#include <sstream>
#include <table_formatter.hpp>

namespace locc
{
namespace
{
void fill(std::ostringstream& out_str, char c, uint16_t count)
{
	if (count > 0)
	{
		out_str << std::setw(count) << std::setfill(c) << c << std::setfill(out_str.widen(' ')) << "\n";
	}
}
} // namespace

void table_formatter::add_column(std::string header, bool left_aligned, uint8_t precision)
{
	m_cols.push_back({std::move(header), 0, precision, left_aligned});
	m_cols.back().width = (uint8_t)m_cols.back().header.size();
}

template <typename F>
void table_formatter::serialise_row(std::ostringstream& out_str, F cell_value) const
{
	out_str << m_row_prefix;
	for (std::size_t index = 0; index < m_cols.size(); ++index)
	{
		auto const& col = m_cols.at(index);
		if (col.left_aligned)
		{
			auto const& value = cell_value(col, index);
			auto const right_pad = (std::size_t)col.width - value.size() + 1;
			out_str << m_col_separator << m_col_padding << cell_value(col, index) << std::setw(right_pad) << " " << m_col_padding;
		}
		else
		{
			out_str << m_col_separator << m_col_padding << std::setw(col.width) << cell_value(col, index) << m_col_padding;
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
		tw += ((uint16_t)m_col_separator.size() + 2 * (uint16_t)m_col_padding.size() + col.width);
	}
	return tw;
}

std::string table_formatter::to_string() const
{
	// m_col_padding = "    ";

	if (m_cols.empty())
	{
		return {};
	}
	std::ostringstream str;
	uint16_t tw = table_width();

	static std::string const blank_str;
	serialise_row(str, [](col const& col, std::size_t) -> std::string const& { return col.header; });

	if (m_show_header_separator)
	{
		str << m_row_prefix;
		fill(str, m_header_separator, tw);
	}
	for (auto const& row : m_rows)
	{
		serialise_row(str, [&row](col const&, std::size_t index) -> std::string const& { return index >= row.size() ? blank_str : row.at(index); });
	}
	return str.str();
}
} // namespace locc
