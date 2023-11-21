#pragma once
#include "Allocator.h"

class MemoryPool;

/*----------------------------
			Memory
------------------------------*/

// MemoryPool�� �Ѱ��ϴ� �Ŵ���
class Memory
{
	enum
	{
		// 0 ~ 1024����Ʈ������ 32����Ʈ ����
		// ~ 2048����Ʈ������ 128 ����Ʈ ����
		// ~ 4096����Ʈ������ 256 ����Ʈ ������
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

private:
	vector<MemoryPool*> m_pools;

	// �޸� ũ�� <-> �޸� Ǯ
	// O(1) ������ ã�� ���� ���� ���̺�
	MemoryPool* m_poolTable[MAX_ALLOC_SIZE + 1];

public:
	Memory();
	~Memory();

	void*	Allocate(int32 _size);
	void	Release(void* _ptr);
};

// ���ϴ� ������� �Ҵ��ϴ� ���� ��ǥ
// - �޸� �Ҵ� ���(Ǯ�� ��) �� �޸� ���� ���� ����
#pragma region custom new, delete

//						�����ڿ� �� ���ڰ�(��) - Variadic template
template <typename Type, typename... Args>
Type* xnew(Args&&... _args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new ����
	// �޸� �����ٰ� ������ ȣ��
	new(memory)Type(std::forward<Args>(_args)...);

	return memory;
}

template <typename Type>
void xdelete(Type* _obj)
{
	// �Ҹ��� ȣ��
	_obj->~Type();
	PoolAllocator::Release(_obj);
}

#pragma endregion custom new, delete

template <typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... _args)
{
	return shared_ptr<Type> {xnew<Type>(std::forward<Args>(_args)...), xdelete<Type>};
}