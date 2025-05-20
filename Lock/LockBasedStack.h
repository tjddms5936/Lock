#pragma once
#include <mutex>
#include <stack>
using namespace std;


template<typename T> 
class LockStack
{
public:
	LockStack() = default;
	~LockStack() = default;
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;
	
public:
	void Push(T value)
	{
		lock_guard<mutex> lock(m_mutex);
		m_stack.push(std::move(value));

		// ���Ǻ����� Ȱ���� �� ���
		m_condvar.notify_one();
	}

	// ��ȿ�������� ��� while���鼭 �������⸦ ��ٸ� �� �ִ�.
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_stack.empty())
			return false;
		
		value = std::move(m_stack.top());
		m_stack.pop();
		return true;
	}

	// ���Ǻ����� Ȱ���ؼ� ����
	void WaitPop(T& value)
	{
		// ���� ���� Ȱ���� ���� ���������� ���� Ǯ�⵵ �ؾ��ϱ� ������ unique_lock ���
		unique_lock<mutex> lock(m_mutex);
		m_condvar.wait(lock, [this] {return m_stack.empty() == false; }); // ���ÿ� �����Ͱ� ���������� ���

		value = std::move(m_stack.pop());
		m_stack.pop();
	}

private:
	stack<T> m_stack;
	mutex m_mutex;

	condition_variable m_condvar;
};
