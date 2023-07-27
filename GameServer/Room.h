#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
private:
	map<uint64, PlayerRef> m_players;

	// ���� ���� �Ұ����ϵ��� private
public:
	// �̱۾����� ȯ������ ����
	void Enter(PlayerRef _player);
	void Leave(PlayerRef _player);
	void Broadcast(SendBufferRef _sendBuffer);

public:
	// ��Ƽ������ ȯ�濡���� job���� ���ٽ��Ѿ���
	virtual void FlushJob() override;
};

extern shared_ptr<Room> GRoom;