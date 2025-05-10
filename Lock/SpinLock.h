#pragma once

#include <mutex>
#include <thread>
#include <atomic>
using namespace std;

/*
----------------------------------------------------------------------------------------------------
SpinLockBase

핵심 개념
    - 잠금 기능을 atomic한 변수로 선언
    - 열린 상태를 확인하고(1) 잠그는(2) 과정을 하나의 행동으로 표현 필요. - CAS 개념

기능상 결함 
    - 만약 해당 자원을 점유하고 오랫동안 일을 하는 쓰레드가 있다면? 
        -> while문을 돌면서 무한정 대기 상태 즉, 쓸대없이 CPU 과한 사용

해결 방안 
    - CPU가 쉴 수 있는 방식 채택
    - 비동기 방식 채택
----------------------------------------------------------------------------------------------------
*/
class SpinLockBase
{
public:
    SpinLockBase() = default;
    ~SpinLockBase() = default;
    SpinLockBase(const SpinLockBase& c) = delete;
    SpinLockBase& operator=(const SpinLockBase&) = delete;

public:
    void lock()
    {
        // 다음 주석 처리된 코드가 한방에 처리되어야 멀티 쓰레드 환경에서 동시에 점유하는 현상을 막을 수 있다.
        /*
            while (m_bLocked)
            {

            }

            m_bLocked = true;
        */

        // CAS (Compare-And-Swap)
        bool expected = false;
        bool desired = true;

        // m_bLocked가 기댓값(expected)와 값이 같다면 m_bLocked를 요구값(desired)로 바꾸어 주는 기능
        while (m_bLocked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false; // 왜 false로 바꾸어주는걸까? 실패했다면 내부에서 expected는 true로 바뀌어 있기 때문이다.
        }
        // CAS 의사 코드
        /*
            if (m_bLocked == expected)
            {
                expected = m_bLocked;
                m_bLocked = desired;
                return true;
            }
            else
            {
                expected = m_bLocked;
                return false;
            }
        */
    }

    void unlock()
    {
        m_bLocked.store(false);
    }

private:
    // volatile/*컴파일러가 최적화 하지 않도록*/ bool m_bLocked = false;
    atomic<bool> m_bLocked = false; // atomic 자체가 volatile 기능을 포함하고 있다고 생각하면 된다.
};