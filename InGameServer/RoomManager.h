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
	unordered_map<int, Room*> roomArr;//ã�ƺ��� unorered_map���ؽ����� ����ϱ⶧���� Ž���ӵ���1�̰� map�� �����Ʈ���� ����ϱ⶧���� Ž���ӵ��� logN�̴� ���� ��Ŷó���ϱ����� ���⼭�� unorderedmap�� ����� 

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