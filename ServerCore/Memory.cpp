#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		m_pools.emplace_back(pool);

		while (tableIndex <= size)
		{
			m_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		m_pools.emplace_back(pool);

		while (tableIndex <= size)
		{
			m_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		m_pools.emplace_back(pool);

		while (tableIndex <= size)
		{
			m_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* pool : m_pools)
		delete pool;

	m_pools.clear();
}

void* Memory::Allocate(int32 _size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = _size + sizeof(MemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(
		StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� �Ҵ�
		//header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
		header = reinterpret_cast<MemoryHeader*>(
			::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// �޸� Ǯ���� �����´�.
		header = m_poolTable[allocSize]->Pop();
	}
#endif

	return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* _ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(_ptr);

	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� ����
		//::free(header);
		// ���� �ʿ������ ��Ģ�� ���
		::_aligned_free(header);
	}
	else
	{
		// �޸� Ǯ�� �ݳ�
		m_poolTable[allocSize]->Push(header);
	}
#endif
}