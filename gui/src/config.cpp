#include <imgui.h>
#include <config.hpp>
#include <klib/task/queue.hpp>
#include <cstring>

namespace locc::gui {
Config::Modal::Modal() : m_thread_count(int(klib::task::Queue::get_max_threads())) {}

void Config::Modal::open_on_next_update(Config config) {
	this->config = std::move(config);
	m_open_popup = true;
}

auto Config::Modal::update() -> bool {
	auto ret = false;

	if (m_open_popup) {
		m_open_popup = false;
		ImGui::OpenPopup(label_v.data());
	}

	auto const* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Once, {0.5f, 0.5f});
	if (ImGui::BeginPopupModal(label_v.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		inspect();
		ImGui::Separator();

		if (ImGui::Button("Save")) {
			ret = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }

		ImGui::TextUnformatted("Note: saving will reload the app");

		ImGui::EndPopup();
	}

	return ret;
}

void Config::Modal::inspect() {
	auto const max_threads = int(klib::task::Queue::get_max_threads());
	m_thread_count = int(config.thread_count);
	ImGui::SetNextItemWidth(140.0f);
	if (ImGui::SliderInt("Threads", &m_thread_count, 2, max_threads)) { config.thread_count = klib::task::ThreadCount(m_thread_count); }

	if (ImGui::BeginListBox("Exclude", {140.0f, 100.0f})) {
		if (m_selected_exclude && *m_selected_exclude >= config.exclude_patterns.size()) { m_selected_exclude.reset(); }
		std::string_view const selected = m_selected_exclude ? config.exclude_patterns[*m_selected_exclude].data() : "";
		for (std::size_t i = 0; i < config.exclude_patterns.size(); ++i) {
			auto const& pattern = config.exclude_patterns[i];
			if (ImGui::Selectable(pattern.data(), std::string_view{pattern.data()} == selected)) { m_selected_exclude = i; }
		}
		ImGui::EndListBox();
	}

	if (m_selected_exclude && ImGui::Button("Remove")) {
		config.exclude_patterns.erase(config.exclude_patterns.begin() + std::ptrdiff_t(*m_selected_exclude));
		m_selected_exclude.reset();
	}

	ImGui::SetNextItemWidth(140.0f);
	ImGui::InputText("##Add", m_exclude_buf.data(), m_exclude_buf.size());
	ImGui::SameLine();
	if (ImGui::SmallButton("Add")) {
		config.exclude_patterns.push_back(m_exclude_buf);
		m_exclude_buf = {};
	}
}
} // namespace locc::gui
