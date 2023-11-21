#pragma once
#include <mutex>

template<typename T>
class LockStack
{
private:
	stack<T> m_stack;
	mutex m_mutex;
	condition_variable m_conVar;

public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T _value)
	{
		lock_guard<mutex> lock(m_mutex);

		m_stack.push(std::move(_value));
		m_conVar.notify_one();
	}

	void WaitPop(T& _value)
	{
		unique_lock<mutex> lock(m_mutex);
		m_conVar.wait(lock, [this] {return m_stack.empty() == false; });

		_value = std::move(m_stack.top());
		m_stack.pop();
	}

	bool TryPop(T& _value)
	{
		lock_guard<mutex> lock(m_mutex);

		if (m_stack.empty()) return false;

		// empty -> top -> pop
		_value = std::move(m_stack.top());
		m_stack.pop();
		return true;
	}

	// ��Ƽ������� �ǹ̰� ����.
	/*bool Empty()
	{
		lock_guard<mutex> lock(m_mutex);
		return m_stack.empty();
	}*/
};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& _value) : data(_value), next(nullptr) {}

		T data;
		Node* next;
	};

private:
	// [ ] [ ] [ ] [ ] [ ]
	// [head]
	atomic<Node*> m_head;

	atomic<int32> m_popCount;		// Pop�� �������� ������ ����
	atomic<Node*> m_pendingList;	// ���� �Ǿ�� �� ���� (ù���� ���)

public:
	LockFreeStack()
	{
		m_popCount = 0;
	}

	LockFreeStack(const LockFreeStack&) = delete;
	LockFreeStack& operator=(const LockFreeStack&) = delete;

	// 1) �� ��带 �����
	// 2) �� ����� next = head
	// 3) head = �� ���
	void Push(const T& _value)
	{
		Node* node = new Node(_value);
		node->next = m_head;

		/*
		if (m_head == node->next)
		{
			m_head = node;
			return true;
		}
		else
		{
			node->next = m_head;
			return false;
		}
		*/
		while (m_head.compare_exchange_weak(node->next, node) == false)
		{		
		}
	}

	// 1) head �б�
	// 2) head->next �б�
	// 3) head = head->next
	// 4) data �����ؼ� ��ȯ
	// 5) ������ ��� ����
	bool TryPop(T& _value)
	{
		++m_popCount;

		Node* oldHead = m_head;

		/*
		if (m_head == oldHead)
		{
			m_head = oldHead->next;
			return true;
		}
		else
		{
			oldHead = m_head;
			return false;
		}
		*/
		while (oldHead && m_head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{			
		}

		if (oldHead == nullptr)
		{
			--m_popCount;
			return false;
		}

		_value = oldHead->data;
		// C#�̳� Java���� GC�� ������ ��� ���⼭ ���̴�.
		TryDelete(oldHead);

		return true;
	}

	// 1) ������ �и�
	// 2) Count üũ
	// 3) �� ȥ�ڸ� ����
	void TryDelete(Node* _oldHead)
	{
		// �� �ܿ� ���� �ִ°�?
		if (m_popCount == 1)
		{
			// �� ȥ�ڱ���

			// �̿� ȥ���ΰ�, ���� ����� �ٸ� �����͵鵵 �����غ���
			Node* node = m_pendingList.exchange(nullptr);

			if (--m_popCount == 0)
			{
				// �߰��� ����� �ְ� ���� -> ���� ����
				// �����ͼ� �������� ����� ������ �����ʹ� �и��ص� �����̴�.			
				DeleteNodes(node);
			}
			else if(node)
			{
				// ���� ���������� �ٽ� ���� ����
				ChainPendingNodeList(node);
			}

			// �� �����ʹ� ����
			delete _oldHead;
		}
		else
		{
			// ���� ������ ���� �������� �ʰ� ���� ���ุ
			ChainPendingNode(_oldHead);
			--m_popCount;
		}
	}

	void ChainPendingNodeList(Node* _first, Node* _last)
	{
		_last->next = m_pendingList;

		while (m_pendingList.compare_exchange_weak(_last->next, _first) == false)
		{

		}
	}

	void ChainPendingNodeList(Node* _node)
	{
		Node* last = _node;

		while (last->next) last = last->next;

		ChainPendingNodeList(_node, last);
	}

	void ChainPendingNode(Node* _node)
	{
		ChainPendingNodeList(_node, _node);
	}

	static void DeleteNodes(Node* _node)
	{
		while (_node)
		{
			Node* next = _node->next;
			delete _node;
			_node = next;
		}
	}
};

// ������ ���α׷��� �����Ϸ� ��������� ��¥�� �������� �ƴϴ�.
template<typename T>
class LockFreeStack2
{
	struct Node
	{
		Node(const T& _value) : data(make_shared<T>(_value)), next(nullptr) {}

		shared_ptr<T> data;
		shared_ptr<Node> next;
	};

private:
	shared_ptr<Node> m_head;

public:
	LockFreeStack2() {}

	LockFreeStack2(const LockFreeStack2&) = delete;
	LockFreeStack2& operator=(const LockFreeStack2&) = delete;

	void Push(const T& _value)
	{
		shared_ptr<Node> node = make_shared<Node>(_value);
		node->next = std::atomic_load(&m_head);

		while (std::atomic_compare_exchange_weak(&m_head, &node->next, node) == false)
		{
		}
	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&m_head);

		while (oldHead && std::atomic_compare_exchange_weak(&m_head, &oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr) return shared_ptr<T>();

		return oldHead->data;
	}
};

template<typename T>
class LockFreeStack3
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& _value) : data(make_shared<T>(_value)) {}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};

private:
	atomic<CountedNodePtr> m_head;

public:
	LockFreeStack3() {}

	LockFreeStack3(const LockFreeStack3&) = delete;
	LockFreeStack3& operator=(const LockFreeStack3&) = delete;

	void Push(const T& _value)
	{
		CountedNodePtr node;
		node.ptr = new Node(_value);
		node.externalCount = 1;
		// [!]
		node.ptr->next = m_head;
		while (m_head.compare_exchange_weak(node.prt->next, node) == false)
		{
		}
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = m_head;
		while (true)
		{
			// ������ Ⱥ�� (externalCount�� �� ���� ���� +1 �� �ְ� �̱�)
			IncreaseHeadCount(oldHead);
			// �ּ��� externalCount >= 2 ���´� ����X (�����ϰ� ������ �� �ִ�)
			Node* ptr = oldHead.ptr;

			// ������ ����
			if (ptr == nullptr)
				return shared_ptr<T>();

			// externalCount ���۰� : 1 -> 2(+1)
			// internalCount ���۰� : 0

			// ������ ȹ�� (ptr->next�� head�� �ٲ�ġ�� �� �ְ� �̱�)		
			if (m_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				// �� ���� �� ���� �ִ°�?
				const int32 countIncrease = oldHead.externalCount - 2;

				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if(ptr->internalCount.fetch_sub(1) == 1)
			{
				// �������� �������, �������� ���� -> �޼����� ���� �Ѵ�.
				delete ptr;
			}
		}
	}

	void IncreaseHeadCount(CountedNodePtr& _oldCounter)
	{
		while(true)
		{
			CountedNodePtr newCounter = _oldCounter;
			newCounter.externalCount++;

			if (m_head.compare_exchange_strong(_oldCounter, newCounter))
			{
				_oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}
};

// MemoryPool ���ο� LockFreeStack
// =====================================================================

/*----------------------------
		   1�� �õ�
------------------------------*/

/*struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* _header);
void PushEntrySList(SListHeader* _header, SListEntry* _entry);
SListEntry* PopEntrySList(SListHeader* _header);*/

/*----------------------------
		   2�� �õ�
------------------------------*/

// ��Ƽ������ ȯ�濡�� ���ư�����.
/*struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* _header);
void PushEntrySList(SListHeader* _header, SListEntry* _entry);
SListEntry* PopEntrySList(SListHeader* _header);*/

/*----------------------------
		   3�� �õ�
------------------------------*/

// MS���� ������� �����ϰ� ����
// MS���� ������� ����� ���̴�.(�ش� �����ؼ� ������ �κ��� ������� ���� ��)
// LockFree�� ���� �����ϱ⿣ �ʹ� �����ϴ�.

// ������ �޸𸮰� 16����Ʈ ������ �ǰԲ� �����Ϸ����� ��û
// 16����Ʈ ���ķ� �ϰԵǸ� �ڴ� ������ ���� 4��Ʈ�� 0000�� �ȴ�.

/*DECLSPEC_ALIGN(16)
struct SListEntry
{
	SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}

	// ABA ���� �ذ��� ����
	// �ΰ��� �ð����� ������ �����͸� ����
	union
	{
		struct
		{
			// 128 ��Ʈ
			uint64 alignment;
			uint64 region;
		} DUMMYSTRUCTNAME;
		struct
		{
			// ������ ī���� ����
			// uint64 alignment;
			uint64 depth		: 16;
			uint64 sequence		: 48; 
			// uint64 region;
			uint64 reserved		: 4;
			uint64 next			: 60;
		} HeaderX64;
	};
};

void InitializeHead(SListHeader* _header);
void PushEntrySList(SListHeader* _header, SListEntry* _entry);
SListEntry* PopEntrySList(SListHeader* _header);*/