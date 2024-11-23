#pragma once
#include <locc/grammar.hpp>
#include <locc/line_count.hpp>
#include <istream>

namespace locc {
[[nodiscard]] auto count_lines(std::istream& in, Grammar const& grammar) -> LineCount;
[[nodiscard]] auto count_lines(std::string_view text, Grammar const& grammar) -> LineCount;
} // namespace locc
