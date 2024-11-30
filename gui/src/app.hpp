#pragma once
#include <config.hpp>
#include <counter.hpp>

#include <locc/file_filter.hpp>
#include <locc/instance.hpp>

#include <gvdi/context.hpp>
#include <klib/task/queue.hpp>

#include <memory>

namespace locc::gui {
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class App {
  public:
	explicit App();

	auto operator=(App&&) = delete;

	auto run() -> int;

  private:
	static auto self(GLFWwindow* window) -> App& { return *static_cast<App*>(glfwGetWindowUserPointer(window)); }

	auto create_context(gvdi::UniqueWindow window) -> bool;

	void on_drop(std::span<char const*> paths);

	void update();

	void update_header();
	void update_sorting();

	void set_filter();
	void set_config();
	void reload(bool copy_config);

	std::unique_ptr<gvdi::Context> m_context{};

	DefaultFileFilter m_filter{};

	Config m_config{};
	Config::Modal m_config_modal{};

	Counter m_counter{}; // tasks must outlive queue

	klib::task::Queue m_queue{};
	Instance m_instance;
};
} // namespace locc::gui
