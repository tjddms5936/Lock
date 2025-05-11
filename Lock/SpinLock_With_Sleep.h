#pragma once
#include <mutex>
#include <thread>
#include <atomic>
using namespace std;

/*
----------------------------------------------------------------------------------------------------
SpinLock_With_Sleep

핵심 개념
    - 기존 SpinLockBase에서의 무한정 CPU 사용을 방지하기 위한 방법 중 하나
    - this_thread::sleep_for 
    - this_thread::yield (개념상 == this_thread::sleep_for(0)과 같다.

주석 제거
    - 기본적인 개념은 SpinLockBase 코드에 주석 달아 놓음.

기능적 결함
    - 결국 CPU 반납을 한다는건... 컨텍스트 스위칭 유발
----------------------------------------------------------------------------------------------------
*/
class SpinLock_With_Sleep
{
public:
    SpinLock_With_Sleep() = default;
    ~SpinLock_With_Sleep() = default;
    SpinLock_With_Sleep(const SpinLock_With_Sleep& c) = delete;
    SpinLock_With_Sleep& operator=(const SpinLock_With_Sleep&) = delete;

public:
    void lock()
    {
        bool expected = false;
        bool desired = true;

        // 방법1. 실패하면 바로 CPU 반납하는 방식
        ///*
            while (m_bLocked.compare_exchange_strong(expected, desired) == false)
            {
                expected = false; 

                // this_thread::sleep_for(std::chrono::milliseconds(100)); (== this_thread::sleep_for(100ms))
                this_thread::yield(); // (== this_thread::sleep_for(0))
            }
        //*/


        // 방법2. 정책에 따라 N번까지는 기존 SpinLock처럼 돌다가 실패하면 Sleep 하는 방식처럼 해도된다.
        /*
            int count = 5000;
            while (m_bLocked.compare_exchange_strong(expected, desired) == false)
            {
                expected = false;
                --count;
                if (count <= 0)
                {
                    count = 5000;
                    this_thread::yield(); // (== this_thread::sleep_for(0))
                }
            }
        */


        // 방법3. 더 나가아서 점진적 backoff 전략. - 실패 횟수가 많아질수록 현재 스레드의 실행을 지정한 시간 동안 일시 중단(suspend)하고, CPU를 반납하여 다시 스케쥴링 대상이 되도록 함
        /*
            int count = 5000;
            while (m_bLocked.compare_exchange_strong(expected, desired) == false)
            {
                expected = false;
                --count;
                if (count <= 0)
                {
                    count = 5000;
                    static thread_local int sleepTimsMs = 1;
                    this_thread::sleep_for(chrono::milliseconds(sleepTimsMs));
                    sleepTimsMs = min(sleepTimsMs * 2, 100); // 최대 100ms
                }
            }
        */
    }

    void unlock()
    {
        m_bLocked.store(false);
    }

private:
    atomic<bool> m_bLocked = false;
};