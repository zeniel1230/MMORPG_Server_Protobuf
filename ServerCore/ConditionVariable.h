#pragma once
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>

using namespace std;

//mutex m;
//queue<int32> q;
//
//// ����) CV�� User-Level Object (Ŀ�� ������Ʈ�� �ƴ�)
//condition_variable cv;
//
//void Producer()
//{
//	while (true)
//	{
//		// 1) Lock�� ���
//		// 2) ���� ���� ���� ����
//		// 3) Lock�� Ǯ��
//		// 4) ���Ǻ��� ���� �ٸ� �����忡�� ����
//
//		{
//			unique_lock<mutex> lock(m);
//			q.push(100);
//		}
//
//		cv.notify_one(); // wait ���� �����尡 ������ �� 1���� �����.
//
//		//this_thread::sleep_for(10000ms);
//	}
//}
//
//void Consumer()
//{
//	while (true)
//	{
//		unique_lock<mutex> lock(m);
//		cv.wait(lock, []() {return q.empty() == false; } /*����*/);
//		// 1) Lock�� ���
//		// 2) ���� Ȯ��
//		//  - ���� O -> ���� ���ͼ� �̾ �ڵ� ����
//		//	- ���� X -> Lock�� Ǯ���ְ� ��� ���� ��ȯ
//
//		// �׷��� notify_one�� ������ �׻� ���ǽ��� �����ϴ°� �ƴұ�?
//		// Spurious WakeUp (��¥ ���)
//		// notify_one�� �� �� lock�� ��� �ִ� ���� �ƴϱ� ����
//
//		//while (q.empty() == false)
//		{
//			int32 data = q.front();
//			q.pop();
//			cout << q.size() << endl;
//		}
//	}
//}
//
//void main()
//{
//	thread t1(Producer);
//	thread t2(Consumer);
//
//	t1.join();
//	t2.join();
//}