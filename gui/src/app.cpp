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
	m_config.exclude_patterns.reserve(m_filter.exclude_patterns.size());
	for (auto const pattern : m_filter.exclude_patterns) { copy_to(m_config.exclude_patterns.emplace_back(), pattern); }
	set_filter();
	set_config();
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
	glfwSetKeyCallback(window.get(), [](GLFWwindow* w, int key, int /*code*/, int action, int mods) {
		if (key == GLFW_KEY_C && action == GLFW_RELEASE && mods == 0) { self(w).m_config_modal.open_on_next_update(self(w).m_config); }
	});

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
	auto const config_saved = m_config_modal.update();

	ImGui::End();

	if (config_saved) { reload(true); }
}

void App::update_header() {
	ImGui::TextUnformatted(m_counter.get_path());
	ImGui::Checkbox("Progress", &m_counter.show_progress);
	ImGui::SameLine();
	ImGui::Checkbox("Totals", &m_counter.show_totals);

	ImGui::SameLine();
	update_sorting();

	ImGui::SameLine();
	if (ImGui::Button("Config")) { m_config_modal.open_on_next_update(m_config); }
	ImGui::SameLine();
	if (ImGui::Button("Reload")) { reload(false); }

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
	ImGui::Checkbox("Ascending", &m_counter.sort_ascend);

	if (prev_sort_ascend != m_counter.sort_ascend || prev_sort_by != m_counter.sort_by) { m_counter.sort_rows(); }
}

void App::set_filter() {
	m_filter.exclude_patterns.clear();
	m_filter.exclude_patterns.reserve(m_config.exclude_patterns.size());
	for (auto const& pattern : m_config.exclude_patterns) { m_filter.exclude_patterns.emplace_back(pattern.data()); }
}

void App::set_config() { m_config.thread_count = m_queue.thread_count(); }

void App::reload(bool const copy_config) {
	m_queue.drop_enqueued();
	m_queue.drain_and_wait();
	m_counter = {};

	if (copy_config) { m_config = m_config_modal.config; }

	m_queue.~Queue();
	new (&m_queue) ktask::Queue{ktask::QueueCreateInfo{.thread_count = m_config.thread_count}};

	set_filter();
	set_config();
}
} // namespace locc::gui
