#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <Windows.h>

using namespace std;
/*
----------------------------------------------------------------------------------------------------
Condition Variable Ȱ��

�ٽ� ����
    - ���� ���� ������Ʈ Ȱ��
    - Event�� ����� ������ Ȱ���ؼ� ���
    - lock�̶� ¦�� ��� ���

��� ���
    - 1. Lock�� ���
    - 2. ���� ���� ���� ����
    - 3. Lock�� Ǯ��
    - 4. ���Ǻ��� ���� �ٸ� �����忡�� ����

����
    - Ŀ�� ������Ʈ���� Ȱ������ �ʾƵ� �ȴ�.



�������ؼ��� �̰ɷ� �������!!!!!!!!!!!!

----------------------------------------------------------------------------------------------------
*/

// ������ ������ ����.


mutex m;
queue<int> qData;
HANDLE handle;

condition_variable cv;


void Producer();
void Consumer();

int main()
{
    handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    thread t1(Producer);
    thread t2(Consumer);

    t1.join();
    t2.join();
}

void Producer()
{
    while (true)
    {
        {
            unique_lock<mutex> lock(m);
            qData.push(100);
        }

        cv.notify_one(); // wait���� �����尡 ������ �� 1���� �����.
        this_thread::sleep_for(10000ms);
    }

}

void Consumer()
{
    while (true)
    {
        unique_lock<mutex> lock(m);

        cv.wait(lock, []() { return qData.empty() == false;  }); // �ι�° �Ű������� ���� �־ ��. �����̶�� �����ϸ� �ȴ�.
        // ������ ���� ������ �޴´�. 
        /*
            1. Lock�� ���
            2. ������ Ȯ���Ѵ� (�Ű������� ���� ���� Ȱ��)
                -> ����O : ���� ���ͼ� �̾ �ڵ� ����
                -> ����X : Lock�� Ǯ���ְ� ��� ���·� ����
                    --> �� ��ɶ����� lock_guard�� �ƴ� unique_lock�� Ȱ���ؾ� �Ѵ�.
                    --> �׸��� ������ notify_one�� �����ָ� �ٽ� 1������ �����Ѵ�.

            ���ǹ��� : ������ notify_one �ߴٴ°�... �׻� ���ǽ� �����ϴ°� �ƴϾ�?! �����͸� ���� ������ notify_one�ϴ°��ݾ�.
                : Spurious Wakeup (��¥ ���)
                : notify_one�Ҷ� lock�� ��� �ִ� ���°� �ƴϴ�.�׷��� ������ C�����尡 �����͸� ���� ���´ٸ�? B�������� ���� �˻縦 ������� ���� �� �ִ�.
                : ��, ������� �����带 ����� ����! lock�� Ǭ �����̱� ������ �ٸ������� �����͸� �� ���°�, ��� �����尡 Ȯ���غ��� �����Ͱ� ����ִ°Ŵ�!

        */

        if (qData.empty() == false)
        {
            int data = qData.front();
            qData.pop();
            cout << data << endl;
        }
        else
        {
            cout << "�̷����� ���µ�?!" << endl;
        }
    }
}