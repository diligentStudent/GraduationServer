#pragma once

using namespace std;
class Room {
public:
	~Room();
	UInt64 User1_ID;
	UInt64 User2_ID;
	UInt64 total_exploded_enemy;
};

class RoomManager : public Singleton < RoomManager >
{
private:
	unordered_map<int, Room*> roomArr;//찾아보니 unorered_map은해쉬맵을 사용하기때문에 탐색속도가1이고 map은 레드블랙트리를 사용하기때문에 탐색속도가 logN이다 빠른 패킷처리하기위해 여기서는 unorderedmap을 사용함 

public:
	void Insert_User_Info(int key, Room* room);
	bool Delete_User_Info(int key);
	unordered_map<int, Room*>* GetroomArr() { return &roomArr; }
};

class SimpleUserManager :public Singleton <SimpleUserManager>
{
private:
	unordered_map<oid_t, Session*> simpleSessionManager;
	SRWLOCK lock_;
public:
	void Insert_Sessions(oid_t id, Session* session_);
	bool Delete_Sessions(oid_t key);
	Session* Find_sessions(oid_t key);
	unordered_map<oid_t, Session*>* GetsimpleSessionManager() { return &simpleSessionManager; }
};