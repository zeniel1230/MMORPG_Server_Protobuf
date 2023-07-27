#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*----------------------------
		 BaseAllocator
------------------------------*/

void* BaseAllocator::Alloc(int32 _size)
{
	return ::malloc(_size);
}

void BaseAllocator::Release(void* _ptr)
{
	::free(_ptr);
}

void* StompAllocator::Alloc(int32 _size)
{
	const int64 pageCount = (_size + PAGE_SIZE - 1) / PAGE_SIZE;
	// �����÷ο츦 ������� �������� ���ʿ� �ڷḦ ���ܼ� �־�д�.
	const int64 dataOffset = pageCount * PAGE_SIZE - _size;
	
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* _ptr)
{
	const int64 address = reinterpret_cast<int64>(_ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

void* PoolAllocator::Alloc(int32 _size)
{
	return GMemory->Allocate(_size);
}

void PoolAllocator::Release(void* _ptr)
{
	GMemory->Release(_ptr);
}