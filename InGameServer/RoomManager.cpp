#include "pch.h"
#include "RoomManager.h"

Room::~Room()
{
	SIMPLEUSERMANAGER.Delete_Sessions(User1_ID);
	User1_ID = NULL;
	SIMPLEUSERMANAGER.Delete_Sessions(User2_ID);
	User2_ID = NULL;
}

void RoomManager::Insert_User_Info(int key, Room* room)
{
	SLog(L"%d room insertTo roomArr", key);
	roomArr.insert(make_pair(key,room));
}

bool RoomManager::Delete_User_Info(int key)
{
	return roomArr.erase(key);
}

void SimpleUserManager::Insert_Sessions(oid_t id, Session* session_)
{
	AcquireSRWLockExclusive(&lock_);
	simpleSessionManager.insert(make_pair(id, session_));
	ReleaseSRWLockExclusive(&lock_);
}

bool SimpleUserManager::Delete_Sessions(oid_t key)
{
	bool result;
	AcquireSRWLockExclusive(&lock_);
	result = simpleSessionManager.erase(key);
	ReleaseSRWLockExclusive(&lock_);
	return result;
}

Session* SimpleUserManager::Find_sessions(oid_t key)
{
	AcquireSRWLockShared(&lock_);
	unordered_map<oid_t, Session*>::iterator it = simpleSessionManager.find(key);//find·Î´Â iterator°¡ ³ª¿È
	ReleaseSRWLockShared(&lock_);
	if (it != simpleSessionManager.end())
		return it->second;

	else
		return nullptr;
}