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

		// 조건변수를 활용할 때 사용
		m_condvar.notify_one();
	}

	// 비효율적으로 계속 while돌면서 꺼내쓰기를 기다릴 수 있다.
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(m_mutex);
		if (m_stack.empty())
			return false;
		
		value = std::move(m_stack.top());
		m_stack.pop();
		return true;
	}

	// 조건변수를 활용해서 개선
	void WaitPop(T& value)
	{
		// 조건 변수 활용할 때는 내부적으로 락을 풀기도 해야하기 때문에 unique_lock 사용
		unique_lock<mutex> lock(m_mutex);
		m_condvar.wait(lock, [this] {return m_stack.empty() == false; }); // 스택에 데이터가 있을때까지 대기

		value = std::move(m_stack.pop());
		m_stack.pop();
	}

private:
	stack<T> m_stack;
	mutex m_mutex;

	condition_variable m_condvar;
};
