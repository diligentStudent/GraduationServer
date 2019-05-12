#pragma once

class Packet {
public:
	virtual PacketType type() = 0;
	virtual void encode(Stream &stream) { stream << (Int64)this->type(); };
	virtual void decode(Stream &stream) { };
};

class PK_C_REQ_EXIT : public Packet
{
public:
	PacketType type() { return E_C_REQ_EXIT; }

};

class PK_S_ANS_EXIT : public Packet
{
public:
	PacketType type() { return E_S_ANS_EXIT; }

};

class PK_I_NOTIFY_TERMINAL : public Packet
{
public:
	PacketType type() { return E_I_NOTIFY_TERMINAL; }

};

class PK_C_NOTIFY_HEARTBEAT : public Packet
{
public:
	PacketType type() { return E_C_NOTIFY_HEARTBEAT; }

};

class PK_C_REQ_ID_PW : public Packet
{
public:
	PacketType type() { return E_C_REQ_ID_PW; }

	std::string     id_;
	std::string     password_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << id_;
		stream << password_;
	}

	void decode(Stream &stream) {
		stream >> &id_;
		stream >> &password_;
	}
};

class PK_S_ANS_ID_PW_FAIL : public Packet
{
public:
	PacketType type() { return E_S_ANS_ID_PW_FAIL; }

};

class PK_S_ANS_ID_PW_SUCCESS : public Packet
{
public:
	PacketType type() { return E_S_ANS_ID_PW_SUCCESS; }

	std::string     ip_;
	UInt32     port_;
	std::string     name_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << ip_;
		stream << port_;
		stream << name_;
	}

	void decode(Stream &stream) {
		stream >> &ip_;
		stream >> &port_;
		stream >> &name_;
	}
};

class PK_I_DB_REQ_ID_PW : public Packet
{
public:
	PacketType type() { return E_I_DB_REQ_ID_PW; }

	UInt64     clientId_;
	std::string     id_;
	std::string     password_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << clientId_;
		stream << id_;
		stream << password_;
	}

	void decode(Stream &stream) {
		stream >> &clientId_;
		stream >> &id_;
		stream >> &password_;
	}
};

class PK_I_DB_ANS_ID_PW : public Packet
{
public:
	PacketType type() { return E_I_DB_ANS_ID_PW; }

	UInt64     clientId_;
	UInt64     oidAccountId_;
	Byte     result_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << clientId_;
		stream << oidAccountId_;
		stream << result_;
	}

	void decode(Stream &stream) {
		stream >> &clientId_;
		stream >> &oidAccountId_;
		stream >> &result_;
	}
};

class PK_I_CHTTING_NOTIFY_ID : public Packet
{
public:
	PacketType type() { return E_I_CHTTING_NOTIFY_ID; }

	UInt64     clientId_;
	UInt64     oidAccountId_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << clientId_;
		stream << oidAccountId_;
	}

	void decode(Stream &stream) {
		stream >> &clientId_;
		stream >> &oidAccountId_;
	}
};

class PK_I_DB_REQ_LOAD_DATA : public Packet
{
public:
	PacketType type() { return E_I_DB_REQ_LOAD_DATA; }

	UInt64     clientId_;
	UInt64     oidAccountId_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << clientId_;
		stream << oidAccountId_;
	}

	void decode(Stream &stream) {
		stream >> &clientId_;
		stream >> &oidAccountId_;
	}
};

class PK_I_DB_ANS_PARSE_DATA : public Packet
{
public:
	PacketType type() { return E_I_DB_ANS_PARSE_DATA; }

	UInt64     clientId_;
	std::string     name_;
	Byte     result_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << clientId_;
		stream << name_;
		stream << result_;
	}

	void decode(Stream &stream) {
		stream >> &clientId_;
		stream >> &name_;
		stream >> &result_;
	}
};

class PK_I_LOGIN_NOTIFY_ID_LOADED : public Packet
{
public:
	PacketType type() { return E_I_LOGIN_NOTIFY_ID_LOADED; }

	UInt64     clientId_;
	std::string     name_;
	Byte     result_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << clientId_;
		stream << name_;
		stream << result_;
	}

	void decode(Stream &stream) {
		stream >> &clientId_;
		stream >> &name_;
		stream >> &result_;
	}
};

class PK_C_REQ_REGIST_CHATTING_NAME : public Packet
{
public:
	PacketType type() { return E_C_REQ_REGIST_CHATTING_NAME; }

	std::string     name_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << name_;
	}

	void decode(Stream &stream) {
		stream >> &name_;
	}
};

class PK_C_REQ_CHATTING : public Packet
{
public:
	PacketType type() { return E_C_REQ_CHATTING; }

	std::string     text_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << text_;
	}

	void decode(Stream &stream) {
		stream >> &text_;
	}
};

class PK_S_ANS_CHATTING : public Packet
{
public:
	PacketType type() { return E_S_ANS_CHATTING; }

	std::string     name_;
	std::string     text_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << name_;
		stream << text_;
	}

	void decode(Stream &stream) {
		stream >> &name_;
		stream >> &text_;
	}
};

class PK_C_REQ_LOBBY_INFO :public Packet
{
public:
	PacketType type() { return E_C_REQ_LOBBY_INFO; }

	UInt64 member_count;
	UInt64 Room_num;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << member_count;
		stream << Room_num;
	}

	void decode(Stream &stream) {
		stream >> &member_count;
		stream >> &Room_num;
	}
};

class PK_S_ANS_LOBBY_INFO :public Packet
{
public:
	PacketType type() { return E_S_ANS_LOBBY_INFO; }

	UInt64 member_count;
	UInt64 Room_num;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << member_count;
		stream << Room_num;
	}

	void decode(Stream &stream) {
		stream >> &member_count;
		stream >> &Room_num;
	}
};

class PK_C_REQ_ENTER_ROOM :public Packet
{
public:
	PacketType type() { return E_C_REQ_ENTER_ROOM; }

	UInt64 member_count;
	UInt64 Room_num;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << member_count;
		stream << Room_num;
	}

	void decode(Stream &stream) {
		stream >> &member_count;
		stream >> &Room_num;
	}
};

class PK_S_ANS_ENTER_ROOM :public Packet
{
public:
	PacketType type() { return E_S_ANS_ENTER_ROOM; }

	UInt64 member_count;
	UInt64 Room_num;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << member_count;
		stream << Room_num;
	}

	void decode(Stream &stream) {
		stream >> &member_count;
		stream >> &Room_num;
	}
};

class PK_S_NOTIFY_SERVER_IP :public Packet
{
public:
	PacketType type() { return E_S_NOTIFY_SERVER_IP; }
	std::string     ip_;
	UInt32     port_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << ip_;
		stream << port_;
	}

	void decode(Stream &stream) {
		stream >> &ip_;
		stream >> &port_;
	}
};

class PK_C_REQ_PLAYER_MOVE :public Packet
{
public:
	PacketType type() { return E_C_REQ_PLAYER_MOVE; }
	UInt64	  Room_num;
	float     x;
	float     y;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << Room_num;
		stream << x;
		stream << y;
	}

	void decode(Stream &stream) {
		stream >> &Room_num;
		stream >> &x;
		stream >> &y;
	}
};

class PK_I_USER_INFO :public Packet
{
public:
	PacketType type() { return E_I_USER_INFO; }
	UInt64     room_Num;
	UInt64     User1_ID;
	UInt64     User2_ID;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_Num;
		stream << User1_ID;
		stream << User2_ID;
	}

	void decode(Stream &stream) {
		stream >> &room_Num;
		stream >> &User1_ID;
		stream >> &User2_ID;
	}
};

class PK_S_ANS_PLAYER_MOVE :public Packet
{
public:
	PacketType type() { return E_S_ANS_PLAYER_MOVE; }
	UInt64	  room_num;
	float     x;
	float     y;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << x;
		stream << y;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &x;
		stream >> &y;
	}
};

class PK_C_REQ_FIRE_BULLET :public Packet
{
public:
	PacketType type() { return E_C_REQ_FIRE_BULLET; }
	UInt64	  room_num;
	float     x;
	float     y;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << x;
		stream << y;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &x;
		stream >> &y;
	}
};

class PK_S_ANS_FIRE_BULLET :public Packet
{
public:
	PacketType type() { return E_S_ANS_FIRE_BULLET; }
	UInt64	  room_num;
	float     x;
	float     y;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << x;
		stream << y;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &x;
		stream >> &y;
	}
};

class PK_C_REQ_MAKE_ENEMY :public Packet
{
public:
	PacketType type() { return E_C_REQ_MAKE_ENEMY; }
	UInt64	  room_num;
	float     x;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << x;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &x;
	}
};

class PK_S_ANS_MAKE_ENEMY :public Packet
{
public:
	PacketType type() { return E_S_ANS_MAKE_ENEMY; }
	float     x;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << x;
	}

	void decode(Stream &stream) {
		stream >> &x;
	}
};

class PK_C_REQ_ENEMY_EXPLOSION :public Packet
{
public:
	PacketType type() { return E_C_REQ_ENEMY_EXPLOSION; }
	UInt64 room_num;
	UInt64 enemycount;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << enemycount;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &enemycount;
	}
};

class PK_S_ANS_ENEMY_EXPLOSION :public Packet
{
public:
	PacketType type() { return E_S_ANS_ENEMY_EXPLOSION; }
	UInt64 room_num;
	UInt64 enemycount;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << enemycount;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &enemycount;
	}
};

class PK_C_REQ_MAKE_BOSS :public Packet
{
public:
	PacketType type() { return E_C_REQ_MAKE_BOSS; }
	UInt64 room_num;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
	}
};

class PK_S_ANS_MAKE_BOSS :public Packet
{
public:
	PacketType type() { return E_S_ANS_MAKE_BOSS; }
	UInt64 room_num;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
	}
};

class PK_C_REQ_BOSS_EXPLOSION :public Packet
{
public:
	PacketType type() { return E_C_REQ_BOSS_EXPLOSION; }
	UInt64 room_num;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
	}
};

class PK_S_ANS_BOSS_EXPLOSION :public Packet
{
public:
	PacketType type() { return E_S_ANS_BOSS_EXPLOSION; }
	UInt64 room_num;
	std::string     ip_;
	UInt32     port_;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << ip_;
		stream << port_;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &ip_;
		stream >> &port_;
	}
};

class PK_I_INITIALIZE_ROOMMEMBER :public Packet
{
public:
	PacketType type() { return E_I_INITIALIZE_ROOMMEMBER; }
	UInt64 room_num;
	UInt64 member_count;

	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
		stream << member_count;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
		stream >> &member_count;
	}
};

class PK_S_REQ_CHANGE_SCENE :public Packet
{
public:
	PacketType type() { return E_S_REQ_CHANGE_SCENE; }
	UInt64 room_num;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
	}
};

class PK_S_REQ_ON_GAME :public Packet
{
public:
	PacketType type() { return E_S_REQ_ON_GAME; }
	UInt64 room_num;


	void encode(Stream &stream) {
		stream << (Int64)this->type();
		stream << room_num;
	}

	void decode(Stream &stream) {
		stream >> &room_num;
	}
};