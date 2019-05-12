#include "pch.h"
#include "LobbyValue.h"

void LobbyManager::Init(Int32 num)
{
	//array�� ���ο� �޾Ƽ� ����
	total_room_num = num;//for���� ���� session������������ room������ŭ send�� ���������� room total�� �����ϴ� ����
	
	for (int i = 0; i < total_room_num; i++)
	{
		Room* p_Temproom = new Room;
		p_Temproom->user1 = NULL;
		p_Temproom->user2 = NULL;
		p_Temproom->Set_Current_member(0);
		room_array.insert(std::make_pair(i,p_Temproom));//key, value���� ����
	}
}

void LobbyManager::Deletesession(oid_t key)
{
	session_storage.erase(key);
}

int Room::Get_Current_member()
{
	return Current_member_in_room;
}

void Room::Set_Current_member(int set_member)
{
	Current_member_in_room = set_member;
}

void Room::LockEvent()
{
	ResetEvent(h_Event);
}

void Room::ReleaseEvent()
{
	SetEvent(h_Event);
}

Int32 LobbyManager::Get_total_room_num() { return total_room_num; }
std::map<int, Room*>& LobbyManager::Get_room_array() { return room_array; }