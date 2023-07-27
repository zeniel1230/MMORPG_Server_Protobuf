#pragma once

/*----------------------------
		 BaseAllocator
------------------------------*/

class BaseAllocator
{
public:
	static void* Alloc(int32 _size);
	static void Release(void* _ptr);
};

/*----------------------------
		 StompAllocator
------------------------------*/

// �𸮾� ���������� ���õǾ� ����
// ȿ�������� ������ �Ѵٱ⺸�� ���׸� ��µ� �����ϴ�.
// (�޸� ���� ��)

class StompAllocator
{
public:
			// 4kb
	enum { PAGE_SIZE = 0x1000 };

	static void*	Alloc(int32 _size);
	static void		Release(void* _ptr);
};

/*----------------------------
		 PoolAllocator
------------------------------*/

class PoolAllocator
{
public:
	static void*	Alloc(int32 _size);
	static void		Release(void* _ptr);
};

/*----------------------------
		 STLAllocator
------------------------------*/

template <typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}
	template <typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	T* allocate(size_t _count)
	{
		const int32 size = static_cast<int32>(_count * sizeof(T));
		//return static_cast<T*>(Xalloc(size));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* _ptr, size_t _count)
	{
		//Xrelease(_ptr);
		PoolAllocator::Release(_ptr);
	}
};