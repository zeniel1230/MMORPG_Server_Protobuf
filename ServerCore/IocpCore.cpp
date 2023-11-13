#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

/*----------------------------
		  IocpCore
------------------------------*/

IocpCore::IocpCore()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(m_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef _iocpObject)
{
	// return ::CreateIoCompletionPort(_iocpObject->GetHandle(), m_iocpHandle, reinterpret_cast<ULONG_PTR>(_iocpObject), 0);

	// CICS���� Key������ ����� iocpObject��, GQCS���� �����ؼ� �۾��ϰ� �־����� �ش� �۾��� �ϰ� ������ iocpObject�� ����ִٴ� ������ ����.
	// ���� �׾��ִٸ� Crash�� �� ���̴�. �׷��Ƿ� �� �� ��������� ����������Ѵ�.
	// �ذ�å
	// 1. RefCounting -> ���������� �ܺο��� Session�� shared_ptr������ �������� RefCounting�� �����ϸ� ����� �����. (�ܺο��� Ref�� 0�� �Ǿ �����Ϸ��ϸ� ���οʹ� ������� �����Ǿ� ������ ��)
	//					 ���� ��� ������ �ڱ� �ڽ��� shared_ptr�� ���� ����ϸ� �Ǳ������� ������� �ʴ�.
	// 2. key���� ������� �ʰ� ��� �͵��� IocpEvent�� �����ش�.(IocpEvent���� �ڱ� �ڽ��� ���������� �ɾ��� IocpObject�� ����Ѵ�.) <- �ش� ������� ����

	// ����, session �ּ� �Ӹ� �ƴ϶� queue�� ����ϵ� ���ϴ� �ϰ��� ���� ���� �ִ�. -> IocpObject�� ����
	return ::CreateIoCompletionPort(_iocpObject->GetHandle(), m_iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 _timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	/*IocpObject* iocpObject = nullptr;

	if (::GetQueuedCompletionStatus(m_iocpHandle, OUT & numOfBytes,
		OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), _timeoutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}*/
	if (::GetQueuedCompletionStatus(m_iocpHandle, OUT &numOfBytes, OUT &key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), _timeoutMs))
	{
		// ����
		IocpObjectRef iocpObject = iocpEvent->m_owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		// ����
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : �α� ���
			IocpObjectRef iocpObject = iocpEvent->m_owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
