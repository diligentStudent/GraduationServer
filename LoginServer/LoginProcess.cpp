#include "pch.h"
#include "LoginProcess.h"

LoginProcess::LoginProcess()
	:ContentsProcess()
{
	this->initialize();
}

LoginProcess::~LoginProcess()
{
}

void LoginProcess::initialize()
{
	ContentsProcess::initialize();

	this->registSubPacketFunc();
}

void LoginProcess::release()
{
	ContentsProcess::release();
}

void LoginProcess::registSubPacketFunc()
{
#define INSERT_PACKET_PROCESS(type)		runFuncTable_.insert(std::make_pair(E_##type, &LoginProcess::##type))

	INSERT_PACKET_PROCESS(C_REQ_ID_PW);				//Ÿ�Կ� ���� �Լ� �����͸� �ҷ���
	INSERT_PACKET_PROCESS(I_DB_ANS_ID_PW);			
	INSERT_PACKET_PROCESS(I_LOGIN_NOTIFY_ID_LOADED);
}

//---------------------------------------------------------------//
//��Ŷ ó�� ����
void LoginProcess::C_REQ_ID_PW(Session *session, Packet *rowPacket)
{
	PK_C_REQ_ID_PW *packet = (PK_C_REQ_ID_PW *)rowPacket;

	PK_I_DB_REQ_ID_PW dbPacket;
	dbPacket.clientId_ = (UInt64)session->id();
	dbPacket.id_ = packet->id_;
	dbPacket.password_ = packet->password_;

	Terminal *terminal = TERMINALMANAGER.get(L"DBAgent");
	terminal->sendPacket(&dbPacket);
}

void LoginProcess::I_DB_ANS_ID_PW(Session *session, Packet *rowPacket)
{
	PK_I_DB_ANS_ID_PW *packet = (PK_I_DB_ANS_ID_PW  *)rowPacket;
	SLog(L"* id/ pw result = %d", packet->result_);

	Session *clientSession = SESSIONMANAGER.session(packet->clientId_);
	if (clientSession == nullptr) 
	{
		SLog(L"LoginProcess::I_DB_ANS_ID_PW --- clientSession is not exist");
		return;
	}

	const int authFail = 0;
	if (packet->result_ == authFail) 
	{
		PK_S_ANS_ID_PW_FAIL ansPacket;
		clientSession->sendPacket(&ansPacket);//Ŭ���̾�Ʈ���� ���� ��Ŷ�� ������
		return;
	}

	PK_I_CHTTING_NOTIFY_ID iPacket;
	iPacket.oidAccountId_ = packet->oidAccountId_;
	iPacket.clientId_ = packet->clientId_;
	Terminal *terminal = TERMINALMANAGER.get(L"ChattingServer");
	if (terminal == nullptr) 
	{
		SLog(L"! Chatting Server terminal is not connected");
	}
	terminal->sendPacket(&iPacket);//ä�ü����� ��Ŷ�� ������
}

void LoginProcess::I_LOGIN_NOTIFY_ID_LOADED(Session *session, Packet *rowPacket)
{
	PK_I_LOGIN_NOTIFY_ID_LOADED *packet = (PK_I_LOGIN_NOTIFY_ID_LOADED *)rowPacket;

	const int dataNull = 0;
	if (packet->result_ == dataNull)
	{
		return;
	}
	Session *clientSession = SESSIONMANAGER.session(packet->clientId_);
	if (clientSession == nullptr) 
	{
		return;
	}
	Terminal *terminal = TERMINALMANAGER.get(L"ChattingServer");
	if (terminal == nullptr) 
	{
		SLog(L"! Chatting Server terminal is not connected");
	}
	PK_S_ANS_ID_PW_SUCCESS ansPacket;
	SLog((WCHAR*)L"������ Success��Ŷ ");
	ansPacket.ip_ = terminal->ip();
	ansPacket.port_ = terminal->port();
	ansPacket.name_ = packet->name_;

	SLog(L"* loaded [%S] user name, from [%s]", ansPacket.name_.c_str(), session->clientAddress().c_str());
	clientSession->sendPacket(&ansPacket);
}