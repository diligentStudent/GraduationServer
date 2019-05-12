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
//패킷 처리 정의
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
	//클라이언트 read thread 를 종료시켜 주기 위해 처리
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
	//방안에 인원수를 하나증가시킨후 member_count에 넣어줌
	LOBBYVALUE.room_array[packet->Room_num]->Set_Current_member(++packet->member_count);//현제 인원을 한명 늘린다
	//처음들어오면 user1에 두번째들어오면 user2에 넣어준다
	if (LOBBYVALUE.room_array[packet->Room_num]->user1 == NULL)
		LOBBYVALUE.room_array[packet->Room_num]->user1 = session;

	else
		LOBBYVALUE.room_array[packet->Room_num]->user2 = session;

	retpacket.member_count = LOBBYVALUE.room_array[packet->Room_num]->Get_Current_member();//현제 인원을 넣어준다
	//방번호를 배정하여 보내줌
	retpacket.Room_num = packet->Room_num;

	SLog(L"[%s] enter the room number %d.", session->clientAddress().c_str(), packet->Room_num);

	//session전체에 보내줌
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
		//InGameServer터미널의 ip와 port번호를 보내주기위해 InGameServer의 정보를가져옴
		Terminal *terminal = TERMINALMANAGER.get(L"InGameServer");
		if (terminal == nullptr)
		{
			SLog(L"! Chatting Server terminal is not connected message from C_REQ_ENTER_ROOM");
		}
		//GameServer에 user1과 2의 id를 알려줌
		PK_I_USER_INFO passidOfUserInRoom;
		passidOfUserInRoom.room_Num = packet->Room_num;
		passidOfUserInRoom.User1_ID = LOBBYVALUE.room_array[packet->Room_num]->user1->id();
		passidOfUserInRoom.User2_ID = LOBBYVALUE.room_array[packet->Room_num]->user2->id();
		while (true)
		{
			if (SESSIONMANAGER.session(LOBBYVALUE.room_array[packet->Room_num]->user1 != NULL))//User1과 2에게 GameServer의 ip와 port번호를 알려줌
			{
				PK_S_NOTIFY_SERVER_IP notifyingpacket1;
				notifyingpacket1.ip_ = terminal->ip();
				notifyingpacket1.port_ = terminal->port();
				LOBBYVALUE.room_array[packet->Room_num]->user1->sendPacket(&notifyingpacket1);
				SESSIONMANAGER.session(LOBBYVALUE.room_array[packet->Room_num]->user1->id())->sendPacket(&notifyingpacket1);
				LOBBYVALUE.room_array[packet->Room_num]->user1 = NULL;
			}
			//Sleep(20);//동시에 Connect시에 port오류가 생김
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
		//게임서버에서 처리하기전에 클라이언트들이 먼저 연결이되어있어야함 이부분은 로딩으로 처리
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

	LOBBYVALUE.room_array[packet->room_num]->Set_Current_member(packet->member_count);//현제 인원을 0으로 초기화 시킴
}