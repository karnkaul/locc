#pragma once
#include <string_view>

namespace locc {
/// \brief Case-insensitive character comparison.
auto char_iequal(char lhs, char rhs) -> bool;
/// \brief Case-insensitive string comparison.
auto iequal(std::string_view lhs, std::string_view rhs) -> bool;
} // namespace locc
