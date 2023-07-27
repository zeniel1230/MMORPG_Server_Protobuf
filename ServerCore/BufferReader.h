#pragma once

/*----------------------------
		  BufferReader
------------------------------*/

class BufferReader
{
private:
	BYTE*			m_buffer = nullptr;
	uint32			m_size = 0;
	uint32			m_pos = 0;
					
public:
	BufferReader();
	BufferReader(BYTE* _buffer, uint32 _size, uint32 _pos = 0);
	~BufferReader();

	BYTE*			Buffer() { return m_buffer; }
	uint32			Size() { return m_size; }
	uint32			ReadSize() { return m_pos; }
	uint32			FreeSize() { return m_size - m_pos; }

	template<typename T>
	bool			Peek(T* _dest) { return Peek(_dest, sizeof(T)); }
	bool			Peek(void* _dest, uint32 _len);

	template<typename T>
	bool			Read(T* _dest) { return Read(_dest, sizeof(T)); }
	bool			Read(void* _dest, uint32 _len);

	template<typename T>
	BufferReader&	operator>>(OUT T& _dest);
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& _dest)
{
	// ���� ��ġ���� TŸ������ ĳ������ ������ �������ش�.
	_dest = *reinterpret_cast<T*>(&m_buffer[m_pos]);
	// �����͸� �о����Ƿ� �ڵ����� ������ �������Բ� ����
	m_pos += sizeof(T);
	return *this;
}