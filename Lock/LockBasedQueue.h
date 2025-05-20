#pragma once
#include <mutex>
#include <queue>
using namespace std;

template<typename T>
class LockQueue
{
public:
	LockQueue() = default;
	~LockQueue() = default;
	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

public:
	void Push(T value)
	{
		lock_guard<mutex> lock(m_mutex);
		m_queue.push(value);
		m_condvar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_queue.empty())
			return false;

		value = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(m_mutex);
		m_condvar.wait(lock, [this] { return m_queue.empty() == false; });

		value = std::move(m_queue.front());
		m_queue.pop();
	}


private:
	queue<T> m_queue;
	mutex m_mutex;
	condition_variable m_condvar;
};