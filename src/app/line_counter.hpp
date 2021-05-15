#pragma once
#include <functional>
#include <app/common.hpp>
#include <kt/async_queue/async_queue.hpp>
#include <kt/async_queue/lockable.hpp>
#include <kt/kthread/kthread.hpp>

namespace locc {
struct result_t;

class async_counter {
  public:
	async_counter(result_t* result);
	~async_counter();

	void count(file_t file, bool blanks);

	result_t* m_result{};

  private:
	struct worker;
	using async_queue = kt::async_queue<std::function<void()>>;

	void count_impl(file_t file, bool blanks);

	async_queue m_queue;
	kt::lockable_t<> m_mutex;
	std::vector<worker> m_workers;
};
} // namespace locc
