#pragma once
#include "locc/record.hpp"
#include <optional>
#include <span>

namespace locc::table {
enum class Align : std::int8_t { Left, Center, Right };

enum class Header : std::int8_t { Category, Files, Lines, Code, Comments, Empty, COUNT_ };
constexpr auto header_name_v = klib::EnumArray<table::Header, std::string_view>{
	"Category", "Files", "Lines", "Code", "Comments", "Empty",
};

/// \returns Metric if valid.
[[nodiscard]] constexpr auto to_metric(Header const in) -> std::optional<Metric> {
	switch (in) {
	default: return {};
	case Header::Files: return Metric::Files;
	case Header::Lines: return Metric::Lines;
	case Header::Code: return Metric::Code;
	case Header::Comments: return Metric::Comments;
	case Header::Empty: return Metric::Empty;
	}
}

struct Column {
	/// \brief Which Header this column binds to.
	Header binding{Header::Category};
	/// \brief Alignment of contents.
	Align align{Align::Left};
	/// \brief Custom title.
	std::string_view title{};
};

struct BuildOptions {
	/// \returns List of default Columns.
	[[nodiscard]] static auto default_columns() -> std::span<Column const>;

	/// \brief Ordered list of Columns to serialize.
	std::span<Column const> columns{default_columns()};
	/// \brief Use empty string for no separator.
	std::string_view digit_separator{","};
};

/// \returns Serialized string table of records.
[[nodiscard]] auto build(std::span<Record const> records, BuildOptions const& options = {}) -> std::string;
} // namespace locc::table
