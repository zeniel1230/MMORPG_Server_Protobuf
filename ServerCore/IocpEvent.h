#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	// https://ozt88.tistory.com/26 - PAGE_LOCKING & ZeroByteRecv�� PAGE_LOCKING �ּ�ȭ
	//PreRecv,		// ��쿡 ���� recv �ϱ� ������ ���� �ϴ� ��찡 �ִ�. (zero-byte receive�� �ϴ� ��ޱ��)
	Recv,
	Send
};

/*----------------------------
		  3. IocpEvent
------------------------------*/

// OVERLAPPED�� ������� �ᵵ�ǰ� ù��° �ɹ������� �ᵵ�ȴ�.
// ��ӱ����� �ϸ� ù��° offset���� OVERLAPPED �޸𸮰� ��������´� �����ͷ� ����ϱ� ���� ��
// ����
// virtual �Լ� ��� ���� - virtual �Լ������� offset 0�� �޸𸮿� ���� �Լ� ���̺��� �� OVERLAPPED�� ���õ� �޸𸮰� �ƴ϶� �ٸ��ɷ� ä���� �� �ִ�.
// (OVERLAPPED �޸𸮰� offset 0�� �޸𸮰� �Ǿ���Ѵ�.)
class IocpEvent : public OVERLAPPED
{
public:
	EventType		m_eventType;
	IocpObjectRef	m_owner;

public:
	IocpEvent(EventType _type);

	void			Init();
	EventType		GetType() { return m_eventType; }
};

/*----------------------------
		  ConnectEvent
------------------------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*----------------------------
		DisconnectEvent
------------------------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};

/*----------------------------
		  AcceptEvent
------------------------------*/

class AcceptEvent : public IocpEvent
{
public:
	// acceptex�� ���� �߿� Ŭ���� ������ �޾��ִ� �κ��� �־� �߰�
	SessionRef m_session = nullptr;

public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }
};

/*----------------------------
		  RecvEvent
------------------------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------------------
		  SendEvent
------------------------------*/

class SendEvent : public IocpEvent
{
public:
	Vector<SendBufferRef> m_sendBuffers;

public:
	SendEvent() : IocpEvent(EventType::Send) { }
};