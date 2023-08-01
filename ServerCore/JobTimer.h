#pragma once

struct JobData
{
	JobData(weak_ptr<JobQueue> _owner, JobRef _job) : m_owner(_owner), m_job(_job)
	{

	}

	// shared_ptr�� �α⿣ �����ֱ⶧���� �Ҹ���� ���� �� �־� weak_ptr�� ����.
	weak_ptr<JobQueue>	m_owner;
	JobRef				m_job;
};

struct TimerItem
{
	bool operator<(const TimerItem& _other) const
	{
		return m_executeTick > _other.m_executeTick;
	}

	// ����� ƽ
	uint64 m_executeTick = 0;
	// JobTimer ���ο����� ����ϰ� �����ҰŶ� �������Ϳ��� ������ �� ����.
	JobData* m_jobData = nullptr;
};

/*----------------------------
			JobTimer
------------------------------*/

// TODO : �������� �����
class JobTimer
{
private:
	USE_LOCK;

	PriorityQueue<TimerItem>	m_items;
	// �ѹ��� �Ѱ��� �ϰ� ����ϵ��� compare�� Atomic<bool>
	Atomic<bool>				m_distributing = false;

public:
	void			Reserve(uint64 _tick, weak_ptr<JobQueue> _owner, JobRef _job);
	void			Distribute(uint64 _now);
	void			Clear();
};

