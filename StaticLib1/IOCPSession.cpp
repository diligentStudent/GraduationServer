#include "stdafx.h"
#include "IOCPSession.h"

IoData::IoData()
	:ioType_(IO_ERROR)
{
	ZeroMemory(&overlapped_, sizeof(overlapped_));

	this->clear();
}

void IoData::clear()
{
	buffer_.fill(0);
	totalBytes_ = 0;
	currentBytes_ = 0;
}

bool IoData::needMoreIO(size_t transferSize)
{
	currentBytes_ += transferSize;
	if (currentBytes_ < totalBytes_)
	{
		return true;
	}
	return false;
}

int32_t IoData::setupTotalBytes()
{
	packet_size_t offset = 0;
	packet_size_t packetLen[1] = { 0, };
	if (totalBytes_ == 0)
	{
		::memcpy_s((void *)packetLen, sizeof(packetLen), (void *)buffer_.data(), sizeof(packetLen));
		//PacketObfuscation::getInstance().decodingHeader((Byte*)&packetLen, sizeof(packetLen));

		totalBytes_ += (size_t)packetLen[0];
	}
	else
	{
		::memcpy_s((void *)packetLen, sizeof(packetLen), (void *)buffer_.data(), sizeof(packetLen));
		//PacketObfuscation::getInstance().decodingHeader((Byte*)&packetLen, sizeof(packetLen));

		totalBytes_ += (size_t)packetLen[0];
		return offset;
	}
	offset += sizeof(packetLen);

	return offset;
}

size_t IoData::totalByte()
{
	return totalBytes_;
}

IO_OPERATION &IoData::type()
{
	return ioType_;
}

void IoData::setType(IO_OPERATION type)
{
	ioType_ = type;
}

char* IoData::data()
{
	return buffer_.data();
}

bool IoData::setData(Stream &stream)
{
	this->clear();

	if (buffer_.max_size() <= stream.size())
	{
		SLog(L"! packet size too big [%d]byte", stream.size());
		return false;
	}

	const size_t packetHeaderSize = sizeof(packet_size_t);
	packet_size_t offset = 0;

	char *buf = buffer_.data();
	//									 head size  + real data size
	packet_size_t packetLen[1] = { (packet_size_t)packetHeaderSize + (packet_size_t)stream.size(), };
	// insert packet len
	::memcpy_s(buf + offset, buffer_.max_size(), (void *)packetLen, packetHeaderSize);
	offset += packetHeaderSize;

	// packet obfuscation
	//PacketObfuscation::getInstance().encodingHeader((Byte*)buf, packetHeaderSize);
	//PacketObfuscation::getInstance().encodingData((Byte*)stream.data(), stream.size());

	// insert packet data
	::memcpy_s(buf + offset, buffer_.max_size(), stream.data(), (int32_t)stream.size());
	offset += (packet_size_t)stream.size();

	totalBytes_ = offset;
	return true;
}

LPWSAOVERLAPPED IoData::overlapped()
{
	return &overlapped_;
}

WSABUF IoData::wsabuf()
{
	WSABUF wsaBuf;
	wsaBuf.buf = buffer_.data() + currentBytes_;
	wsaBuf.len = (ULONG)(totalBytes_ - currentBytes_);

	return wsaBuf;
}

//-----------------------------------------------------------------//
IOCPSession::IOCPSession()
	: Session()
{
	this->initialize();
}

void IOCPSession::initialize()
{
	ZeroMemory(&socketData_, sizeof(SOCKET_DATA));
	ioData_[IO_READ].setType(IO_READ);
	ioData_[IO_WRITE].setType(IO_WRITE);
}

void IOCPSession::checkErrorIO(DWORD ret)
{
	if (ret == SOCKET_ERROR
		&& (::WSAGetLastError() != ERROR_IO_PENDING))
	{
		SLog(L"! socket error: %d", ::WSAGetLastError());
	}
}

void IOCPSession::recv(WSABUF wsaBuf)
{
	DWORD flags = 0;
	DWORD recvBytes;
	DWORD errorCode = ::WSARecv(socketData_.socket_, &wsaBuf, 1, &recvBytes, &flags, ioData_[IO_READ].overlapped(), NULL);
	this->checkErrorIO(errorCode);
}

bool IOCPSession::isRecving(size_t transferSize)
{
	if (ioData_[IO_READ].needMoreIO(transferSize))
	{
		this->recv(ioData_[IO_READ].wsabuf());
		ioData_[IO_READ].setupTotalBytes();
		return true;
	}
	return false;
}

void IOCPSession::recvStandBy()
{
	ioData_[IO_READ].clear();

	WSABUF wsaBuf;
	wsaBuf.buf = ioData_[IO_READ].data();
	wsaBuf.len = SOCKET_BUF_SIZE;

	this->recv(wsaBuf);
}

void IOCPSession::send(WSABUF wsaBuf)
{
	DWORD flags = 0;
	DWORD sendBytes;
	DWORD errorCode = ::WSASend(socketData_.socket_,
		&wsaBuf, 1, &sendBytes, flags,
		ioData_[IO_WRITE].overlapped(), NULL);
	this->checkErrorIO(errorCode);
}

void IOCPSession::onSend(size_t transferSize)
{
	if (ioData_[IO_WRITE].needMoreIO(transferSize))
	{
		this->send(ioData_[IO_WRITE].wsabuf());
	}
}

void IOCPSession::sendPacket(Packet *packet)
{
	Stream stream;
	packet->encode(stream);
	if (!ioData_[IO_WRITE].setData(stream))
	{
		return;
	}

	WSABUF wsaBuf;
	wsaBuf.buf = ioData_[IO_WRITE].data();
	wsaBuf.len = (ULONG)stream.size();

	this->send(wsaBuf);
	//this->recvStandBy();
}

size_t* IoData::Get_currentBytes_()
{
	return &currentBytes_;
}

size_t* IoData::Get_totalBytes_()
{
	return &totalBytes_;
}

void IoData::Set_totalBytes_(size_t settotalBytes)
{
	this->totalBytes_ = settotalBytes;
}

std::array<char, SOCKET_BUF_SIZE>* IoData::Get_wsa_buffer_()
{
	return &buffer_;
}

Package *IOCPSession::onRecv(size_t transferSize)
{
	packet_size_t offset = 0;
	size_t packet_len = 0;
	size_t* packetSize = nullptr;
	size_t check_Databuffer = 0;
	offset += ioData_[IO_READ].setupTotalBytes();//offset에 토탈바이트를 설정

	if (this->isRecving(transferSize))
	{
		return NULL;
	}
	//wsabuffer로 받은 패킷을 main 버퍼로 복사함
	memcpy_s(&main_buffer_ + main_buffer_offset, sizeof(main_buffer_) - main_buffer_offset, ioData_[IO_READ].Get_wsa_buffer_(), *ioData_[IO_READ].Get_currentBytes_());
	main_buffer_offset += *ioData_[IO_READ].Get_currentBytes_();//복사한만큼 offset을 이동
	ioData_[IO_READ].Set_totalBytes_(main_buffer_offset);
	while (true)
	{
		//packet_len을보고 main_buffer_안에 패킷을 처리해주는 구문
		if (sizeof(packet_len) > main_buffer_offset) { SLog(L"no Data in main_Buffer"); break; }//main_buffer_안에 읽을 내용이 없을때
		//길이를알지만 totalbytes가 패킷 길이와 비교했을때 작을때
		memcpy(&packet_len, &main_buffer_, sizeof(packet_size_t));
		//Get_total_bytes가 앞에 헤더만 읽어서 진정한 totalBytes라고 할 수 없음
		if (packet_len > *ioData_[IO_READ].Get_totalBytes_()) { SLog(L"not enough Data to read packet in main_Buffer"); break; }
		
		const size_t packetHeaderSize = sizeof(packet_size_t);
		packet_size_t packetDataSize = (packet_size_t)(packet_len - packetHeaderSize);
		Byte packetData[200] = { NULL, };
		//packetData에 mainbuffer_에있는 패킷의 데이터만큼 잘라서 넣어줌
		memcpy(packetData,//dest
			main_buffer_ + offset,//source
			packetDataSize);//복사할 byte 

		Packet *packet = PacketAnalyzer::getInstance().analyzer((const char *)packetData, packetDataSize);
		
		if (packet == nullptr)
		{
			
			SLog(L"! [%d],invaild packet", packet->type());
			this->onClose(true);
			ioData_[IO_READ].Set_totalBytes_(main_buffer_offset);
			break;
		}

		//SLog(L"onRecv: packettype: %d, packetSize: %d, ", packet->type(), transferSize);
		
		SLog(L"%d", packet->type());
		
		Package *package = new Package(this, packet);
		if (packet != NULL)
		{
			//packetStorage에 넣는 구문
			PutPackage(package);
		}
		memcpy(main_buffer_, main_buffer_ + packet_len, sizeof(main_buffer_) - packet_len);
		main_buffer_offset -= packet_len;
		ioData_[IO_READ].Set_totalBytes_(main_buffer_offset);
	}

	this->recvStandBy();
	return NULL;
}

bool IOCPSession::IsEmpty()
{
	bool temp;
	AcquireSRWLockShared(&lock_);
	temp = packageStorage.empty();
	ReleaseSRWLockShared(&lock_);
	return temp;
}

void IOCPSession::PutPackage(Package* package_)
{
	AcquireSRWLockExclusive(&lock_);
	packageStorage.push_back(package_);
	ReleaseSRWLockExclusive(&lock_);
}

Package* IOCPSession::PopPackage()
{
	Package* temp;
	AcquireSRWLockExclusive(&lock_);
	temp = packageStorage.front();
	packageStorage.pop_front();
	ReleaseSRWLockExclusive(&lock_);
	return temp;
}