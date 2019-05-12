#pragma once

class PacketFactory : public Singleton<PacketFactory>
{
	friend Singleton;
public:
	Packet* getPacket(Int64 packetType)
	{
		switch (packetType) {
		case E_C_REQ_EXIT:    return new PK_C_REQ_EXIT();
		case E_S_ANS_EXIT:    return new PK_S_ANS_EXIT();
		case E_I_NOTIFY_TERMINAL:    return new PK_I_NOTIFY_TERMINAL();
		case E_C_NOTIFY_HEARTBEAT:    return new PK_C_NOTIFY_HEARTBEAT();
		case E_C_REQ_ID_PW:    return new PK_C_REQ_ID_PW();
		case E_S_ANS_ID_PW_FAIL:    return new PK_S_ANS_ID_PW_FAIL();
		case E_S_ANS_ID_PW_SUCCESS:    return new PK_S_ANS_ID_PW_SUCCESS();
		case E_I_DB_REQ_ID_PW:    return new PK_I_DB_REQ_ID_PW();
		case E_I_DB_ANS_ID_PW:    return new PK_I_DB_ANS_ID_PW();
		case E_I_CHTTING_NOTIFY_ID:    return new PK_I_CHTTING_NOTIFY_ID();
		case E_I_DB_REQ_LOAD_DATA:    return new PK_I_DB_REQ_LOAD_DATA();
		case E_I_DB_ANS_PARSE_DATA:    return new PK_I_DB_ANS_PARSE_DATA();
		case E_I_LOGIN_NOTIFY_ID_LOADED:    return new PK_I_LOGIN_NOTIFY_ID_LOADED();
		case E_C_REQ_REGIST_CHATTING_NAME:    return new PK_C_REQ_REGIST_CHATTING_NAME();
		case E_C_REQ_CHATTING:    return new PK_C_REQ_CHATTING();
		case E_S_ANS_CHATTING:    return new PK_S_ANS_CHATTING();
		case E_C_REQ_LOBBY_INFO:    return new PK_C_REQ_LOBBY_INFO();
		case E_S_ANS_LOBBY_INFO:    return new PK_S_ANS_LOBBY_INFO();
		case E_C_REQ_ENTER_ROOM:    return new PK_C_REQ_ENTER_ROOM();
		case E_S_ANS_ENTER_ROOM:    return new PK_S_ANS_ENTER_ROOM();
		case E_C_REQ_PLAYER_MOVE:    return new PK_C_REQ_PLAYER_MOVE();
		case E_I_USER_INFO:		return new PK_I_USER_INFO();
		case E_S_ANS_PLAYER_MOVE:		return new PK_S_ANS_PLAYER_MOVE();
		case E_C_REQ_FIRE_BULLET:		return new PK_C_REQ_FIRE_BULLET();
		case E_S_ANS_FIRE_BULLET:		return new PK_S_ANS_FIRE_BULLET();
		case E_C_REQ_MAKE_ENEMY:		return new PK_C_REQ_MAKE_ENEMY();
		case E_S_ANS_MAKE_ENEMY:		return new PK_S_ANS_MAKE_ENEMY();
		case E_C_REQ_ENEMY_EXPLOSION:		return new PK_C_REQ_ENEMY_EXPLOSION();
		case E_S_ANS_ENEMY_EXPLOSION:		return new PK_S_ANS_ENEMY_EXPLOSION();
		case E_C_REQ_MAKE_BOSS:		return new PK_C_REQ_MAKE_BOSS();
		case E_S_ANS_MAKE_BOSS:		return new PK_S_ANS_MAKE_BOSS();
		case E_C_REQ_BOSS_EXPLOSION:		return new PK_C_REQ_BOSS_EXPLOSION();
		case E_S_ANS_BOSS_EXPLOSION:		return new PK_S_ANS_BOSS_EXPLOSION();
		case E_I_INITIALIZE_ROOMMEMBER:		return new PK_I_INITIALIZE_ROOMMEMBER();
		case E_S_REQ_CHANGE_SCENE:		return new PK_S_REQ_CHANGE_SCENE();
		case E_S_REQ_ON_GAME:		return new PK_S_REQ_ON_GAME();
		}
		return nullptr;
	}
};
