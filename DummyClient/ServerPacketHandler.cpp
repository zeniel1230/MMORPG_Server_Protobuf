#include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& _session, BYTE* _buffer, int32 _len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& _session, Protocol::S_LOGIN& _pkt)
{
	if (_pkt.success() == false)
		return true;

	if (_pkt.players().size() == 0)
	{
		// ĳ���Ͱ� �����Ƿ� ĳ���� �����ϴ� ����
	}

	// ���� UI ��ư ������ ���� ����
	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_playerindex(0); // ù��° ĳ���ͷ� ����
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	_session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& _session, Protocol::S_ENTER_GAME& _pkt)
{
	// TODO

	return true;
}

bool Handle_S_CHAT(PacketSessionRef& _session, Protocol::S_CHAT& _pkt)
{
	std::cout << _pkt.msg() << endl;

	return true;
}

