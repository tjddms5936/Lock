#pragma once

#include <mutex>
#include <thread>
#include <atomic>
using namespace std;

/*
----------------------------------------------------------------------------------------------------
SpinLockBase

�ٽ� ����
    - ��� ����� atomic�� ������ ����
    - ���� ���¸� Ȯ���ϰ�(1) ��״�(2) ������ �ϳ��� �ൿ���� ǥ�� �ʿ�. - CAS ����

��ɻ� ���� 
    - ���� �ش� �ڿ��� �����ϰ� �������� ���� �ϴ� �����尡 �ִٸ�? 
        -> while���� ���鼭 ������ ��� ���� ��, ������� CPU ���� ���

�ذ� ��� 
    - CPU�� �� �� �ִ� ��� ä��
    - �񵿱� ��� ä��
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
        // ���� �ּ� ó���� �ڵ尡 �ѹ濡 ó���Ǿ�� ��Ƽ ������ ȯ�濡�� ���ÿ� �����ϴ� ������ ���� �� �ִ�.
        /*
            while (m_bLocked)
            {

            }

            m_bLocked = true;
        */

        // CAS (Compare-And-Swap)
        bool expected = false;
        bool desired = true;

        // m_bLocked�� ���(expected)�� ���� ���ٸ� m_bLocked�� �䱸��(desired)�� �ٲپ� �ִ� ���
        while (m_bLocked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false; // �� false�� �ٲپ��ִ°ɱ�? �����ߴٸ� ���ο��� expected�� true�� �ٲ�� �ֱ� �����̴�.
        }
        // CAS �ǻ� �ڵ�
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
    // volatile/*�����Ϸ��� ����ȭ ���� �ʵ���*/ bool m_bLocked = false;
    atomic<bool> m_bLocked = false; // atomic ��ü�� volatile ����� �����ϰ� �ִٰ� �����ϸ� �ȴ�.
};