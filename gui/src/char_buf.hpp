#pragma once
#include <array>
#include <cstring>
#include <string_view>

namespace locc::gui {
template <std::size_t N = 128>
using CharBufN = std::array<char, N>;

using CharBuf = CharBufN<>;

template <std::size_t N>
constexpr void copy_to(CharBufN<N>& out, std::string_view const text) {
	if (text.empty()) { return; }
	auto length = text.size();
	if (text.size() >= N) { length = N - 1; }
	std::memcpy(out.data(), text.data(), length);
}
} // namespace locc::gui
