#include "pch.h"
#include "ConcurrentStack.h"

/*----------------------------
		   1�� �õ�
------------------------------*/

/*void InitializeHead(SListHeader* _header)
{
	_header->next = nullptr;
}

void PushEntrySList(SListHeader* _header, SListEntry* _entry)
{
	_entry->next = _header->next;
	_header->next = _entry;
}

SListEntry* PopEntrySList(SListHeader* _header)
{
	SListEntry* first = _header->next;

	if (first != nullptr)
		_header->next = first->next;

	return first;
}*/

/*----------------------------
		   2�� �õ�
------------------------------*/

/*void InitializeHead(SListHeader* _header)
{
	_header->next = nullptr;
}

void PushEntrySList(SListHeader* _header, SListEntry* _entry)
{
	_entry->next = _header->next;
															         // desired, expected
	while (::InterlockedCompareExchange64((int64*)&_header->next, (int64)_entry, (int64)_entry->next) == 0)
	{

	}
}

SListEntry* PopEntrySList(SListHeader* _header)
{
	SListEntry* expected = _header->next;

	// ABA ����
	while (expected && ::InterlockedCompareExchange64(
		(int64*)&_header->next, (int64)expected->next, (int64)expected) == 0)
	{

	}

	return expected;
}*/

/*----------------------------
		   3�� �õ�
------------------------------*/

/*void InitializeHead(SListHeader* _header)
{
	_header->alignment = 0;
	_header->region = 0;
}

void PushEntrySList(SListHeader* _header, SListEntry* _entry)
{
	SListHeader expected = {};
	SListHeader desired = {};

	// 16����Ʈ ����
	// 60��Ʈ(next ũ��)�� �ּҸ� ǥ��(���� 60���� �� ������ �����̱� ����)
	desired.HeaderX64.next = (((uint64)_entry) >> 4);

	while (true)
	{
		expected = *_header;

		// �� ���̿� ����� �� �ִ�.

		// 4��Ʈ�� ���� �����ߴ� ���� ����
		_entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		// ī����
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

			//									���
		if (::InterlockedCompareExchange128((int64*)_header, 
			// exchange �� 2��					������ ��
			desired.region, desired.alignment, (int64*)&expected) == 1)
			// ����
			break;
	}
}

SListEntry* PopEntrySList(SListHeader* _header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *_header;
		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		if (entry == nullptr) break;

		// �̰����� Use_After_Free ���� ���� �� �ִ�. 
		// (�ٸ� �����尡 pop�� �Ͽ� ���� �ּҸ� �������� ��� ũ���� �Ǵ� �޸� ���������� �߻��Ѵ�.)
		// 64��Ʈ �ּҸ� 60��Ʈ�� ���ֱܳ� ���� 4�� �����.
		desired.HeaderX64.next = ((uint64)entry->next) >> 4;

		// �ش� ���� ���п� ABA ������ �ذ�
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)_header,
			desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}*/

// GameServer.cpp���� ������(���� �� ���� ����Ͱ� ���� ���̰� ������ ������)
/*
DECLSPEC_ALIGN(16)
class Data
{
public:
	// ��ӹްų� �ɹ� ������ Ȱ���ϸ�ȴ�.
	SLIST_ENTRY m_entry; //SListEntry m_entry;

	int64 m_rand = rand() % 1000;
};

SLIST_HEADER* GHeader; //SListHeader* GHeader;

int main()
{
	GHeader = new SLIST_HEADER(); //GHeader = new SListHeader();
	// 16����Ʈ �����ΰ� üũ
	ASSERT_CRASH((uint64)GHeader % 16 == 0);
	::InitializeSListHead(GHeader); //InitializeHead(GHeader);

	for (int32 i = 0; i < 3; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Data* data = new Data();
					ASSERT_CRASH((uint64)data % 16 == 0);

					::InterlockedPushEntrySList(GHeader, (PSLIST_ENTRY)data);
					//PushEntrySList(GHeader, (SListEntry*)data);
					this_thread::sleep_for(10ms);
				}
			});
	}

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Data* popData = nullptr;
					popData = (Data*)::InterlockedPopEntrySList(GHeader);
					//popData = (Data*)PopEntrySList(GHeader);

					if (popData)
					{
						cout << popData->m_rand << endl;
					}
					else
					{
						cout << "NONE" << endl;
					}

					this_thread::sleep_for(5ms);
				}
			});
	}

	GThreadManager->Join();
}
*/