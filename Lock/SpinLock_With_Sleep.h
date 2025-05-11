#pragma once
#include <mutex>
#include <thread>
#include <atomic>
using namespace std;

/*
----------------------------------------------------------------------------------------------------
SpinLock_With_Sleep

�ٽ� ����
    - ���� SpinLockBase������ ������ CPU ����� �����ϱ� ���� ��� �� �ϳ�
    - this_thread::sleep_for 
    - this_thread::yield (����� == this_thread::sleep_for(0)�� ����.

�ּ� ����
    - �⺻���� ������ SpinLockBase �ڵ忡 �ּ� �޾� ����.

����� ����
    - �ᱹ CPU �ݳ��� �Ѵٴ°�... ���ؽ�Ʈ ����Ī ����
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

        // ���1. �����ϸ� �ٷ� CPU �ݳ��ϴ� ���
        ///*
            while (m_bLocked.compare_exchange_strong(expected, desired) == false)
            {
                expected = false; 

                // this_thread::sleep_for(std::chrono::milliseconds(100)); (== this_thread::sleep_for(100ms))
                this_thread::yield(); // (== this_thread::sleep_for(0))
            }
        //*/


        // ���2. ��å�� ���� N�������� ���� SpinLockó�� ���ٰ� �����ϸ� Sleep �ϴ� ���ó�� �ص��ȴ�.
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


        // ���3. �� �����Ƽ� ������ backoff ����. - ���� Ƚ���� ���������� ���� �������� ������ ������ �ð� ���� �Ͻ� �ߴ�(suspend)�ϰ�, CPU�� �ݳ��Ͽ� �ٽ� �����층 ����� �ǵ��� ��
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
                    sleepTimsMs = min(sleepTimsMs * 2, 100); // �ִ� 100ms
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