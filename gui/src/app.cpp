#include <app.hpp>
#include <cstdlib>
#include <print>

namespace locc::gui {
namespace {
[[nodiscard]] constexpr auto sort_by_str(LineCount::Metric const metric) -> std::string_view {
	if (metric == Counter::sort_by_file_type_v) { return "File Type"; }
	assert(metric < LineCount::COUNT_);
	return metric_name_v[std::size_t(metric)]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}
} // namespace

App::App() : m_instance(m_queue, InstanceCreateInfo{.file_filter = &m_filter}) {
	for (auto const pattern : m_filter.exclude_patterns) { m_exclude_patterns.emplace_back(pattern); }
	set_filter();
}

auto App::run() -> int {
	auto window = gvdi::Context::create_window({640.0f, 360.0f}, "locc");
	if (!window) {
		std::println(stderr, "Failed to create window");
		return EXIT_FAILURE;
	}

	if (!create_context(std::move(window))) {
		std::println(stderr, "Failed to create gvdi Context");
		return EXIT_FAILURE;
	}

	while (m_context->next_frame()) {
		update();
		m_context->render({0.0f, 0.0f, 0.0f, 0.0f});
	}

	m_queue.drop_enqueued();
	m_queue.drain_and_wait();

	return EXIT_SUCCESS;
}

auto App::create_context(gvdi::UniqueWindow window) -> bool {
	glfwSetWindowUserPointer(window.get(), this);

	glfwSetDropCallback(window.get(), [](GLFWwindow* w, int const count, char const** paths) { self(w).on_drop({paths, std::size_t(count)}); });

	try {
		m_context = std::make_unique<gvdi::Context>(std::move(window));
	} catch (std::exception const& e) {
		std::println("FATAL: {}", e.what());
		return false;
	}

	return true;
}

void App::on_drop(std::span<char const*> paths) {
	assert(!m_instance.get_grammars().empty());
	for (auto const* path : paths) {
		auto line_counter = m_instance.start_count(path);
		if (!line_counter) {
			// TODO: fail popup
			continue;
		}

		m_counter.start(path, std::move(line_counter));
		break;
	}
}

void App::update() {
	ImGui::SetNextWindowSize(m_context->get_framebuffer_size(), ImGuiCond_Always);
	ImGui::SetNextWindowPos({}, ImGuiCond_Always);
	static constexpr auto main_flags_v = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
										 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGui::Begin("Main", nullptr, main_flags_v);

	update_header();
	m_counter.update();

	ImGui::End();
	ImGui::ShowDemoWindow();
}

void App::update_header() {
	ImGui::TextUnformatted(m_counter.get_path());
	ImGui::Checkbox("progress", &m_counter.show_progress);
	ImGui::SameLine();
	ImGui::Checkbox("totals", &m_counter.show_totals);

	ImGui::SameLine();
	update_sorting();

	update_filter();

	ImGui::Separator();
}

void App::update_sorting() {
	auto const prev_sort_by = m_counter.sort_by;
	auto const prev_sort_ascend = m_counter.sort_ascend;

	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::BeginCombo("Sort By", sort_by_str(m_counter.sort_by).data())) {
		if (ImGui::Selectable(sort_by_str(Counter::sort_by_file_type_v).data())) { m_counter.sort_by = Counter::sort_by_file_type_v; }
		for (std::size_t i = 0; i < LineCount::COUNT_; ++i) {
			if (ImGui::Selectable(metric_name_v.at(i).data())) { m_counter.sort_by = LineCount::Metric{i}; }
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Checkbox("ascending", &m_counter.sort_ascend);

	if (prev_sort_ascend != m_counter.sort_ascend || prev_sort_by != m_counter.sort_by) { m_counter.sort_rows(); }
}

void App::update_filter() {
	if (ImGui::TreeNode("Exclude patterns")) {
		for (auto it = m_exclude_patterns.begin(); it != m_exclude_patterns.end(); ++it) {
			ImGui::TextUnformatted(it->c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(200.0f);
			auto label = std::array<char, 32>{};
			std::format_to_n(label.data(), label.size(), "Remove##{}", std::size_t(it - m_exclude_patterns.begin()));
			if (ImGui::SmallButton(label.data())) {
				m_exclude_patterns.erase(it);
				set_filter();
				break;
			}
		}

		ImGui::SetNextItemWidth(180.0f);
		ImGui::InputText("##Add", m_exclude_buf.data(), m_exclude_buf.size());
		ImGui::SameLine();
		if (ImGui::SmallButton("Add")) {
			m_exclude_patterns.emplace_back(m_exclude_buf.data());
			m_exclude_buf = {};
			set_filter();
		}
		ImGui::TreePop();
	}
}

void App::set_filter() {
	m_filter.exclude_patterns.clear();
	for (auto const& pattern : m_exclude_patterns) { m_filter.exclude_patterns.push_back(pattern); }
}
} // namespace locc::gui
