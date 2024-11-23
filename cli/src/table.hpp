#pragma once
#include <locc/row.hpp>
#include <array>
#include <vector>

namespace locc {
class Table {
  public:
	static constexpr std::size_t cols_v = 6;
	static constexpr std::string_view file_type_name_v = "File Type";

	void add_row(Row row);

	[[nodiscard]] auto serialize() -> std::string;

  private:
	struct Col {
		std::vector<std::string> cells{};
		std::size_t width{};
	};

	void set_header();
	void update_widths();

	std::array<Col, cols_v> m_cols{};
};
} // namespace locc
