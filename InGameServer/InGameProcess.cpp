#include "pch.h"
#include "InGameProcess.h"

InGameProcess::InGameProcess()
	:ContentsProcess()
{
	this->initialize();
}

InGameProcess::~InGameProcess()
{
}

void InGameProcess::initialize()
{
	ContentsProcess::initialize();
	this->registSubPacketFunc();
}

void InGameProcess::release()
{
	ContentsProcess::release();
}

void InGameProcess::registSubPacketFunc()
{
#define INSERT_PACKET_PROCESS(type)		runFuncTable_.insert(std::make_pair(E_##type, &InGameProcess::##type))
	//ContentsProcess의 run에서 콜백함수로 불러다씀
	INSERT_PACKET_PROCESS(C_REQ_ENEMY_EXPLOSION);
	INSERT_PACKET_PROCESS(C_REQ_PLAYER_MOVE);
	INSERT_PACKET_PROCESS(I_USER_INFO);
	INSERT_PACKET_PROCESS(C_REQ_FIRE_BULLET);
	INSERT_PACKET_PROCESS(C_REQ_MAKE_ENEMY);
	INSERT_PACKET_PROCESS(C_REQ_MAKE_BOSS);
	INSERT_PACKET_PROCESS(C_REQ_BOSS_EXPLOSION);
}

//---------------------------------------------------------------//
//패킷 처리 정의

void InGameProcess::I_USER_INFO(Session *session, Packet *rowPacket)
{
	PK_I_USER_INFO *packet = (PK_I_USER_INFO*)rowPacket;

	Room* tempRoom = new Room();
	tempRoom->User1_ID = packet->User1_ID;
	tempRoom->User2_ID = packet->User2_ID;
	tempRoom->total_exploded_enemy = 0;

	ROOMMANAGER.getInstance().Insert_User_Info(packet->room_Num, tempRoom);
	//Session* temp_user1 = nullptr;
	//Session* temp_user2 = nullptr;

	//while (temp_user1 == NULL || temp_user2 == NULL)
	//{
	//	SLog(L"enter while");
	//	temp_user1 = SESSIONMANAGER.session(tempRoom->User1_ID);
	//	temp_user2 = SESSIONMANAGER.session(tempRoom->User2_ID);
	//}
	Sleep(50);
	Session* temp1 = NULL;
	Session* temp2 = NULL;
	int count = 0;
	do
	{
		if (count >= 3)
		{
			Terminal *terminal = TERMINALMANAGER.get(L"ChattingServer");
			/*if (temp1 != nullptr)
			{*/
				PK_I_INITIALIZE_ROOMMEMBER terminalPacket;
				terminalPacket.room_num = packet->room_Num;
				terminalPacket.member_count = 0;
				terminal->sendPacket(&terminalPacket);
				ROOMMANAGER.getInstance().GetroomArr()->erase(packet->room_Num);
				delete tempRoom;
				/*PK_S_ANS_BOSS_EXPLOSION retpacket;
				retpacket.room_num = packet->room_Num;
				retpacket.ip_ = terminal->ip();
				retpacket.port_ = terminal->port();
				temp1->sendPacket(&retpacket);*/
				return;
			/*}
			PK_I_INITIALIZE_ROOMMEMBER terminalPacket;
			terminalPacket.room_num = packet->room_Num;
			terminalPacket.member_count = 0;
			terminal->sendPacket(&terminalPacket);*/
			//PK_S_ANS_BOSS_EXPLOSION retpacket;
			//retpacket.room_num = packet->room_Num;
			//retpacket.ip_ = terminal->ip();
			//retpacket.port_ = terminal->port();
			//temp2->sendPacket(&retpacket);
			//return;
		}
		count++;
		SLog(L"try find user1 and user2");
		SIMPLEUSERMANAGER.getInstance().Insert_Sessions(tempRoom->User1_ID, SESSIONMANAGER.session(tempRoom->User1_ID));
		SIMPLEUSERMANAGER.getInstance().Insert_Sessions(tempRoom->User2_ID, SESSIONMANAGER.session(tempRoom->User2_ID));
		temp1 = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User1_ID);// ->sendPacket(&retpacket);
		temp2 = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);// ->sendPacket(&retpacket);
	} while (temp1 == NULL || temp2 == NULL);
	PK_S_REQ_ON_GAME	packet1;
	packet1.room_num = packet->room_Num;
	PK_S_REQ_ON_GAME	packet2;
	packet2.room_num = packet->room_Num;
	temp1->sendPacket(&packet1);
	SLog(L"send packet PK_S_REQ_ON_GAME to user1");
	temp2->sendPacket(&packet2);
	SLog(L"send packet PK_S_REQ_ON_GAME to user2");
	/*PK_S_REQ_CHANGE_SCENE resultpacket1;
	resultpacket1.room_num = packet->room_Num;
	Session* temp1 = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User1_ID);
	temp1->sendPacket(&resultpacket1);
	SLog(L"sendpacket pk_s_req_change_scene");

	PK_S_REQ_CHANGE_SCENE resultpacket2;
	resultpacket2.room_num = packet->room_Num;
	Session* temp2 = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);
	temp2->sendPacket(&resultpacket2);
	SLog(L"sendpacket pk_s_req_change_scene");*/
}


void InGameProcess::C_REQ_PLAYER_MOVE(Session *session, Packet *rowPacket)
{
	PK_C_REQ_PLAYER_MOVE *packet = (PK_C_REQ_PLAYER_MOVE*)rowPacket;

	//다른 클라이언트에게 보내줌

	PK_S_ANS_PLAYER_MOVE retpacket;
	retpacket.room_num = packet->Room_num;
	retpacket.x = packet->x;
	retpacket.y = packet->y;
	unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->find(packet->Room_num);

	if (it == ROOMMANAGER.getInstance().GetroomArr()->end())
	{
		SLog(L"there is no Room");
		return;
	}

	Room* tempRoom = it->second;
	//Room* tempRoom = ROOMMANAGER.getInstance().GetroomArr()->find(packet->Room_num)->second;//room을가져옴

	if (tempRoom->User1_ID == session->id())
	{
		//SLog(L"User1 Position x: %f y: %f", packet->x, packet->y);
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);// ->sendPacket(&retpacket);
		if (temp == nullptr)
		{
			SLog(L"User2 cannot connect");
			return;
		}
		temp->sendPacket(&retpacket);
	}
	else if (tempRoom->User2_ID == session->id())
	{
		//SLog(L"User2 Position x: %f y: %f", packet->x, packet->y);
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User1_ID);// ->sendPacket(&retpacket);
		if (temp == nullptr)
		{
			SLog(L"User1 cannot connect");
			return;
		}
		temp->sendPacket(&retpacket);
	}
	//unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->begin();


}

void InGameProcess::C_REQ_FIRE_BULLET(Session *session, Packet *rowPacket)
{
	PK_C_REQ_FIRE_BULLET *packet = (PK_C_REQ_FIRE_BULLET*)rowPacket;

	PK_S_ANS_FIRE_BULLET retpacket;
	retpacket.room_num = packet->room_num;
	retpacket.x = packet->x;
	retpacket.y = packet->y;

	//room을가져옴
	unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num);
	
	if (it == ROOMMANAGER.getInstance().GetroomArr()->end())
	{
		SLog(L"there is no Room [%d]", packet->room_num);
		return;
	}

	Room* tempRoom = it->second;

	if (tempRoom->User1_ID == session->id())
	{
		//SLog(L"User1 BulletPos x: %f y: %f", packet->x, packet->y);
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);// ->sendPacket(&retpacket);
		temp->sendPacket(&retpacket);
	}

	else if (tempRoom->User2_ID == session->id())
	{
		//SLog(L"User2 BulletPos x: %f y: %f", packet->x, packet->y);
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User1_ID);// ->sendPacket(&retpacket);
		temp->sendPacket(&retpacket);
	}
}

void InGameProcess::C_REQ_MAKE_ENEMY(Session *session, Packet *rowPacket)
{
	PK_C_REQ_MAKE_ENEMY *packet = (PK_C_REQ_MAKE_ENEMY*)rowPacket;

	unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num);

	if (it == ROOMMANAGER.getInstance().GetroomArr()->end())
	{
		SLog(L"there is no Room [%d]", packet->room_num);
		return;
	}

	Room* tempRoom = it->second;

	//Room* tempRoom = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num)->second;

	if (tempRoom->total_exploded_enemy >= 1)
	{
		return;
	}

	if (session->id() == tempRoom->User1_ID)
	{
		PK_S_ANS_MAKE_ENEMY retpacket2;
		retpacket2.x = packet->x;
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);// ->sendPacket(&retpacket);
		if (temp == nullptr)
		{
			return;
		}
		temp->sendPacket(&retpacket2);

		//SLog(L"Get C_REQ_MAKE_ENEMY packet from [%s]", session->clientAddress());
		PK_S_ANS_MAKE_ENEMY retpacket1;
		retpacket1.x = packet->x;
		session->sendPacket(&retpacket1);
	}
	else
		return;
}

void InGameProcess::C_REQ_ENEMY_EXPLOSION(Session *session, Packet *rowPacket)
{
	PK_C_REQ_ENEMY_EXPLOSION *packet = (PK_C_REQ_ENEMY_EXPLOSION*)rowPacket;

	unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num);

	if (it == ROOMMANAGER.getInstance().GetroomArr()->end())
	{
		SLog(L"there is no Room [%d]", packet->room_num);
		return;
	}

	Room* tempRoom = it->second;

	//Room* tempRoom = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num)->second;
	if (session->id() == tempRoom->User1_ID && tempRoom->total_exploded_enemy < 5)
	{
		tempRoom->total_exploded_enemy += packet->enemycount;
		PK_S_ANS_ENEMY_EXPLOSION retpacket;
		retpacket.room_num = packet->room_num;
		retpacket.enemycount = tempRoom->total_exploded_enemy;
		session->sendPacket(&retpacket);
	}
}

void InGameProcess::C_REQ_MAKE_BOSS(Session *session, Packet *rowPacket)
{
	PK_C_REQ_MAKE_BOSS *packet = (PK_C_REQ_MAKE_BOSS*)rowPacket;

	unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num);

	if (it == ROOMMANAGER.getInstance().GetroomArr()->end())
	{
		SLog(L"there is no Room [%d]", packet->room_num);
		return;
	}

	Room* tempRoom = it->second;
	//Room* tempRoom = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num)->second;
	if (session->id() == tempRoom->User1_ID)
	{
		PK_S_ANS_MAKE_BOSS retpacket1;
		retpacket1.room_num = packet->room_num;
		session->sendPacket(&retpacket1);

		PK_S_ANS_MAKE_BOSS retpacket2;
		retpacket2.room_num = packet->room_num;
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);
		temp->sendPacket(&retpacket2);
	}
}

void InGameProcess::C_REQ_BOSS_EXPLOSION(Session *session, Packet *rowPacket)
{
	PK_C_REQ_BOSS_EXPLOSION *packet = (PK_C_REQ_BOSS_EXPLOSION*)rowPacket;

	unordered_map<int, Room*>::iterator it = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num);

	if (it == ROOMMANAGER.getInstance().GetroomArr()->end())
	{
		SLog(L"there is no Room [%d]", packet->room_num);
		return;
	}

	Room* tempRoom = it->second;
//	Room* tempRoom = ROOMMANAGER.getInstance().GetroomArr()->find(packet->room_num)->second;

	if (session->id() == tempRoom->User1_ID)
	{
		Terminal *terminal = TERMINALMANAGER.get(L"ChattingServer");
		PK_I_INITIALIZE_ROOMMEMBER terminalPacket;
		terminalPacket.room_num = packet->room_num;
		terminalPacket.member_count = 0;
		terminal->sendPacket(&terminalPacket);

		PK_S_ANS_BOSS_EXPLOSION retpacket1;
		retpacket1.room_num = packet->room_num;
		retpacket1.ip_ = terminal->ip();
		retpacket1.port_ = terminal->port();
		session->sendPacket(&retpacket1);
		//SIMPLEUSERMANAGER.Delete_Sessions(tempRoom->User1_ID);
		PK_S_ANS_BOSS_EXPLOSION retpacket2;
		retpacket2.room_num = packet->room_num;
		retpacket2.ip_ = terminal->ip();
		retpacket2.port_ = terminal->port();
		Session* temp = SIMPLEUSERMANAGER.Find_sessions(tempRoom->User2_ID);
		temp->sendPacket(&retpacket2);
		//SIMPLEUSERMANAGER.Delete_Sessions(tempRoom->User2_ID);
		SLog(L"send packet S_ANS_BOSS_EXPLOSSION");
		ROOMMANAGER.getInstance().GetroomArr()->erase(packet->room_num);
		delete tempRoom;
	}
	//ROOMMANAGER.Delete_User_Info(packet->room_num);
}