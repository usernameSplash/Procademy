#include "Logic.h"

#include <Windows.h>
#include <conio.h>
#include <utility>
#include <timeapi.h>


#include "Packet.h"
#include "Renderer.h"

std::unordered_map<PlayerID_t, Player_t> g_PlayerList;
static PlayerID_t myID;

#define MOVE_COOLTIME 50

void ProcessPacket(void* packet)
{
	const ePacketType* type = static_cast<ePacketType*>(packet);

	switch (*type)
	{
	case ePacketType::ASSIGN_ID:
		{
			PacketAssignID_t* packetAssignId = static_cast<PacketAssignID_t*>(packet);
			myID = packetAssignId->id;
			break;
		}
	case ePacketType::CREATE_STAR:
		{
			PacketCreateStar_t* packetCreateStar = static_cast<PacketCreateStar_t*>(packet);

			Player_t player;
			player.id = packetCreateStar->id;
			player.x = packetCreateStar->xCoord;
			player.y = packetCreateStar->yCoord;

			g_PlayerList.insert(std::make_pair(player.id, player));
			break;
		}
	case ePacketType::DELETE_STAR:
		{
			PacketDeleteStar_t* packetDeleteStar = static_cast<PacketDeleteStar_t*>(packet);

			PlayerID_t id = packetDeleteStar->id;
			g_PlayerList.erase(id);
			break;
		}
	case ePacketType::MOVE_STAR:
		{
			PacketMoveStar_t* packetMoveStar = static_cast<PacketMoveStar_t*>(packet);

			auto it = g_PlayerList.find(packetMoveStar->id);
			
			if (it != g_PlayerList.end())
			{
				Player_t* pPlayer = &(it->second);
				pPlayer->x = packetMoveStar->xCoord;
				pPlayer->y = packetMoveStar->yCoord;
			}

			break;
		}
	default:
		break;
	}
}

void RenderObjects(void)
{
	BufferClear();

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); it++)
	{
		SpriteDraw(it->second.x, it->second.y, L'*');
	}

	BufferFlip();
}

bool Update(void* out_packet)
{
	static DWORD movedTime;
	static bool bCanMove = true;

	auto meIterator = g_PlayerList.find(myID);
	Player_t* me;
	bool bMoved = false;
	
	if (meIterator == g_PlayerList.end())
	{
		return false;
	}

	me = &(meIterator->second);

	INT32 x = me->x;
	INT32 y = me->y;

	PacketMoveStar_t* packetMoveStar = static_cast<PacketMoveStar_t*>(out_packet);

	if (bCanMove == true)
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x8001)
		{
			if (x > 0)
			{
				x--;
				bMoved = true;
			}
		}

		if (GetAsyncKeyState(VK_UP) & 0x8001)
		{
			if (y > 0)
			{
				y--;
				bMoved = true;
			}
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8001)
		{
			if (x < (SCREEN_WIDTH - 2))
			{
				x++;
				bMoved = true;
			}
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8001)
		{
			if (y < (SCREEN_HEIGHT - 1))
			{
				y++;
				bMoved = true;
			}
		}
	}
	else
	{
		DWORD curTime = timeGetTime();
		DWORD deltaTime;
		if (movedTime > curTime)
		{
			deltaTime = MAXDWORD - movedTime + curTime;
		}
		else
		{
			deltaTime = curTime - movedTime;
		}

		if (deltaTime >= MOVE_COOLTIME)
		{
			bCanMove = true;
		}
	}

	if (bMoved)
	{
		movedTime = timeGetTime();
		bCanMove = false;

		me->x = x;
		me->y = y;

		packetMoveStar->type = ePacketType::MOVE_STAR;
		packetMoveStar->id = myID;
		packetMoveStar->xCoord = x;
		packetMoveStar->yCoord = y;
	}

	return bMoved;
}