#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
private:
	map<uint64, PlayerRef> m_players;

	// ���� ���� �Ұ����ϵ��� private
public:
	// �̱۾����� ȯ������ ����
	void Enter(PlayerRef _player);
	void Leave(PlayerRef _player);
	void Broadcast(SendBufferRef _sendBuffer);
};