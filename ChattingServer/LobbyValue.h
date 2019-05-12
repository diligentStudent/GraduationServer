#pragma once

class Room {
private:
	int Current_member_in_room;
	HANDLE h_Event;
public:
	Session *user1;
	Session *user2;
	Room()
	{
		h_Event = CreateEvent(NULL, FALSE, TRUE, NULL);//자동리셋 이벤트 시그널로시작
		user1 = NULL;
		user2 = NULL;
		Current_member_in_room = 0;
	}
	~Room()
	{
		delete this;
	}

	int Get_Current_member();
	void Set_Current_member(int set_member);
	void LockEvent();
	void ReleaseEvent();
};

class LobbyManager :public Singleton<LobbyManager>
{
	friend Singleton;
private:
	Int32 total_room_num;
public:
	std::map<oid_t,Session*> session_storage;//room안에 member몇명가지고있는지
	std::map<int, Room*> room_array;
	void Init(Int32 num);
	Int32 Get_total_room_num();
	std::map<int, Room*>& Get_room_array();
	void Deletesession(oid_t key);
};