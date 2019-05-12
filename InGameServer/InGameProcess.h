#pragma once
class InGameProcess : public ContentsProcess
{
private:
	void registSubPacketFunc();

	/// 초기화
	void initialize() override;
	///해제
	void release() override;
	
public:
	InGameProcess();
	~InGameProcess();
//--------------------------패킷 선언--------------------------------
	static void C_REQ_ENEMY_EXPLOSION(Session *session, Packet *rowPacket);
	static void C_REQ_MAKE_ENEMY(Session *session, Packet *rowPacket);
	static void C_REQ_PLAYER_MOVE(Session *session, Packet *rowPacket);
	static void I_USER_INFO(Session *session, Packet *rowPacket);
	static void C_REQ_FIRE_BULLET(Session *session, Packet *rowPacket);
	static void C_REQ_MAKE_BOSS(Session *session, Packet *rowPacket);
	static void C_REQ_BOSS_EXPLOSION(Session *session, Packet *rowPacket);
};