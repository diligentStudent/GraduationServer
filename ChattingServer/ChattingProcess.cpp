#include "pch.h"
#include "ChattingProcess.h"

ChattingProcess::ChattingProcess()
	:ContentsProcess()
{
	this->initialize();
}

ChattingProcess::~ChattingProcess()
{

}

void ChattingProcess::initialize()
{
	ContentsProcess::initialize();
	this->registSubPacketFunc();
}

void ChattingProcess::release()
{
	ContentsProcess::release();
}

void ChattingProcess::registSubPacketFunc()
{
#define INSERT_PACKET_PROCESS(type)		runFuncTable_.insert(std::make_pair(E_##type, &ChattingProcess::##type))

	INSERT_PACKET_PROCESS(I_CHTTING_NOTIFY_ID);
	INSERT_PACKET_PROCESS(I_DB_ANS_PARSE_DATA);
	INSERT_PACKET_PROCESS(C_REQ_REGIST_CHATTING_NAME);
	INSERT_PACKET_PROCESS(C_REQ_CHATTING);
	INSERT_PACKET_PROCESS(C_REQ_EXIT);
	INSERT_PACKET_PROCESS(C_REQ_LOBBY_INFO);
	INSERT_PACKET_PROCESS(C_REQ_ENTER_ROOM);
	INSERT_PACKET_PROCESS(I_INITIALIZE_ROOMMEMBER);
}

//---------------------------------------------------------------//
//��Ŷ ó�� ����
void ChattingProcess::I_CHTTING_NOTIFY_ID(Session *session, Packet *rowPacket)
{
	PK_I_CHTTING_NOTIFY_ID *packet = (PK_I_CHTTING_NOTIFY_ID *)rowPacket;

	PK_I_DB_REQ_LOAD_DATA dbPacket;
	dbPacket.clientId_ = packet->clientId_;
	dbPacket.oidAccountId_ = packet->oidAccountId_;

	Terminal *terminal = TERMINALMANAGER.get(L"DBAgent");
	terminal->sendPacket(&dbPacket);
}

void ChattingProcess::I_DB_ANS_PARSE_DATA(Session *session, Packet *rowPacket)
{
	PK_I_DB_ANS_PARSE_DATA *packet = (PK_I_DB_ANS_PARSE_DATA  *)rowPacket;

	PK_I_LOGIN_NOTIFY_ID_LOADED iPacket;
	iPacket.clientId_ = packet->clientId_;
	iPacket.name_ = packet->name_;
	iPacket.result_ = packet->result_;

	SLog(L"* [%S] name load from db", iPacket.name_.c_str());
	Terminal *terminal = TERMINALMANAGER.get(L"LoginServer");
	terminal->sendPacket(&iPacket);
}

void ChattingProcess::C_REQ_REGIST_CHATTING_NAME(Session *session, Packet *rowPacket)
{
	PK_C_REQ_REGIST_CHATTING_NAME *packet = (PK_C_REQ_REGIST_CHATTING_NAME *)rowPacket;
	User *user = USERMANAGER.at(session->id());
	if (user != nullptr)
	{
		SLog(L"! try duplicate regist : %s, name : %S", session->clientAddress().c_str(), packet->name_.c_str());
		session->onClose();
		return;
	}
	user = new User(session);

	std::array<WCHAR, SIZE_64> userName;
	StrConvA2W((CHAR *)packet->name_.c_str(), userName.data(), userName.size());
	user->setName(userName.data());
	USERMANAGER.insert(user);

	SLog(L"* user [%s] created from [%S]", userName.data(), session->clientAddress().c_str());
}

void ChattingProcess::C_REQ_CHATTING(Session *session, Packet *rowPacket)
{
	PK_C_REQ_CHATTING *packet = (PK_C_REQ_CHATTING *)rowPacket;
	User *user = USERMANAGER.at(session->id());
	if (user == nullptr)
	{
		SLog(L"! not registed : %s", session->clientAddress().c_str());
		session->onClose();
		return;
	}

	PK_S_ANS_CHATTING retPacket;
	std::array<char, SIZE_64> name;
	StrConvW2A((WCHAR *)user->name().c_str(), name.data(), name.size());
	retPacket.name_ = name.data();
	retPacket.text_ = packet->text_;

	SLog(L"* send message %S, %S", retPacket.name_.c_str(), retPacket.text_.c_str());

	std::map<oid_t, Session*>::iterator it = LOBBYVALUE.session_storage.begin();
	while (it != LOBBYVALUE.session_storage.end())
	{
		Session* tempsession = it->second;
		if (tempsession->socket() == NULL)
		{
			it++;
			continue;
		}
		tempsession->sendPacket(&retPacket);
		it++;
	}
	//session->sendPacket(&retPacket);
}

void ChattingProcess::C_REQ_EXIT(Session *session, Packet *rowPacket)
{
	//Ŭ���̾�Ʈ read thread �� ������� �ֱ� ���� ó��
	PK_C_REQ_EXIT *packet = (PK_C_REQ_EXIT *)rowPacket;
	User *user = USERMANAGER.at(session->id());
	if (user == nullptr)
	{
		SLog(L"! not registed : %s", session->clientAddress().c_str());
		session->onClose();
		return;
	}
	USERMANAGER.remove(session->id());

	PK_S_ANS_EXIT ansPacket;
	SLog(L"* recv exit packet by [%s]", session->clientAddress().c_str());
	session->sendPacket(&ansPacket);
}

void ChattingProcess::C_REQ_LOBBY_INFO(Session *session, Packet *rowPacket)
{
	PK_C_REQ_LOBBY_INFO *packet = (PK_C_REQ_LOBBY_INFO*)rowPacket;
	LOBBYVALUE.session_storage.insert(std::make_pair(session->id(), session));
	UInt64 roomnum_ = 0;
	auto first = LOBBYVALUE.Get_room_array().begin();
	auto end = LOBBYVALUE.Get_room_array().end();
	while (first != end)
	{
		PK_S_ANS_LOBBY_INFO retpacket;
		retpacket.Room_num = roomnum_;
		retpacket.member_count = first->second->Get_Current_member();
		session->sendPacket(&retpacket);
		first++;
		++roomnum_;
		SLog(L"Send Room memeber: [%d], member count: [%d] to %s", retpacket.Room_num, retpacket.member_count, session->clientAddress().c_str());
	}
}

void ChattingProcess::C_REQ_ENTER_ROOM(Session *session, Packet *rowPacket)
{
	PK_C_REQ_ENTER_ROOM* packet = (PK_C_REQ_ENTER_ROOM*)rowPacket;
	if (!packet)
	{
		SLog(L"PK_C_REQ_ENTER_ROOM doesn't exit");
		return;
	}
	//LOBBYVALUE.room_array[packet->Room_num]->LockEvent();
	if (LOBBYVALUE.room_array[packet->Room_num]->Get_Current_member() >= 2)
	{
		SLog(L"this room is already full");
		return;
	}

	PK_S_ANS_ENTER_ROOM retpacket;
	//��ȿ� �ο����� �ϳ�������Ų�� member_count�� �־���
	LOBBYVALUE.room_array[packet->Room_num]->Set_Current_member(++packet->member_count);//���� �ο��� �Ѹ� �ø���
	//ó�������� user1�� �ι�°������ user2�� �־��ش�
	if (LOBBYVALUE.room_array[packet->Room_num]->user1 == NULL)
		LOBBYVALUE.room_array[packet->Room_num]->user1 = session;

	else
		LOBBYVALUE.room_array[packet->Room_num]->user2 = session;

	retpacket.member_count = LOBBYVALUE.room_array[packet->Room_num]->Get_Current_member();//���� �ο��� �־��ش�
	//���ȣ�� �����Ͽ� ������
	retpacket.Room_num = packet->Room_num;

	SLog(L"[%s] enter the room number %d.", session->clientAddress().c_str(), packet->Room_num);

	//session��ü�� ������
	std::map<oid_t, Session*>::iterator it = LOBBYVALUE.session_storage.begin();
	while (it != LOBBYVALUE.session_storage.end())
	{
		Session* tempsession = it->second;
		if (tempsession->socket() == NULL)
		{
			it++;
			continue;
		}
		tempsession->sendPacket(&retpacket);
		it++;
	}

	if (LOBBYVALUE.room_array[packet->Room_num]->Get_Current_member() == 2)
	{
		//InGameServer�͹̳��� ip�� port��ȣ�� �����ֱ����� InGameServer�� ������������
		Terminal *terminal = TERMINALMANAGER.get(L"InGameServer");
		if (terminal == nullptr)
		{
			SLog(L"! Chatting Server terminal is not connected message from C_REQ_ENTER_ROOM");
		}
		//GameServer�� user1�� 2�� id�� �˷���
		PK_I_USER_INFO passidOfUserInRoom;
		passidOfUserInRoom.room_Num = packet->Room_num;
		passidOfUserInRoom.User1_ID = LOBBYVALUE.room_array[packet->Room_num]->user1->id();
		passidOfUserInRoom.User2_ID = LOBBYVALUE.room_array[packet->Room_num]->user2->id();
		while (true)
		{
			if (SESSIONMANAGER.session(LOBBYVALUE.room_array[packet->Room_num]->user1 != NULL))//User1�� 2���� GameServer�� ip�� port��ȣ�� �˷���
			{
				PK_S_NOTIFY_SERVER_IP notifyingpacket1;
				notifyingpacket1.ip_ = terminal->ip();
				notifyingpacket1.port_ = terminal->port();
				LOBBYVALUE.room_array[packet->Room_num]->user1->sendPacket(&notifyingpacket1);
				SESSIONMANAGER.session(LOBBYVALUE.room_array[packet->Room_num]->user1->id())->sendPacket(&notifyingpacket1);
				LOBBYVALUE.room_array[packet->Room_num]->user1 = NULL;
			}
			//Sleep(20);//���ÿ� Connect�ÿ� port������ ����
			else
			{
				PK_S_NOTIFY_SERVER_IP notifyingpacket2;
				notifyingpacket2.ip_ = terminal->ip();
				notifyingpacket2.port_ = terminal->port();
				LOBBYVALUE.room_array[packet->Room_num]->user2->sendPacket(&notifyingpacket2);
				LOBBYVALUE.room_array[packet->Room_num]->user2 = NULL;
				break;
			}
		}
		//���Ӽ������� ó���ϱ����� Ŭ���̾�Ʈ���� ���� �����̵Ǿ��־���� �̺κ��� �ε����� ó��
		terminal->sendPacket(&passidOfUserInRoom);
		//SESSIONMANAGER.forceCloseSession(LOBBYVALUE.room_array[packet->Room_num]->user1);
		//SESSIONMANAGER.forceCloseSession(LOBBYVALUE.room_array[packet->Room_num]->user2);
		//LOBBYVALUE.session_storage.erase(LOBBYVALUE.room_array[packet->Room_num]->user1->id());
		//LOBBYVALUE.session_storage.erase(LOBBYVALUE.room_array[packet->Room_num]->user2->id());
		LOBBYVALUE.room_array[packet->Room_num]->user1 = NULL;
		LOBBYVALUE.session_storage.erase(passidOfUserInRoom.User1_ID);
		LOBBYVALUE.session_storage.erase(passidOfUserInRoom.User2_ID);


	}
	LOBBYVALUE.room_array[packet->Room_num]->ReleaseEvent();
}

void ChattingProcess::I_INITIALIZE_ROOMMEMBER(Session *session, Packet *rowPacket)
{
	PK_I_INITIALIZE_ROOMMEMBER *packet = (PK_I_INITIALIZE_ROOMMEMBER*)rowPacket;

	LOBBYVALUE.room_array[packet->room_num]->Set_Current_member(packet->member_count);//���� �ο��� 0���� �ʱ�ȭ ��Ŵ
}