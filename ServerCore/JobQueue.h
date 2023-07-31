#pragma once
#include "Job.h"
#include "LockQueue.h"

/*----------------------------
			JobQueue
------------------------------*/

class JobQueue : public enable_shared_from_this<JobQueue>
{
protected:
	LockQueue<JobRef>	m_jobs;
	// �ϰ� ������
	Atomic<int32>		m_jobCount;

public:
	void DoAsync(CallbackType&& _callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(_callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* _memFunc)(Args...), Args... _args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, _memFunc, std::forward<Args>(_args)...));
	}


	void ClearJobs() { m_jobs.Clear(); }

public:
	// ���� �������� ������ �����. ���۷��� ī��Ʈ ���� ������ ���� ����
	//void Push(JobRef&& _job);
	// _pushOnly �߰� ���� : �����ϰ� ����ϰ� �ϱ� �����̴�.
	// CPU �ھ�� ���� �յ��ϰ� �й��ؾ��ϴµ� JobQueue Ư���� Job���� �� Job�� �����ϴٺ���
	// �������� �ϰ��� �����Ⱑ ����. �׷��� �ϰ��� �о�ֱ⸸ �ϰ�
	// ���߿� ���� ������ ������ �����ϸ� ���� �й谡 ������ �� ����� ����ϱ�� �ߴ�.
	void Push(JobRef _job, bool _pushOnly = false);
	void Execute();
};