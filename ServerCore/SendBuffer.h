#pragma once

class SendBufferChunk;

/*----------------------------
		  SendBuffer
------------------------------*/

// ���������� SendBufferChunk�� �߶� ����ϴ� ���� �� SendBuffer��.(SendBufferChunk���� �߶�� ���� SendBuffer�� ����)
class SendBuffer
{
private:
	BYTE*				m_buffer;
	// �Ҵ�� ������
	uint32				m_allocSize = 0;
	// ������ ����ϴ� ������ ������
	uint32				m_writeSize = 0;
	// ������ SendBufferChunk�� �Ϻ� ������ ��� ���̸� SendBufferChunk�� �����ϸ� �ȵǹǷ�
	// RefCounting�� (����)�ϱ����� SendBuffer�� �ڽ��� �����ϰ��ִ� SendBufferChunk�� �˾ƾ��Ѵ�.
	SendBufferChunkRef	m_owner;

public:
	SendBuffer(SendBufferChunkRef _owner, BYTE* _buffer, uint32 _allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return m_buffer; }
	uint32		AllocSize() { return m_allocSize; }
	uint32		WriteSize() { return m_writeSize; }
	void		Close(uint32 _writeSize);
};

/*----------------------------
		SendBufferChunk
------------------------------*/

// SendBufferManager���� TLS�� ����� SendBufferChunk�� ���� ����ϹǷ� ��Ƽ������ ȯ���� �ƴ� �̱۾����� ȯ������ �����ϸ�ȴ�.(Lock �ʿ�X)
// enable_shared_from_this �� �� public�� �����ϸ� ������ ����.(�ڽ��� weak_ptr�� ä���ִ� �κ��� ������� �ʱ� ����)
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		m_buffer = {};
	bool									m_open = false;
	uint32									m_usedSize = 0;

public:
	SendBufferChunk();
	~SendBufferChunk();

	// �ʱ�ȭ
	void				Reset();
	SendBufferRef		Open(uint32 _allocSize);
	void				Close(uint32 _writeSize);

	bool				IsOpen() { return m_open; }
	// m_usedSize���� ��������� �� ���ĺ��� ����ϵ��� �ش� ���� ������ ��ȯ
	BYTE*				Buffer() { return &m_buffer[m_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(m_buffer.size()) - m_usedSize; }
};

/*----------------------------
		SendBufferManager
------------------------------*/

// �����庰�� SendBuffer�� �ּ�(TLS Ȱ��) Lock�� ��κ��� ��Ȳ���� ���� �ʵ��� ����
class SendBufferManager
{
private:
	USE_LOCK;
	Vector<SendBufferChunkRef> m_sendBufferChunks;

public:
	// ���������� ���������� ����� �Լ�
	// �̸� ������ ū ���ۿ��� ����� ũ�⸸ŭ �ɰ��� ����ϵ��� �����ִ� �Լ�
	SendBufferRef		Open(uint32 _size);

private:
	// ��������
	SendBufferChunkRef	Pop();
	// �ݳ�
	void				Push(SendBufferChunkRef _buffer);

	static void			PushGlobal(SendBufferChunk* _buffer);
};
