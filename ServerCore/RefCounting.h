#pragma once

/*----------------------------
		 RefCountable
------------------------------*/

class RefCountable
{
protected:
	atomic<int32> m_refCount;

public:
	RefCountable() : m_refCount(1) {}
	virtual ~RefCountable() {}

	int32 AddRef() { return ++m_refCount; }
	int32 GetRefCount() { return m_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --m_refCount;
		if (refCount == 0)
		{
			delete this;	
		}

		return refCount;
	}
};

/*----------------------------
		   SharedPtr
------------------------------*/

// 1) �̹� ������� Ŭ���� ������� ��� �Ұ�
// - RefCountable�� ��ӹ޾ƾ��ϹǷ� �ܺ� ���̺귯�� ��� �� ��� �Ұ�
// 2) ��ȯ(Cycle) ���� ���� (ǥ�� shared_ptr�� ���� ����)

template <typename T>
class TSharedPTr
{
private:
	T* m_ptr = nullptr;

public:
	TSharedPTr() {}
	TSharedPTr(T* _ptr) { Set(_ptr); }
	// ����
	TSharedPTr(const TSharedPTr& _rhs) { Set(_rhs.m_ptr); }
	// �̵�
	TSharedPTr(TSharedPTr&& _rhs) 
	{ 
		m_ptr = _rhs.m_ptr; 
		_rhs.m_ptr = nullptr; 
	}
	// ��� ���� ����
	template<typename U>
	TSharedPTr(const TSharedPTr<U>& _rhs) 
	{ 
		Set(static_cast<T*>(_rhs.m_ptr)); 
	}

	~TSharedPTr() { Release(); }

public:
	// ���� ������
	TSharedPTr& operator=(const TSharedPTr& _rhs)
	{
		if (m_ptr != _rhs.m_ptr)
		{
			Release();
			Set(_rhs.m_ptr);
		}
		return *this;
	}
	// �̵� ������
	TSharedPTr& operator=(TSharedPTr&& _rhs)
	{
		Release();
		m_ptr = _rhs.m_ptr;
		_rhs.m_ptr = nullptr;
		return *this;
	}
	// ��
	bool		operator==(const TSharedPTr& _rhs)	{ return m_ptr == _rhs.m_ptr; }
	bool		operator==(T* _ptr) const			{ return m_ptr == _ptr; }
	bool		operator!=(const TSharedPTr& _rhs)	{ return m_ptr != _rhs.m_ptr; }
	bool		operator!=(T* _ptr) const			{ return m_ptr != _ptr; }
	// ��Ұ���
	bool		operator<(const TSharedPTr& _rhs)	{ return m_ptr < _rhs.m_ptr; }
	
	T*			operator*()							{ return m_ptr; }
	const T*	operator*() const					{ return m_ptr; }
	// Conversion Operator or Casting Operator
	// �ٸ� Ŭ������ ĳ������ �� �����
				operator T* () const				{ return m_ptr; }
	T*			operator->()						{ return m_ptr; }
	const T*	operator->() const					{ return m_ptr; }

	bool isNull() { return m_ptr == nullptr; }

private:
	inline void Set(T* _ptr)
	{
		m_ptr = _ptr;
		if (m_ptr)
			_ptr->AddRef();
	}

	inline void Release()
	{
		if (m_ptr != nullptr)
		{
			m_ptr->ReleaseRef();
			m_ptr = nullptr;
		}
	}
};

// unique_ptr
// - ���� �����Ϳ� ���̰� ����.
// - �̵��� �����ϰ� �����ϴ� ����� �����ִ�. (����ũ�ؾ��Ѵ�.)

// [T*][RefCountBlock*]
// shared_ptr
// weak_ptr
// - shared_ptr�� ��ȯ ���� �ذ� ����
// - expired()�� �����ϴ��� üũ�ϰ� ����ؾ���
// - Ȥ�� lock()�� �̿��� shared_ptr�� �ٽ� ĳ������ ����Ѵ�.
// - ���ο� RefCountBlock���� useCount�� weakCount�� �ִ�.
//	 useCount�� ���ݱ��� �˴� refCount��. 0�� �Ǹ� ��ȿ���� �����Ƿ� ����
//   weakCount�� weak_ptr ��� üũ�� refCount��.
//   ��� useCount�� weakCount �� �� üũ�ؼ� �ش� ����Ʈ �����͸� �������� Ȯ���Ѵ�.