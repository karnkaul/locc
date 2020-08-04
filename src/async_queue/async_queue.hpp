#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>

namespace kt
{
///
/// \brief Wrapper for a mutex and associated locks
///
template <typename Mutex = std::mutex>
struct lockable final
{
	Mutex mutex;

	///
	/// \brief Obtain a Lock on mutex
	///
	template <template <typename...> typename Lock = std::scoped_lock>
	Lock<Mutex> lock()
	{
		return Lock<Mutex>(mutex);
	}
};

///
/// \brief FIFO std::deque wrapper with thread safe API
///
/// Features:
/// 	- Thread-safe push-and-notify
/// 	- Thread-safe wait-and-pop
/// 	- Clear queue and return residue
/// 	- Deactivate queue (as secondary wait condition)
///
template <typename T, typename Mutex = std::mutex>
class async_queue
{
public:
	using type = T;
	using sync = Mutex;

protected:
	///
	/// \brief Used to wait on pop() and notify on push()/clear()
	///
	std::condition_variable m_cv;
	std::deque<T> m_queue;
	mutable lockable<Mutex> m_mutex;
	///
	/// \brief Used to halt / deactivate queue
	///
	bool m_active = true;

public:
	///
	/// \brief Default constructor
	///
	async_queue() = default;
	///
	/// \brief Polymorhic destructor
	///
	virtual ~async_queue()
	{
		clear();
	}

public:
	///
	/// \brief Forward a T to the back of the queue and notify one
	///
	void push(T&& t);
	///
	/// \brief Pop a T from the front of the queue, wait until populated / not active
	///
	T pop();
	///
	/// \brief Flush the queue, notify all, and obtain any residual items
	/// \param active Set m_active after moving items
	/// \returns Residual items that were still in the queue
	///
	std::deque<T> clear(bool active = false);
	///
	/// \brief Check whether queue is empty
	///
	bool empty() const;
	///
	/// \brief Check whether queue is active
	///
	bool active() const;
	///
	/// \brief Set active/inactive
	///
	void active(bool set);
};

template <typename T, typename Mutex>
void async_queue<T, Mutex>::push(T&& t)
{
	{
		auto lock = m_mutex.lock();
		if (m_active)
		{
			m_queue.push_back(std::forward<T>(t));
		}
	}
	m_cv.notify_one();
}

template <typename T, typename Mutex>
T async_queue<T, Mutex>::pop()
{
	auto lock = m_mutex.template lock<std::unique_lock>();
	m_cv.wait(lock, [this]() -> bool { return !m_queue.empty() || !m_active; });
	if (!m_queue.empty())
	{
		auto ret = std::move(m_queue.front());
		m_queue.pop_front();
		return ret;
	}
	return {};
}

template <typename T, typename Mutex>
std::deque<T> async_queue<T, Mutex>::clear(bool active)
{
	decltype(m_queue) ret;
	{
		auto lock = m_mutex.lock();
		ret = std::move(m_queue);
		m_queue.clear();
		m_active = active;
	}
	m_cv.notify_all();
	return ret;
}

template <typename T, typename Mutex>
bool async_queue<T, Mutex>::empty() const
{
	auto lock = m_mutex.lock();
	return m_queue.empty();
}

template <typename T, typename Mutex>
bool async_queue<T, Mutex>::active() const
{
	auto lock = m_mutex.lock();
	return m_active;
}

template <typename T, typename Mutex>
void async_queue<T, Mutex>::active(bool set)
{
	auto lock = m_mutex.lock();
	m_active = set;
	m_cv.notify_all();
}
} // namespace kt
