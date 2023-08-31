#pragma once

struct SPacketHeader
{
	short len;
};

enum class ePacketType
{
	PACKET_SC_CREATE_MY_CHARACTER = 0,
	PACKET_SC_CREATE_OTHER_CHARACTER,
	PACKET_SC_DELETE_CHARACTER,

	PACKET_CS_MOVE_START = 10,
	PACKET_SC_MOVE_START,
	PACKET_CS_MOVE_STOP,
	PACKET_SC_MOVE_STOP,

	PACKET_CS_ATTACK_1 = 20,
	PACKET_SC_ATTACK_1,
	PACKET_CS_ATTACK_2,
	PACKET_SC_ATTACK_2,
	PACKET_CS_ATTACK_3,
	PACKET_SC_ATTACK_3,

	PACKET_SC_DAMAGE = 30,

	PACKET_CS_SYNC = 250,
	PACKET_SC_SYNC,
	PACKET_CS_ECHO,
	PACKET_SC_ECHO,
};


// Server Info
#define SERVER_IP L"0.0.0.0"
#define SERVER_PORT 11098

//-----------------------------------------------------------------
// 30초 이상이 되도록 아무런 메시지 수신도 없는경우 접속 끊음.
//-----------------------------------------------------------------
#define NETWORK_PACKET_RECV_TIMEOUT	30000

//-----------------------------------------------------------------
// 이동 오류체크 범위
//-----------------------------------------------------------------
#define ERROR_RANGE	50

#define SESSION_MAX 7000

#define FRAME_PER_SEC 25
#define MILLISEC_PER_FRAME (1000 / FRAME_PER_SEC)