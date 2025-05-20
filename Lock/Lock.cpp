// Lock.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <queue>
#include <Windows.h>
#include "SpinLock.h"
#include "SpinLock_With_Sleep.h"
#include "LockBasedQueue.h"
#include "LockBasedStack.h"

int sum = 0;
mutex m;
queue<int> qData;
HANDLE handle;

LockQueue<int> q;
LockStack<int> s;

SpinLockBase spinlock;
SpinLock_With_Sleep spinlock_with_sleep;
void Add();
void Sub();
void Push();
void Pop();

int main()
{
    thread t1(Push);
    thread t2(Pop);
    thread t3(Pop);

    t1.join();
    t2.join();
    t3.join();

}


void Add()
{
    for (int i = 0; i < 10'0000; ++i)
    {
        lock_guard<SpinLock_With_Sleep> guard(spinlock_with_sleep);
        ++sum;
    }
}

void Sub()
{
    for (int i = 0; i < 10'0000; ++i)
    {
        lock_guard<SpinLock_With_Sleep> guard(spinlock_with_sleep);
        --sum;
    }
}

void Push()
{
    while (true)
    {
        int val = rand() % 100;
        q.Push(val);
        this_thread::sleep_for(10ms);
    }

}

void Pop()
{
    while (true)
    {
        /*int data = 0;
        if (q.TryPop(data))
            cout << data << endl;*/
        int data = 0;
        q.WaitPop(data);
        cout << data << endl;
    }
}
