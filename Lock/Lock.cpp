// Lock.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "SpinLock.h"

int sum = 0;
mutex m;
SpinLockBase spinlock;

void Add();
void Sub();

int main()
{
    thread t1(Add);
    thread t2(Sub);

    t1.join();
    t2.join();

    cout << sum << endl;
}


void Add()
{
    for (int i = 0; i < 10'0000; ++i)
    {
        lock_guard<SpinLockBase> guard(spinlock);
        ++sum;
    }
}

void Sub()
{
    for (int i = 0; i < 10'0000; ++i)
    {
        lock_guard<SpinLockBase> guard(spinlock);
        --sum;
    }
}