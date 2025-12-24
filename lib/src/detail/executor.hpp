#pragma once
#include "detail/counter.hpp"
#include "detail/workspace.hpp"
#include "locc/result.hpp"
#include <klib/task/queue_fwd.hpp>

namespace locc::detail {
class Executor {
  public:
	explicit Executor(klib::task::Queue* queue, Config const& config);

	auto execute(std::string_view path) -> Result;

  private:
	struct Job {
		std::unique_ptr<Counter> counter{};
		Workspace::Entry* entry{}; // must only be accessed on main thread.
	};

	void process_file(fs::path path);
	void enqueue_job(fs::path path, Workspace::Entry& entry);
	void walk_directory(fs::path const& path, int depth);
	void prepare_result();

	[[nodiscard]] auto create_counter(fs::path path, Workspace::Entry const& entry) const -> std::unique_ptr<Counter>;

	klib::task::Queue* m_queue{};
	Heuristic m_heuristic{};

	Workspace m_workspace;

	std::chrono::steady_clock::time_point m_start_time{};
	std::vector<Job> m_jobs{};
	Result m_result{};
};
} // namespace locc::detail
