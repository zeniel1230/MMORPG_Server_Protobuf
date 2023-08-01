#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

/*----------------------------
			JobTimer
------------------------------*/

void JobTimer::Reserve(uint64 _tick, weak_ptr<JobQueue> _owner, JobRef _job)
{
	const uint64 executeTick = ::GetTickCount64() + _tick;
	// STOMP �̻���(Release)�� �̹� ���������� Lock�� �ɷ��ִ�.
	JobData* jobData = ObjectPool<JobData>::Pop(_owner, _job);

	WRITE_LOCK;

	m_items.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(uint64 _now)
{
	// �� ���� 1 �����常 ���
	// -> ��ڰ� �����尡 ������ ������ ��ó���� ���ÿ� �ϰԵǸ�
	// �ϰ��� ������ ���� ���� �ִ�.(������ ���� ���� ����)
	if (m_distributing.exchange(true) == true)
		// ���� ���� true�� ������ �̹� �� �Լ��� ���� ���̴� return
		return;

	// �ּ����� ��
	// ����� �ֵ鸸 �̾Ƶα� ���� ���� ����
	Vector<TimerItem> items;
	{
		WRITE_LOCK;

		while (m_items.empty() == false)
		{
			const TimerItem& timerItem = m_items.top();
			if (_now < timerItem.m_executeTick)
				// ���� ���� ����
				break;

			// ������ ���� ��
			items.push_back(timerItem);
			m_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.m_jobData->m_owner.lock())
			owner->Push(item.m_jobData->m_job);

		ObjectPool<JobData>::Push(item.m_jobData);
	}

	// �������� Ǯ���ش�
	m_distributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;

	while (m_items.empty() == false)
	{
		const TimerItem& timerItem = m_items.top();
		ObjectPool<JobData>::Push(timerItem.m_jobData);
		m_items.pop();
	}
}
