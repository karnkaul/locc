#pragma once
#include <config.hpp>
#include <counter.hpp>
#include <gvdi/app.hpp>
#include <klib/task/queue.hpp>
#include <locc/file_filter.hpp>
#include <locc/instance.hpp>

namespace locc::gui {
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class App : public gvdi::App {
  public:
	explicit App();

	auto operator=(App&&) = delete;

  private:
	static auto self(GLFWwindow* window) -> App& { return *static_cast<App*>(glfwGetWindowUserPointer(window)); }

	auto create_window() -> GLFWwindow* final;
	void update() final;
	void post_run() final;

	[[nodiscard]] auto framebuffer_size() const -> ImVec2;

	void on_drop(std::span<char const*> paths);

	void update_header();
	void update_sorting();

	void set_filter();
	void set_config();
	void reload(bool copy_config);

	DefaultFileFilter m_filter{};

	Config m_config{};
	Config::Modal m_config_modal{};

	Counter m_counter{}; // tasks must outlive queue

	klib::task::Queue m_queue{};
	Instance m_instance;
};
} // namespace locc::gui
