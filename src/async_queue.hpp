#pragma once
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

namespace locc
{
struct lockable final
{
	std::mutex mutex;

	template <template <typename...> typename Lock = std::scoped_lock>
	Lock<decltype(mutex)> lock()
	{
		return Lock<decltype(mutex)>(mutex);
	}
};

class async_queue final
{
private:
	std::condition_variable m_cv;
	std::deque<std::function<void()>> m_queue;
	mutable lockable m_mutex;
	bool m_bWork = true;

public:
	template <typename F>
	void push(F&& task)
	{
		{
			auto lock = m_mutex.lock();
			m_queue.push_back(std::forward<F>(task));
		}
		m_cv.notify_one();
	}

	std::function<void()> pop()
	{
		auto lock = m_mutex.lock<std::unique_lock>();
		m_cv.wait(lock, [this]() -> bool { return !m_queue.empty() || !m_bWork; });
		if (!m_queue.empty())
		{
			auto ret = std::move(m_queue.back());
			m_queue.pop_back();
			return ret;
		}
		return {};
	}

	std::deque<std::function<void()>> flush()
	{
		std::deque<std::function<void()>> ret;
		{
			auto lock = m_mutex.lock();
			ret = std::move(m_queue);
			m_queue.clear();
			m_bWork = false;
		}
		m_cv.notify_all();
		return ret;
	}

	bool empty() const
	{
		auto lock = m_mutex.lock();
		return m_queue.empty();
	}
};
} // namespace locc
