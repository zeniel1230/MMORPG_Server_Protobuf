#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;

/*----------------------------
		   Listener
------------------------------*/

class Listener : public IocpObject
{
protected:
	SOCKET					m_socket = INVALID_SOCKET;
	Vector<AcceptEvent*>	m_acceptEvents;
	ServerServiceRef		m_service;

public:
	Listener() = default;
	~Listener();

public:
	/* �ܺο��� ��� */
	bool StartAccept(ServerServiceRef _service);
	void CloseSocket();

public:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* _iocpEvent, int32 _numOfBytes = 0) override;

private:
	/* ���� ���� */
	void RegisterAccept(AcceptEvent* _acceptEvent);
	void ProcessAccept(AcceptEvent* _acceptEvent);
};