#pragma once

/*----------------------------
		  2. IocpObject
------------------------------*/

// https://chipmaker.tistory.com/entry/enablesharedfromthis-%EC%A0%95%EB%A6%AC
// enable_shared_from_this
// ���������� �ڱ� �ڽ��� weakPtr�� �����ִ�.
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) abstract;
};

/*----------------------------
		  1. IocpCore
------------------------------*/

class IocpCore
{
private:
	HANDLE		m_iocpHandle;

public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return m_iocpHandle; }

	// IOCP�� ���� ������� ���
	bool		Register(IocpObjectRef _iocpObject);
	// WorkerThread���� IOCP�� �ϰŸ��� ã�� �Լ�
	bool		Dispatch(uint32 _timeoutMs = INFINITE);
};