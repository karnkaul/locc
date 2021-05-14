#pragma once
#include <app/common.hpp>
#include <kt/async_queue/lockable.hpp>

namespace locc {
struct result_t;

class counter_t {
  public:
	counter_t(result_t* result);
	~counter_t();

	void count(file_t file, bool blanks);

	result_t* m_result{};

  private:
	void count_impl(file_t file, bool blanks);

	kt::lockable_t<> m_mutex;
};
} // namespace locc
