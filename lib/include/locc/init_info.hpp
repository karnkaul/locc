#pragma once
#include "locc/specification.hpp"
#include <klib/base_types.hpp>
#include <klib/enum_array.hpp>
#include <klib/enum_ops.hpp>
#include <cstdint>

namespace locc {
enum struct ThreadCount : std::int8_t {};
[[nodiscard]] auto get_max_threads() -> ThreadCount;

/// \brief Line counting heuristic.
/// Performance: only look for comments at the start of a line (after trimming whitespace).
/// Precision: look for comments across every character.
enum class Heuristic : std::int8_t { Performance, Precision, COUNT_ };
constexpr auto heuristic_name_v = klib::EnumArray<Heuristic, std::string_view>{"Performance", "Precision"};

enum class Flag : std::uint8_t {
	None = 0,
	NoDefaultCodeFamilies = 1 << 0,
	NoDefaultTextCategories = 1 << 1,
	NoDefaultExcludeSuffixes = 1 << 2,
};
[[nodiscard]] constexpr auto enable_enum_bitops(Flag /*unused*/) { return true; }

struct InitInfo {
	ThreadCount thread_count{get_max_threads()};
	Heuristic heuristic{Heuristic::Performance};
	Specification custom_spec{};
	std::string_view custom_spec_json{};
	Flag flags{Flag::None};
};
} // namespace locc
