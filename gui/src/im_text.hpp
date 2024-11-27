#pragma once
#include <imgui.h>
#include <char_buf.hpp>
#include <format>

namespace locc::gui {
template <std::size_t BufSize = 1024, typename... Args>
void im_text(std::format_string<Args...> fmt, Args&&... args) {
	static_assert(BufSize > 0);
	auto buf = CharBufN<BufSize>{};
	std::format_to_n(buf.data(), buf.size() - 1, fmt, std::forward<Args>(args)...);
	ImGui::Text("%s", buf.data()); // NOLINT(cppcoreguidelines-pro-type-vararg)
}
} // namespace locc::gui
