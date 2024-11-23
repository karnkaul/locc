#pragma once
#include <array>
#include <compare>
#include <cstdint>
#include <string_view>

namespace locc {
struct LineCount {
	enum Metric : std::size_t { Files, Lines, Code, Comments, Empty, COUNT_ };

	std::array<std::uint64_t, COUNT_> metrics{};

	constexpr auto operator+=(LineCount const& rhs) -> LineCount& {
		for (auto i = std::size_t{}; i < COUNT_; ++i) {
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
			metrics[i] += rhs.metrics[i];
		}
		return *this;
	}

	auto operator<=>(LineCount const& rhs) const -> std::strong_ordering = default;
};

[[nodiscard]] constexpr auto operator+(LineCount const& a, LineCount const& b) -> LineCount {
	auto ret = a;
	ret += b;
	return ret;
}

constexpr auto metric_name_v = std::array<std::string_view, LineCount::COUNT_>{
	"Files", "Lines", "Code", "Comments", "Empty",
};
} // namespace locc
