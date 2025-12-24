#include "locc/instance.hpp"
#include "detail/config.hpp"
#include "detail/executor.hpp"
#include "locc/logger.hpp"
#include <klib/task/queue.hpp>
#include <algorithm>

namespace locc {
namespace detail {
namespace {
[[nodiscard, maybe_unused]] constexpr auto test(Flag const flags, Flag const mask) -> bool { return (flags & mask) == mask; }

[[nodiscard, maybe_unused]] auto serialize_flags(Flag const flags) -> std::string {
	auto ret = std::string{};
	if (flags == Flag::None) { return "None"; }
	if (test(flags, Flag::NoDefaultCodeFamilies)) { ret += "NoDefaultCodeFamilies|"; }
	if (test(flags, Flag::NoDefaultTextCategories)) { ret += "NoDefaultTextCategories|"; }
	if (test(flags, Flag::NoDefaultExcludeSuffixes)) { ret += "NoDefaultExcludeSuffixes|"; }
	ret.pop_back();
	return ret;
}

class Instance : public locc::Instance {
  public:
	explicit Instance(InitInfo info) {
		auto const thread_count = std::clamp(info.thread_count, ThreadCount{0}, get_max_threads());
		if (thread_count > ThreadCount{0}) {
			auto const queue_ci = klib::task::QueueCreateInfo{
				.thread_count = klib::task::ThreadCount(thread_count),
			};
			m_task_queue = std::make_unique<klib::task::Queue>(queue_ci);
		}

		[[maybe_unused]] auto const flags = info.flags;
		m_config = Config::build(std::move(info));

		if constexpr (klib::log::debug_enabled_v) {
			m_log.info("threads: {}, Heuristic: {}, max_depth: {}, flags: {}", int(get_thread_count()), heuristic_name_v[m_config.heuristic],
					   m_config.max_depth, serialize_flags(flags));
		}
	}

  private:
	[[nodiscard]] auto get_thread_count() const -> ThreadCount final {
		if (!m_task_queue) { return ThreadCount{0}; }
		return ThreadCount(m_task_queue->thread_count());
	}

	[[nodiscard]] auto process(std::string_view const path) -> Result final {
		auto executor = detail::Executor{m_task_queue.get(), m_config};
		return executor.execute(path);
	}

	Logger<Instance> m_log{};

	std::unique_ptr<klib::task::Queue> m_task_queue{};
	Config m_config{};
};
} // namespace
} // namespace detail

auto Instance::create(InitInfo init_info) -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(std::move(init_info)); }
} // namespace locc

auto locc::get_max_threads() -> ThreadCount { return ThreadCount(klib::task::get_max_threads()); }
