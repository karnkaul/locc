#pragma once
#include <imgui.h>
#include <array>
#include <format>

namespace locc::gui {
template <std::size_t BufSize = 1024, typename... Args>
void im_text(std::format_string<Args...> fmt, Args&&... args) {
	auto buf = std::array<char, BufSize>{};
	std::format_to_n(buf.data(), buf.size(), fmt, std::forward<Args>(args)...);
	ImGui::Text("%s", buf.data()); // NOLINT(cppcoreguidelines-pro-type-vararg)
}
} // namespace locc::gui
