#pragma once

struct NetworkHeader
{
	short len;
};

// Server Info
#define SERVER_IP L"0.0.0.0"
#define SERVER_PORT 11098

#define SESSION_MAX_CNT 5000
#define SEND_PACKET_MAX_CNT 30000

//-----------------------------------------------------------------
// 30�� �̻��� �ǵ��� �ƹ��� �޽��� ���ŵ� ���°�� ���� ����.
//-----------------------------------------------------------------
#define NETWORK_PACKET_RECV_TIMEOUT	30000