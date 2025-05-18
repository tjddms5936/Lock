#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <Windows.h>

using namespace std;
/*
----------------------------------------------------------------------------------------------------
Condition Variable 활용

핵심 개념
    - 유저 레벨 오브젝트 활용
    - Event와 비슷한 개념을 활용해서 사용
    - lock이랑 짝을 지어서 사용

사용 방법
    - 1. Lock을 잡고
    - 2. 공유 변수 값을 수정
    - 3. Lock을 풀고
    - 4. 조건변수 통해 다른 쓰레드에게 통지

장점
    - 커널 오브젝트까지 활용하지 않아도 된다.



어지간해서는 이걸로 사용하자!!!!!!!!!!!!

----------------------------------------------------------------------------------------------------
*/

// 사용법은 다음과 같다.


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

        cv.notify_one(); // wait중인 쓰레드가 있으면 딱 1개를 깨운다.
        this_thread::sleep_for(10000ms);
    }

}

void Consumer()
{
    while (true)
    {
        unique_lock<mutex> lock(m);

        cv.wait(lock, []() { return qData.empty() == false;  }); // 두번째 매개변수에 람다 넣어도 됨. 조건이라고 생각하면 된다.
        // 다음과 같은 과정을 겪는다. 
        /*
            1. Lock을 잡고
            2. 조건을 확인한다 (매개변수로 넣은 람다 활용)
                -> 만족O : 빠져 나와서 이어서 코드 실행
                -> 만족X : Lock을 풀어주고 대기 상태로 진입
                    --> 이 기능때문에 lock_guard가 아닌 unique_lock을 활용해야 한다.
                    --> 그리고서 누군가 notify_one을 때려주면 다시 1번부터 시작한다.

            ※의문점 : 누군가 notify_one 했다는건... 항상 조건식 만족하는거 아니야?! 데이터를 넣은 다음에 notify_one하는거잖아.
                : Spurious Wakeup (가짜 기상)
                : notify_one할때 lock을 잡고 있는 상태가 아니다.그렇기 때문에 C스레드가 데이터를 전부 꺼냈다면? B스레드의 조건 검사를 통과하지 못할 수 있다.
                : 즉, 대기중인 스레드를 깨우기 전에! lock을 푼 상태이기 때문에 다른곳에서 데이터를 다 꺼냈고, 깨어난 스레드가 확인해보니 데이터가 비어있는거다!

        */

        if (qData.empty() == false)
        {
            int data = qData.front();
            qData.pop();
            cout << data << endl;
        }
        else
        {
            cout << "이럴수가 없는데?!" << endl;
        }
    }
}