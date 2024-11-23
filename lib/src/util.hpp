#pragma once
#include <string_view>

namespace locc {
template <typename Type, std::same_as<Type>... Ts>
constexpr auto match_any(Type const& t, Ts const&... ts) {
	return (... || (t == ts));
}

constexpr auto is_space(char const c) -> bool { return match_any(c, ' ', '\t', '\n'); }

constexpr auto trim_front(std::string_view input) -> std::string_view {
	while (!input.empty() && is_space(input.front())) { input = input.substr(1); }
	return input;
}

constexpr auto trim_back(std::string_view input) -> std::string_view {
	while (!input.empty() && is_space(input.back())) { input = input.substr(0, input.size() - 1); }
	return input;
}

constexpr auto trim_spaces(std::string_view input) -> std::string_view { return trim_front(trim_back(input)); }
} // namespace locc
