// Lock.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "SpinLock.h"
#include "SpinLock_With_Sleep.h"

int sum = 0;
mutex m;
SpinLockBase spinlock;
SpinLock_With_Sleep spinlock_with_sleep;

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