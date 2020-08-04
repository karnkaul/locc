#pragma once
#include <deque>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

using sv = std::string_view;

namespace locc
{
class table_formatter final
{
public:
	using row = std::deque<std::string>;

	struct col
	{
		std::string header;
		uint8_t width = 0;
		uint8_t precision = 3;
		bool left_aligned = false;
	};

	using fill = std::pair<char, uint8_t>;

public:
	std::string m_col_separator = "|";
	fill m_row_prefix = {' ', 1};
	char m_col_pad = ' ';
	char m_header_pad = ' ';
	char m_header_separator = '-';
	bool m_show_header_separator = true;

private:
	std::deque<col> m_cols;
	std::deque<row> m_rows;
	std::size_t m_write_head = 0;

public:
	void add_column(std::string header, bool left_aligned = false, uint8_t precision = 3);

	template <typename Arg, typename... Args>
	void add_row(Arg&& arg, Args&&... args)
	{
		if (m_cols.empty())
		{
			m_cols.push_back({});
		}
		m_rows.push_back({});
		add_row_impl(std::forward<Arg>(arg), std::forward<Args>(args)...);
		m_write_head = 0;
	}

	void clear();

	std::string to_string() const;

private:
	template <typename...>
	constexpr static bool always_false = false;

	constexpr static uint8_t width(int64_t number)
	{
		uint8_t ret = 0;
		if (number < 0)
		{
			++ret;
			number = -number;
		}
		while (number > 0)
		{
			number /= 10;
			++ret;
		}
		return ret;
	}

	constexpr static uint8_t width(double number, uint8_t precision)
	{
		uint8_t ret = precision;
		if (number <= 0.0)
		{
			++ret;
			number = -number;
		}
		while (number > 10.0)
		{
			number /= 10.0;
			++ret;
		}
		return ret;
	}

	template <typename T>
	void add_cell(T&& arg)
	{
		if (m_write_head >= m_cols.size())
		{
			return;
		}
		auto& row = m_rows.back();
		auto& col = m_cols.at(m_write_head);
		while (m_write_head >= row.size())
		{
			row.push_back({});
		}
		auto& cell = row.at(m_write_head++);
		if constexpr (std::is_integral_v<T>)
		{
			cell = std::to_string(arg);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
		{
			cell = std::forward<T>(arg);
		}
		else if constexpr (std::is_same_v<std::decay_t<T>, sv>)
		{
			cell = std::string(arg);
		}
		else
		{
			std::stringstream str;
			if constexpr (std::is_floating_point_v<T>)
			{
				str.precision(col.precision);
				str << std::fixed << arg;
			}
			else
			{
				str << arg;
			}
			cell = str.str();
		}
		col.width = std::max((uint8_t)cell.size(), col.width);
	}

	template <typename Arg, typename... Args>
	void add_row_impl(Arg&& arg, Args&&... args)
	{
		add_cell<Arg>(std::forward<Arg>(arg));
		if constexpr (sizeof...(args) > 0)
		{
			add_row_impl<Args...>(std::forward<Args>(args)...);
		}
	}

	uint16_t table_width() const;

	template <typename F>
	void serialise_row(std::ostringstream& out_str, char fill, F cell_value) const;
};
} // namespace locc
