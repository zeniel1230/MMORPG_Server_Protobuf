#include "pch.h"
#include "SendBuffer.h"

/*----------------------------
		  SendBuffer
------------------------------*/

SendBuffer::SendBuffer(SendBufferChunkRef _owner, BYTE* _buffer, uint32 _allocSize)
	: m_owner(_owner), m_buffer(_buffer), m_allocSize(_allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 _writeSize)
{
	ASSERT_CRASH(m_allocSize >= _writeSize);
	m_writeSize = _writeSize;
	m_owner->Close(_writeSize);
}

/*----------------------------
		SendBufferChunk
------------------------------*/

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	m_open = false;
	m_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 _allocSize)
{
	ASSERT_CRASH(_allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(m_open == false);

	if (_allocSize > FreeSize())
		return nullptr;

	m_open = true;
	//												�� �ڽ�			����			
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), _allocSize);
}

void SendBufferChunk::Close(uint32 _writeSize)
{
	ASSERT_CRASH(m_open == true);
	m_open = false;
	m_usedSize += _writeSize;
}

/*----------------------------
		SendBufferManager
------------------------------*/

SendBufferRef SendBufferManager::Open(uint32 _size)
{
	// TLS �̹Ƿ� ����� �������ϸ� �������� �ʴ´�.
	// ��ü�� �����ϴ� Pop�� �� ���
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}		

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// ���������� üũ�� �� ������ ������ ���ŷ� ��ü
	// (������ LSendBufferChunk�� RefCount�� 0�� �Ǿ� PushGlobal�� ȣ���Ѵ�.)
	if (LSendBufferChunk->FreeSize() < _size)
	{
		LSendBufferChunk = Pop(); // WRITE_LOCK
		LSendBufferChunk->Reset();
	}

	//cout << "FREE : " << LSendBufferChunk->FreeSize() << endl;

	return LSendBufferChunk->Open(_size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	//cout << "Pop SENDBUFFERCHUNK" << endl;

	{
		WRITE_LOCK;
		if (m_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = m_sendBufferChunks.back();
			m_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}

	// �������� �����Ƿ� ���� ����
	// ������ ������ ���̹Ƿ� deleter �ڸ��� delete�� �޸𸮸� �����°� �ƴ� PushGlobal�� �ִ´�.
	// PushGlobal�� �ɹ��Լ��� ������ �� �����Ƿ� static �Լ��� ����
	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef _buffer)
{
	WRITE_LOCK;
	m_sendBufferChunks.push_back(_buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* _buffer)
{
#ifdef _DEBUG
	cout << "PushGlobal SENDBUFFERCHUNK" << endl;
#endif

	GSendBufferManager->Push(SendBufferChunkRef(_buffer, PushGlobal));
}