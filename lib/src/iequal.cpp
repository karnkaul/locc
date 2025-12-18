#include "locc/iequal.hpp"
#include <algorithm>
#include <cctype>

auto locc::char_iequal(char const lhs, char const rhs) -> bool {
	return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs));
}

auto locc::iequal(std::string_view const lhs, std::string_view const rhs) -> bool { return std::ranges::equal(lhs, rhs, char_iequal); }
