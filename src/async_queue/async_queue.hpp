#pragma once
#include <condition_variable>
#include <mutex>
#include <vector>

namespace kt
{
template <typename Mutex = std::mutex>
struct lockable final
{
	Mutex mutex;

	template <template <typename...> typename Lock = std::scoped_lock>
	Lock<decltype(mutex)> lock()
	{
		return Lock<decltype(mutex)>(mutex);
	}
};

template <typename T, typename Mutex = std::mutex>
class async_queue
{
public:
	using type = T;
	using sync = Mutex;

protected:
	std::condition_variable m_cv;
	std::vector<T> m_queue;
	mutable lockable<Mutex> m_mutex;
	bool m_active = true;

public:
	async_queue() = default;
	virtual ~async_queue()
	{
		clear();
	}

public:
	template <typename F>
	void push(F&& task)
	{
		{
			auto lock = m_mutex.lock();
			if (m_active)
			{
				m_queue.push_back(std::forward<F>(task));
			}
		}
		m_cv.notify_one();
	}

	T pop()
	{
		auto lock = m_mutex.template lock<std::unique_lock>();
		m_cv.wait(lock, [this]() -> bool { return !m_queue.empty() || !m_active; });
		if (!m_queue.empty())
		{
			auto ret = std::move(m_queue.back());
			m_queue.pop_back();
			return ret;
		}
		return {};
	}

	std::vector<T> clear()
	{
		std::vector<T> ret;
		{
			auto lock = m_mutex.lock();
			ret = std::move(m_queue);
			m_queue.clear();
			m_active = false;
		}
		m_cv.notify_all();
		return ret;
	}

	bool empty() const
	{
		auto lock = m_mutex.lock();
		return m_queue.empty();
	}

	bool active() const
	{
		auto lock = m_mutex.lock();
		return m_active;
	}

	void active(bool set)
	{
		auto lock = m_mutex.lock();
		m_active = set;
		m_cv.notify_all();
	}
};
} // namespace kt
