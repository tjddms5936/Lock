#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <Windows.h>
using namespace std;
/*
----------------------------------------------------------------------------------------------------
Event�� Ȱ���Ͽ� ������ �����

�ٽ� ����
    - Ŀ�� ������Ʈ Ȱ��
    - windows.h���� �����ϴ� �ñ׳� ������ Ȱ��
    - �����尡 �ڿ��� �����Ϸ� �ϴµ� �������̸� Ŀ�� ������� �Ķ��� ���� �� �����޶�� ��Ź�ϰ� ���.
    - Ŀ�� ���� �Ķ����� �Ǹ� ��Ź���� �����带 �����ش�.

���� 
    - ������ ��⸦ ���� �ʱ� ������ CPU ��뷮�� ���� �� �ִ�.
    - A������� �����͸� 100�ʿ� �ѹ��� �ְ�, B������� �����͸� ������ �� ������ �� �ִ�. (B�����尡 ������ ��� ���ص� �Ǳ� ����)

����� ����
    - �ᱹ Ŀ�� ����� �ϰ��� �þ��.

�ذ� ���
    - condition variable ... etc
----------------------------------------------------------------------------------------------------
*/

// ������ ������ ����.

// Lock.cpp : �� ���Ͽ��� 'main' �Լ��� ���Ե˴ϴ�. �ű⼭ ���α׷� ������ ���۵ǰ� ����˴ϴ�.
//


mutex m;
queue<int> qData;
HANDLE handle;

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
        
        ::SetEvent(handle);

        this_thread::sleep_for(10000ms);
    }

}

void Consumer()
{
    while (true)
    {
        ::WaitForSingleObject(handle, INFINITE);
        // �Ķ����� �Ǿ� �����, �ڵ����� �����ߴٸ� non-signal ���·� �ٲپ� �ش�.

        unique_lock<mutex> lock(m);
        if (qData.empty() == false)
        {
            int data = qData.front();
            qData.pop();
        }
    }
}
