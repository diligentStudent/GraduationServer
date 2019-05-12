#pragma once


typedef enum
{
	IO_READ,
	IO_WRITE,
	IO_ERROR,
} IO_OPERATION;

#define IO_DATA_MAX     (2)

class IoData
{
private:
	OVERLAPPED		overlapped_;
	IO_OPERATION	ioType_;
	size_t  		totalBytes_;
	size_t			currentBytes_;
	std::array<char, SOCKET_BUF_SIZE> buffer_;

public:
	IoData();
	~IoData() {}

	/****************************************************************************
	�Լ���	: clear
	����		: IoData Ŭ������ �����͸� 0���� �ʱ�ȭ��
	���ϰ�	: ����
	�Ű�����	: ����
	*****************************************************************************/
	void clear();

	/****************************************************************************
	�Լ���	: needMoreIO
	����		: IO�� �� �ʿ����� �Ǻ���
	���ϰ�	: bool
	�Ű�����	: size_t
	*****************************************************************************/
	bool needMoreIO(size_t transferSize);

	/****************************************************************************
	�Լ���	: setupTotalBytes
	����		: totalBytes_�� ������, ���� �������� ���� �� ��Ŷ ũ�� ��ȯ
	���ϰ�	: int32_t
	�Ű�����	: ����
	*****************************************************************************/
	int32_t setupTotalBytes();

	/****************************************************************************
	�Լ���	: wsabuf
	����		: ������� �� ����, ���� ���۸� ������
	���ϰ�	: WSABUF
	�Ű�����	: ����
	*****************************************************************************/
	WSABUF wsabuf();

	/****************************************************************************
	�Լ���	: setData
	����		: buffer_�� stream�� ũ�⸦ �����, ���۸� �ڿ� �߰����ִ� ��Ŷ ����, totalBytes_ ����
	���ϰ�	: bool
	�Ű�����	: Stream&
	*****************************************************************************/
	bool setData(Stream &stream);
	size_t* Get_currentBytes_();
	size_t* Get_totalBytes_();
	void Set_totalBytes_(size_t settotalBytes);
	std::array<char, SOCKET_BUF_SIZE>* Get_wsa_buffer_();
public:
	//get�Լ�
	size_t totalByte();
	IO_OPERATION &type();

	char *data();
	LPWSAOVERLAPPED overlapped();

	//set�Լ�
	void setType(IO_OPERATION type);

};

//-----------------------------------------------------------------//
class IOCPSession : public Session
{
private:
	//�ʱ�ȭ
	void			initialize();

	//IO���� üũ��
	void			checkErrorIO(DWORD ret);

	//��Ĺ recv ���ְ� ���� üũ��
	void			recv(WSABUF wsaBuf);
	//���� �ް� �ֳ� üũ
	bool			isRecving(size_t transferSize);
	//wsaBuf���� �۽�, ���� üũ��
	void			send(WSABUF wsaBuf);

	SRWLOCK			lock_;
public:
	std::array<IoData, IO_DATA_MAX> ioData_;
	Byte main_buffer_[1024 * 1000];
	int main_buffer_offset;
	std::list<Package*> packageStorage;

public:
	IOCPSession();
	virtual ~IOCPSession() {}

	/****************************************************************************
	�Լ���	: onSend
	����		: ��Ŷ ������ On�̸� ��Ŷ �۽���
	���ϰ�	: ����
	�Ű�����	: size_t
	*****************************************************************************/
	void			onSend(size_t transferSize);

	/****************************************************************************
	�Լ���	: sendPacket
	����		: packet�����͸� ioData_ ���·� ���� �� send��
	���ϰ�	: ����
	�Ű�����	: Packet*
	*****************************************************************************/
	void		    sendPacket(Packet *packet);

	/****************************************************************************
	�Լ���	: onRecv
	����		: ������ �м�, Recv ������ ��Ŷ ��������
	���ϰ�	: Package*
	�Ű�����	: size_t
	*****************************************************************************/
	Package*		onRecv(size_t transferSize);

	/****************************************************************************
	�Լ���	: recvStandBy
	����		: ioData_[IO_READ] ���� �ʱ�ȭ, ������ recv���·� �ٲ���
	���ϰ�	: ����
	�Ű�����	: ����
	*****************************************************************************/
	void			recvStandBy();
	bool			IsEmpty();
	void			PutPackage(Package* package_);
	Package*		PopPackage();
};
