#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <Windows.h>
using namespace std;
/*
----------------------------------------------------------------------------------------------------
Event를 활용하여 스레드 깨우기

핵심 개념
    - 커널 오브젝트 활용
    - windows.h에서 제공하는 시그날 개념을 활용
    - 스레드가 자원에 접근하려 하는데 빨간불이면 커널 모드한테 파란불 됐을 때 깨워달라고 부탁하고 잠듬.
    - 커널 모드는 파란불이 되면 부탁받은 스레드를 깨워준다.

장점 
    - 무한정 대기를 돌지 않기 때문에 CPU 사용량을 줄일 수 있다.
    - A스레드는 데이터를 100초에 한번씩 넣고, B스레드는 데이터를 꺼내쓸 때 유용할 수 있다. (B스레드가 무한정 대기 안해도 되기 때문)

기능적 결함
    - 결국 커널 모드의 일감이 늘어난다.

해결 방법
    - condition variable ... etc
----------------------------------------------------------------------------------------------------
*/

// 사용법은 다음과 같다.

// Lock.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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
        // 파란불이 되어 깨어나면, 자동으로 설정했다면 non-signal 상태로 바꾸어 준다.

        unique_lock<mutex> lock(m);
        if (qData.empty() == false)
        {
            int data = qData.front();
            qData.pop();
        }
    }
}
