#pragma once
#include <klib/enum_array.hpp>
#include <cstdint>
#include <ranges>

namespace locc {
/// \brief Type of line count.
enum class Metric : std::int8_t { Files, Lines, Code, Comments, Empty, COUNT_ };
constexpr auto metric_name_v = klib::EnumArray<Metric, std::string_view>{"Files", "Lines", "Code", "Comments", "Empty"};

/// \brief Collection of line counts.
struct Metrics : klib::EnumArray<Metric, std::uint64_t> {};

constexpr auto operator+=(Metrics& lhs, Metrics const& rhs) -> Metrics& {
	for (auto [a, b] : std::views::zip(lhs, rhs)) { a += b; }
	return lhs;
}

[[nodiscard]] constexpr auto operator+(Metrics const& lhs, Metrics const& rhs) -> Metrics {
	auto ret = lhs;
	ret += rhs;
	return ret;
}
} // namespace locc
