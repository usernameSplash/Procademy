#pragma once

struct NetworkHeader
{
	short len;
};

// Server Info
#define SERVER_IP L"0.0.0.0"
#define SERVER_PORT 11098

//-----------------------------------------------------------------
// 30초 이상이 되도록 아무런 메시지 수신도 없는경우 접속 끊음.
//-----------------------------------------------------------------
#define NETWORK_PACKET_RECV_TIMEOUT	30000