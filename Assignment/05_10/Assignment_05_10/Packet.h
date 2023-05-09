#pragma once

typedef struct PacketHeader
{
	short length;
}PacketHeader_t;

typedef struct DrawPacket
{
	int startX;
	int startY;
	int endX;
	int endY;
} DrawPacket_t;