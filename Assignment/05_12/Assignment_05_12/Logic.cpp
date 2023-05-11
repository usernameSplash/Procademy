
#include "Network.h"
#include "Logic.h"
#include "Player.h"
#include "Packet.h"

//using namespace std;

chrono::high_resolution_clock::duration g_DeltaTime;

void ReadRecvBufferProc(void)
{
	PacketHeader header;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;
		if (player.recvBuffer.Size() == 0)
		{
			continue;
		}

		while (TRUE)
		{
			//size_t peekSize; //useless

			if (player.recvBuffer.Size() < sizeof(header))
			{
				break;
			}

			player.recvBuffer.Peek((char*)&header, sizeof(header));
			
			if (player.recvBuffer.Size() < (sizeof(header) + header.size))
			{
				break;
			}

			player.recvBuffer.Dequeue(sizeof(header));

			switch ((ePacketType)header.type)
			{
			case ePacketType::PACKET_CS_MOVE_START:
				{
					PacketCSMoveStart pMS;
					
					player.recvBuffer.Peek((char*)&pMS, sizeof(pMS)); // Assure <peeksize> is equal to <header.size>
					player.recvBuffer.Dequeue(sizeof(pMS));

					if (abs(player.x - pMS.x) > RANGE_MOVE_ERROR || abs(player.y - pMS.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.bMoving = TRUE;
					player.dir = pMS.dir;
					player.x = pMS.x;
					player.y = pMS.y;

					break;
				}
			case ePacketType::PACKET_CS_MOVE_STOP:
				{
					PacketCSMoveStop pMS;

					player.recvBuffer.Peek((char*)&pMS, sizeof(pMS));
					player.recvBuffer.Dequeue(sizeof(pMS));

					if (abs(player.x - pMS.x) > RANGE_MOVE_ERROR || abs(player.y - pMS.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.bMoving = FALSE;
					player.dir = pMS.dir;
					player.x = pMS.x;
					player.y = pMS.y;

					break;
				}
			case ePacketType::PACKET_CS_ATTACK_1:
				{
					PacketCSAttack1 pATK1;

					player.recvBuffer.Peek((char*)&pATK1, sizeof(pATK1));
					player.recvBuffer.Dequeue(sizeof(pATK1));

					if (abs(player.x - pATK1.x) > RANGE_MOVE_ERROR || abs(player.y - pATK1.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.dir = pATK1.dir;
					player.x = pATK1.x;
					player.y = pATK1.y;

					// TODO: Attack Proccess;

					break;
				}
			case ePacketType::PACKET_CS_ATTACK_2:
				{
					PacketCSAttack2 pATK2;

					player.recvBuffer.Peek((char*)&pATK2, sizeof(pATK2));
					player.recvBuffer.Dequeue(sizeof(pATK2));

					if (abs(player.x - pATK2.x) > RANGE_MOVE_ERROR || abs(player.y - pATK2.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.dir = pATK2.dir;
					player.x = pATK2.x;
					player.y = pATK2.y;

					// TODO: Attack Proccess;

					break;
				}
			case ePacketType::PACKET_CS_ATTACK_3:
				{
					PacketCSAttack2 pATK3;

					player.recvBuffer.Peek((char*)&pATK3, sizeof(pATK3));
					player.recvBuffer.Dequeue(sizeof(pATK3));

					if (abs(player.x - pATK3.x) > RANGE_MOVE_ERROR || abs(player.y - pATK3.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.dir = pATK3.dir;
					player.x = pATK3.x;
					player.y = pATK3.y;

					// TODO: Attack Proccess;

					break;
				}
			case ePacketType::PACKET_CS_SYNC:
				break;
			default:
				break;
			}
		}
	}
}

void LogicProc(void)
{
	if (g_DeltaTime < MAX_FRAME_TIME)
	{
		return;
	}

	// TODO...

	/*
		Move processing
	*/
	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;

		if (!player.bMoving)
		{
			continue;
		}

		// Move Each Player By Its Direction
		switch ((eMoveDir)player.dir)
		{
		case eMoveDir::LL:
			{
				break;
			}
		case eMoveDir::LU:
			{
				break;
			}
		case eMoveDir::UU:
			{
				break;
			}
		case eMoveDir::RU:
			{
				break;
			}
		case eMoveDir::RR:
			{
				break;
			}
		case eMoveDir::RD:
			{
				break;
			}
		case eMoveDir::DD:
			{
				break;
			}
		case eMoveDir::LD:
			{
				break;
			}
		default:
			break;
		}
	}

	g_DeltaTime = 0ns;
}
