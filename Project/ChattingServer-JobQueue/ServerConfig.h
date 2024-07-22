#pragma once

/*
	Server
*/
constexpr int PACKET_CODE = 0x77;
constexpr int ENCODE_KEY = 0x32;

#define SERVER_NET

#ifndef SERVER_NET
#ifdef SERVER_LAN
#pragma pack (push, 1)
struct PacketHeader
{
	unsigned short _len;
};
#pragma pack(pop)
#endif
#endif

#ifdef SERVER_NET
#pragma pack(push, 1)
struct PacketHeader
{
	unsigned char _code;
	unsigned short _len;
	unsigned char _randKey;
	unsigned char _checkSum;
};
#pragma pack(pop)
#endif

/*
	Session
*/

#define HEADER_SIZE sizeof(PacketHeader)

constexpr int SESSION_MAX = 15000;

constexpr int RECV_MAX = 2;
constexpr int SEND_MAX = 100;
