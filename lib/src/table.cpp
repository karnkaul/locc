#include "locc/table.hpp"
#include <klib/text_table.hpp>
#include <array>
#include <cassert>

namespace locc {
namespace table {
namespace {
[[nodiscard]] constexpr auto to_align(Align const in) -> klib::TextTable::Align {
	switch (in) {
	case Align::Center: return klib::TextTable::Align::Center;
	case Align::Right: return klib::TextTable::Align::Right;
	default:
	case Align::Left: return klib::TextTable::Align::Left;
	}
}

constexpr auto column_for(Header const header) -> Column {
	auto ret = Column{.binding = header};
	if (header == Header::Category) {
		ret.align = Align::Left;
	} else {
		ret.align = Align::Right;
	}
	return ret;
}

constexpr auto default_columns_v = [] {
	auto ret = std::array<Column, std::size_t(Header::COUNT_)>{};
	for (auto i = 0uz; i < ret.size(); ++i) { ret.at(i) = column_for(Header(i)); }
	return ret;
}();

[[nodiscard]] auto serialize(Record const& record, Column const& column, std::string_view const separator) -> std::string {
	if (column.binding == Header::Category) { return record.category; }
	return record.serialize(to_metric(column.binding).value(), separator);
}

class Builder {
  public:
	explicit Builder(std::span<Column const> columns) { create_columns(columns); }

	[[nodiscard]] auto operator()(std::span<Record const> records, std::string_view const digit_separator) -> std::string {
		auto table_builder = klib::TextTable::Builder{};
		for (auto const& column : m_columns) { table_builder.add_column(std::string{column.title}, to_align(column.align)); }
		auto table = table_builder.build();

		auto row = std::vector<std::string>{};
		for (auto const& record : records) {
			row.clear();
			row.reserve(m_columns.size());
			for (auto const& column : m_columns) { row.push_back(serialize(record, column, digit_separator)); }
			table.push_row(std::move(row));
		}

		return table.serialize();
	}

  private:
	void create_columns(std::span<Column const> columns) {
		if (columns.empty()) { columns = default_columns_v; }

		m_columns.clear();
		for (auto column : columns) {
			if (column.title.empty()) { column.title = header_name_v[column.binding]; }
			m_columns.push_back(column);
		}
	}

	std::vector<Column> m_columns{};
};
} // namespace

auto BuildOptions::default_columns() -> std::span<Column const> { return default_columns_v; }
} // namespace table

auto table::build(std::span<Record const> records, BuildOptions const& options) -> std::string {
	return table::Builder{options.columns}(records, options.digit_separator);
}
} // namespace locc
