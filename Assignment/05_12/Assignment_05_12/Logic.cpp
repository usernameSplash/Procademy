
#include "Network.h"
#include "Logic.h"
#include "Player.h"
#include "Packet.h"

//using namespace std;

chrono::high_resolution_clock::duration g_DeltaTime;

void PacketProc(void)
{
	PacketHeader header;

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;
		if (player.recvBuffer.Size() == 0)
		{
			continue;
		}

		while (true)
		{
			char buf[512];

			if (player.recvBuffer.Size() < sizeof(header))
			{
				break;
			}

			player.recvBuffer.Peek((char*)&header, sizeof(header));

			if (header.code != 0x89)
			{
				wprintf(L"Client(%d) Sent Wrong Packet Header Code\n", it->first);
				DeleteUser(it->first);
				break;
			}
			
			if (player.recvBuffer.Size() < (sizeof(header) + header.size))
			{
				break;
			}

			player.recvBuffer.Dequeue(sizeof(header));
			player.recvBuffer.Peek(buf, header.size); // Assure <peeksize> is equal to <header.size>
			player.recvBuffer.Dequeue(header.size);

			// TODO : make functions process each packet
			switch ((ePacketType)header.type)
			{
			case ePacketType::PACKET_CS_MOVE_START:
				{
					PacketMoveStartProc(&player, buf);
					wprintf(L"# <Packet Received> - [MOVE_START] : Client(%d) / Dir : %d / X : %d / Y : %d\n", player.id, player.dir, player.x, player.y);
					break;
				}
			case ePacketType::PACKET_CS_MOVE_STOP:
				{
					PacketMoveStopProc(&player, buf);
					wprintf(L"# <Packet Received> - [MOVE_STOP] : Client(%d) / Dir : %d / X : %d / Y : %d\n", player.id, player.dir, player.x, player.y);
					break;
				}
			case ePacketType::PACKET_CS_ATTACK_1:
				{
					PacketAttack1Proc(&player, buf);
					wprintf(L"# <Packet Received> - [ATTACK_1] : Client(%d) / Dir : %d / X : %d / Y : %d\n", player.id, player.dir, player.x, player.y);
					break;
				}
			case ePacketType::PACKET_CS_ATTACK_2:
				{
					PacketAttack2Proc(&player, buf);
					wprintf(L"# <Packet Received> - [ATTACK_2] : Client(%d) / Dir : %d / X : %d / Y : %d\n", player.id, player.dir, player.x, player.y);
					break;
				}
			case ePacketType::PACKET_CS_ATTACK_3:
				{
					PacketAttack3Proc(&player, buf);
					wprintf(L"# <Packet Received> - [ATTACK_3] : Client(%d) / Dir : %d / X : %d / Y : %d\n", player.id, player.dir, player.x, player.y);
					break;
				}
			case ePacketType::PACKET_CS_SYNC:
				{
					break;
				}
			default:
				break;
			}
		}
	}
}

void PacketMoveStartProc(Player* pPlayer, char* pPacket)
{
	PacketHeader pSCMSHeader;
	PacketSCMoveStart pSCMS;
	PacketCSMoveStart* pPCSMS;

	pPCSMS = (PacketCSMoveStart*)pPacket;

	if (abs(pPlayer->x - pPCSMS->x) > RANGE_MOVE_ERROR || abs(pPlayer->y - pPCSMS->y) > RANGE_MOVE_ERROR)
	{
		return;
	}

	pPlayer->status = ePlayerStatus::MOVE;

	switch ((eMoveDir)pPCSMS->dir)
	{
	case eMoveDir::RU:
	case eMoveDir::RR:
	case eMoveDir::RD:
		{
			pPlayer->dir = (BYTE)eMoveDir::RR;
			break;
		}
	case eMoveDir::LU:
	case eMoveDir::LL:
	case eMoveDir::LD:
		{
			pPlayer->dir = (BYTE)eMoveDir::LL;
			break;
		}
	default:
		break;
	}

	pPlayer->x = pPCSMS->x;
	pPlayer->y = pPCSMS->y;

	CreatePacketMoveStart(&pSCMSHeader, &pSCMS, pPlayer->id, pPlayer->dir, pPlayer->x, pPlayer->y);

	SendBroadcast(&pPlayer->id, 1, sizeof(pSCMSHeader), (char*)&pSCMSHeader);
	SendBroadcast(&pPlayer->id, 1, sizeof(pSCMS), (char*)&pSCMS);
	
	return;
}

void PacketMoveStopProc(Player* pPlayer, char* pPacket)
{
	PacketHeader pSCMSHeader;
	PacketSCMoveStop pSCMS;
	PacketCSMoveStop* pPCSMS;

	pPCSMS = (PacketCSMoveStop*)pPacket;

	if (abs(pPlayer->x - pPCSMS->x) > RANGE_MOVE_ERROR || abs(pPlayer->y - pPCSMS->y) > RANGE_MOVE_ERROR)
	{
		return;
	}

	pPlayer->status = ePlayerStatus::IDLE;

	switch ((eMoveDir)pPCSMS->dir)
	{
	case eMoveDir::RU:
	case eMoveDir::RR:
	case eMoveDir::RD:
		{
			pPlayer->dir = (BYTE)eMoveDir::RR;
			break;
		}
	case eMoveDir::LU:
	case eMoveDir::LL:
	case eMoveDir::LD:
		{
			pPlayer->dir = (BYTE)eMoveDir::LL;
			break;
		}
	default:
		break;
	}

	pPlayer->x = pPCSMS->x;
	pPlayer->y = pPCSMS->y;

	CreatePacketMoveStop(&pSCMSHeader, &pSCMS, pPlayer->id, pPlayer->dir, pPlayer->x, pPlayer->y);

	SendBroadcast(&pPlayer->id, 1, sizeof(pSCMSHeader), (char*)&pSCMSHeader);
	SendBroadcast(&pPlayer->id, 1, sizeof(pSCMS), (char*)&pSCMS);

	return;
}

void PacketAttack1Proc(Player* pPlayer, char* pPacket)
{
	{
		PacketHeader pSCAtkHeader;
		PacketSCAttack1 pSCAtk;
		PacketCSAttack1* pPCSAtk;

		PacketHeader pDmgHeader;
		PacketSCDamage pDmg;

		pPCSAtk = (PacketCSAttack1*)pPacket;

		if (abs(pPlayer->x - pPCSAtk->x) > RANGE_MOVE_ERROR || abs(pPlayer->y - pPCSAtk->y) > RANGE_MOVE_ERROR)
		{
			return;
		}

		pPlayer->status = ePlayerStatus::ATTACK;

		switch ((eMoveDir)pPCSAtk->dir)
		{
		case eMoveDir::RU:
		case eMoveDir::RR:
		case eMoveDir::RD:
			{
				pPlayer->dir = (BYTE)eMoveDir::RR;
				break;
			}
		case eMoveDir::LU:
		case eMoveDir::LL:
		case eMoveDir::LD:
			{
				pPlayer->dir = (BYTE)eMoveDir::LL;
				break;
			}
		default:
			break;
		}

		pPlayer->x = pPCSAtk->x;
		pPlayer->y = pPCSAtk->y;

		CreatePacketAttack1(&pSCAtkHeader, &pSCAtk, pPlayer->id, pPlayer->dir, pPlayer->x, pPlayer->y);

		SendBroadcast(&pPlayer->id, 1, sizeof(pSCAtkHeader), (char*)&pSCAtkHeader);
		SendBroadcast(&pPlayer->id, 1, sizeof(pSCAtk), (char*)&pSCAtk);

		for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
		{
			if (it->second.status == ePlayerStatus::DEAD)
			{
				continue;
			}

			if (abs(pPlayer->x - it->second.x) > RANGE_ATTACK_2_X || abs(pPlayer->y - it->second.y) > RANGE_ATTACK_2_Y)
			{
				continue;
			}

			it->second.hp -= 1;

			CreatePacketDamage(&pDmgHeader, &pDmg, pPlayer->id, it->second.id, it->second.hp);

			SendBroadcast(NULL, 0, sizeof(pDmgHeader), (char*)&pDmgHeader);
			SendBroadcast(NULL, 0, sizeof(pDmg), (char*)&pDmg);
		}

		return;
	}
}

void PacketAttack2Proc(Player* pPlayer, char* pPacket)
{
	PacketHeader pSCAtkHeader;
	PacketSCAttack2 pSCAtk;
	PacketCSAttack2* pPCSAtk;

	PacketHeader pDmgHeader;
	PacketSCDamage pDmg;

	pPCSAtk = (PacketCSAttack2*)pPacket;

	if (abs(pPlayer->x - pPCSAtk->x) > RANGE_MOVE_ERROR || abs(pPlayer->y - pPCSAtk->y) > RANGE_MOVE_ERROR)
	{
		return;
	}

	pPlayer->status = ePlayerStatus::ATTACK;

	switch ((eMoveDir)pPCSAtk->dir)
	{
	case eMoveDir::RU:
	case eMoveDir::RR:
	case eMoveDir::RD:
		{
			pPlayer->dir = (BYTE)eMoveDir::RR;
			break;
		}
	case eMoveDir::LU:
	case eMoveDir::LL:
	case eMoveDir::LD:
		{
			pPlayer->dir = (BYTE)eMoveDir::LL;
			break;
		}
	default:
		break;
	}

	pPlayer->x = pPCSAtk->x;
	pPlayer->y = pPCSAtk->y;

	CreatePacketAttack2(&pSCAtkHeader, &pSCAtk, pPlayer->id, pPlayer->dir, pPlayer->x, pPlayer->y);

	SendBroadcast(&pPlayer->id, 1, sizeof(pSCAtkHeader), (char*)&pSCAtkHeader);
	SendBroadcast(&pPlayer->id, 1, sizeof(pSCAtk), (char*)&pSCAtk);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->second.status == ePlayerStatus::DEAD)
		{
			continue;
		}

		if (abs(pPlayer->x - it->second.x) > RANGE_ATTACK_2_X || abs(pPlayer->y - it->second.y) > RANGE_ATTACK_2_Y)
		{
			continue;
		}

		if (it->second.hp < 2)
		{
			it->second.hp = 0;
		}
		else
		{
			it->second.hp -= 2;
		}

		CreatePacketDamage(&pDmgHeader, &pDmg, pPlayer->id, it->second.id, it->second.hp);

		SendBroadcast(NULL, 0, sizeof(pDmgHeader), (char*)&pDmgHeader);
		SendBroadcast(NULL, 0, sizeof(pDmg), (char*)&pDmg);
	}

	return;
}

void PacketAttack3Proc(Player* pPlayer, char* pPacket)
{
	PacketHeader pSCAtkHeader;
	PacketSCAttack3 pSCAtk;
	PacketCSAttack3* pPCSAtk;

	PacketHeader pDmgHeader;
	PacketSCDamage pDmg;

	pPCSAtk = (PacketCSAttack3*)pPacket;

	if (abs(pPlayer->x - pPCSAtk->x) > RANGE_MOVE_ERROR || abs(pPlayer->y - pPCSAtk->y) > RANGE_MOVE_ERROR)
	{
		return;
	}

	pPlayer->status = ePlayerStatus::ATTACK;

	switch ((eMoveDir)pPCSAtk->dir)
	{
	case eMoveDir::RU:
	case eMoveDir::RR:
	case eMoveDir::RD:
		{
			pPlayer->dir = (BYTE)eMoveDir::RR;
			break;
		}
	case eMoveDir::LU:
	case eMoveDir::LL:
	case eMoveDir::LD:
		{
			pPlayer->dir = (BYTE)eMoveDir::LL;
			break;
		}
	default:
		break;
	}

	pPlayer->x = pPCSAtk->x;
	pPlayer->y = pPCSAtk->y;

	CreatePacketAttack3(&pSCAtkHeader, &pSCAtk, pPlayer->id, pPlayer->dir, pPlayer->x, pPlayer->y);

	SendBroadcast(&pPlayer->id, 1, sizeof(pSCAtkHeader), (char*)&pSCAtkHeader);
	SendBroadcast(&pPlayer->id, 1, sizeof(pSCAtk), (char*)&pSCAtk);

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		if (it->second.status == ePlayerStatus::DEAD)
		{
			continue;
		}

		if (abs(pPlayer->x - it->second.x) > RANGE_ATTACK_3_X || abs(pPlayer->y - it->second.y) > RANGE_ATTACK_3_Y)
		{
			continue;
		}

		if (it->second.hp < 3)
		{
			it->second.hp = 0;
		}
		else
		{
			it->second.hp -= 3;
		}

		CreatePacketDamage(&pDmgHeader, &pDmg, pPlayer->id, it->second.id, it->second.hp);

		SendBroadcast(NULL, 0, sizeof(pDmgHeader), (char*)&pDmgHeader);
		SendBroadcast(NULL, 0, sizeof(pDmg), (char*)&pDmg);
	}

	return;
}

void LogicProc(void)
{
	if (g_DeltaTime < MAX_FRAME_TIME)
	{
		return;
	}

	for (auto it = g_PlayerList.begin(); it != g_PlayerList.end(); ++it)
	{
		Player& player = it->second;

		if (player.status == ePlayerStatus::DEAD)
		{
			continue;
		}

		if (player.hp == 0)
		{
			DeleteUser(it->first);
			continue;
		}

		if (player.status == ePlayerStatus::MOVE)
		{
			// Move Each Player By Its Direction
			switch ((eMoveDir)player.dir)
			{
			case eMoveDir::LL:
				{
					if (player.x > RANGE_MOVE_LEFT)
					{
						player.x -= 3;

						if (player.x < RANGE_MOVE_LEFT)
						{
							player.x = RANGE_MOVE_LEFT;
						}

						wprintf(L"[Move] - Client(%d) / DIR : LL / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::LU:
				{
					if (player.x > RANGE_MOVE_LEFT && player.y > RANGE_MOVE_TOP)
					{
						player.x -= 3;
						player.y -= 2;

						if (player.x < RANGE_MOVE_LEFT)
						{
							player.x = RANGE_MOVE_LEFT;
						}

						if (player.y < RANGE_MOVE_TOP)
						{
							player.y = RANGE_MOVE_TOP;
						}

						wprintf(L"[Move] - Client(%d) / DIR : LU / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::UU:
				{
					if (player.y > RANGE_MOVE_TOP)
					{
						player.y -= 2;

						if (player.y < RANGE_MOVE_TOP)
						{
							player.y = RANGE_MOVE_TOP;
						}

						wprintf(L"[Move] - Client(%d) / DIR : UU / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::RU:
				{
					if (player.x < RANGE_MOVE_RIGHT && player.y > RANGE_MOVE_TOP)
					{
						player.x += 3;
						player.y -= 2;

						if (player.x > RANGE_MOVE_RIGHT)
						{
							player.x = RANGE_MOVE_RIGHT;
						}

						if (player.y < RANGE_MOVE_TOP)
						{
							player.y = RANGE_MOVE_TOP;
						}

						wprintf(L"[Move] - Client(%d) / DIR : RU / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::RR:
				{
					if (player.x < RANGE_MOVE_RIGHT)
					{
						player.x += 3;

						if (player.x > RANGE_MOVE_RIGHT)
						{
							player.x = RANGE_MOVE_RIGHT;
						}

						wprintf(L"[Move] - Client(%d) / DIR : RR / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::RD:
				{
					if (player.x < RANGE_MOVE_RIGHT && player.y < RANGE_MOVE_BOTTOM)
					{
						player.x += 3;
						player.y += 2;

						if (player.x > RANGE_MOVE_RIGHT)
						{
							player.x = RANGE_MOVE_RIGHT;
						}

						if (player.y > RANGE_MOVE_BOTTOM)
						{
							player.y = RANGE_MOVE_BOTTOM;
						}

						wprintf(L"[Move] - Client(%d) / DIR : RD / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::DD:
				{
					if (player.y < RANGE_MOVE_BOTTOM)
					{
						player.y += 2;

						if (player.y > RANGE_MOVE_BOTTOM)
						{
							player.y = RANGE_MOVE_BOTTOM;
						}

						wprintf(L"[Move] - Client(%d) / DIR : DD / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			case eMoveDir::LD:
				{
					if (player.x > RANGE_MOVE_LEFT && player.y < RANGE_MOVE_BOTTOM)
					{
						player.x -= 3;
						player.y += 2;

						if (player.x < RANGE_MOVE_LEFT)
						{
							player.x = RANGE_MOVE_LEFT;
						}

						if (player.y > RANGE_MOVE_BOTTOM)
						{
							player.y = RANGE_MOVE_BOTTOM;
						}

						wprintf(L"[Move] - Client(%d) / DIR : LD / X : %d / Y : %d\n", player.id, player.x, player.y);
					}

					break;
				}
			default:
				break;
			}
		}
	}

	g_DeltaTime = 0ns;
}
