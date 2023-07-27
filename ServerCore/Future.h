#pragma once
/*
#include <thread>
#include <future>

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100000; i++)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

void main()
{
	// ����(Synchronous) ����
	//int64 sum = Calculate();
	//cout << sum << endl;

	// std::future
	// ���� ���� ���۽� ������ ��Ʈ �ε����� ����ߴٰ� �Ѵ�.
	// ��Ƽ������� �ص� ������ ������ ������ �ε����� ���Ƿ� �� ����� �����غ��δ�.
	{
		// 1) deferred : lazy evaluation �����ؼ� �����ϼ���.
		// 2) async : ������ �����带 ���� �����ϼ���.
		// 3) deferred | async : �� �� �˾Ƽ� ����ּ���.

		// ������ �̷��� ������� ����ٰž�!
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// TODO
		std::future_status status = future.wait_for(1ms);
		if (status == future_status::ready)
		{
			// �ϰ��� �Ϸ�Ǿ����� ó��
		}

		int64 sum = future.get(); // = future.wait(); ������� �������� �ʿ��ϴ�!

		// �ɹ� �Լ� �̿��
		{
			class Knight
			{
			public:
				int64 GetHP() { return 100; }
			};

			Knight knight;
			std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHP, knight); //knight.GetHp();
		}
	}

	//std::promise
	{
		// �̷���(std::future)�� ������� ��ȭ���ٲ��� ���(std::promise) ����
		std::promise<string> promise;
		std::future<string> future = promise.get_future();

		thread t(PromiseWorker, std::move(promise));

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	// std::packaged_task
	// async future�� �������� �����带 ����� ������� �޾Ƴ��� �Ͱ� �ٸ��� 
	// �̹� �����ϴ� �����忡 �ϰ��̶� ������ ����� ���Ѱ��ְ� ������� future�� �޴´�.
	{
		std::packaged_task<int64(void)> task(Calculate);
		std::future<int64> future = task.get_future();

		thread t(TaskWorker, std::move(task));

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}

	// ���)
	// mutex, condition_variable���� ���� �ʰ� �ܼ��� �ֵ��� ó���� �� �ִ�
	// Ư����, �� �� �߻��ϴ� �̺�Ʈ�� �����ϴ�!
	// ����µ� ����� Į�� �� �ʿ� ����!
	// 1) async
	// ���ϴ� �Լ� �ϳ��� �񵿱������� ����
	// 2) promise (���� ���X)
	// ������� promise�� ���� future�� �޾���
	// 3) packaged_task (���� ���X)
	// ���ϴ� �Լ��� ���� ����� packaged_task�� ���� future�� �޾���
}
*/