#pragma once
#include <cstdint>
#include <deque>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace kt {
class table_formatter final {
  public:
	struct cell final {
		std::string text;
		std::variant<std::uint64_t, std::int64_t, double, bool> number;
	};

	using row = std::deque<cell>;
	using fill = std::pair<char, std::uint16_t>;

	struct info final {
		std::string col_separator = "|";
		fill row_prefix = {' ', 1};
		char col_pad = ' ';
		char header_pad = ' ';
		char header_separator = '-';
		bool show_header_separator = true;
	};

  public:
	info m_info;

  private:
	struct col final {
		std::string header;
		std::uint8_t width = 0;
		std::uint8_t precision = 3;
		bool left_aligned = false;
	};

	struct {
		std::deque<col> cols;
		std::deque<row> rows;
		std::size_t write_head = 0;
	} m_data;

  public:
	std::int8_t add_column(std::string header, bool left_aligned = false, std::uint8_t precision = 3);

	template <typename Arg, typename... Args>
	void add_row(Arg&& arg, Args&&... args);

	void clear();
	bool sort(std::uint8_t col_index, bool descending);

	std::string to_string() const;

  private:
	template <typename T>
	void add_cell(T&& arg);

	template <typename Arg, typename... Args>
	void add_row_impl(Arg&& arg, Args&&... args);

	std::uint16_t table_width() const;

	template <typename F>
	void serialise_row(std::ostringstream& out_str, char c, F cell_value) const;
};

std::ostream& operator<<(std::ostream& out_str, table_formatter::fill const& fill);

template <typename Arg, typename... Args>
void table_formatter::add_row(Arg&& arg, Args&&... args) {
	if (m_data.cols.empty()) {
		m_data.cols.push_back({});
	}
	m_data.rows.push_back({});
	add_row_impl(std::forward<Arg>(arg), std::forward<Args>(args)...);
	m_data.write_head = 0;
}

template <typename T>
void table_formatter::add_cell(T&& arg) {
	if (m_data.write_head >= m_data.cols.size()) {
		return;
	}
	auto& row = m_data.rows.back();
	auto& col = m_data.cols.at(m_data.write_head);
	while (m_data.write_head >= row.size()) {
		row.push_back({});
	}
	auto& cell = row.at(m_data.write_head++);
	if constexpr (std::is_integral_v<std::decay_t<T>>) {
		cell.text = std::to_string(arg);
		if constexpr (std::is_unsigned_v<std::decay_t<T>>) {
			cell.number = (std::uint64_t)arg;
		} else {
			cell.number = (std::int64_t)arg;
		}
	} else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
		cell.text = std::forward<T>(arg);
		cell.number = false;
	} else {
		std::stringstream str;
		str.precision(col.precision);
		if constexpr (std::is_floating_point_v<std::decay_t<T>>) {
			str << std::fixed << arg;
			cell.number = (double)arg;
		} else {
			str << arg;
			cell.number = false;
		}
		cell.text = str.str();
	}
	col.width = std::max((std::uint8_t)cell.text.size(), col.width);
}

template <typename Arg, typename... Args>
void table_formatter::add_row_impl(Arg&& arg, Args&&... args) {
	add_cell<Arg>(std::forward<Arg>(arg));
	if constexpr (sizeof...(args) > 0) {
		add_row_impl<Args...>(std::forward<Args>(args)...);
	}
}
} // namespace kt
