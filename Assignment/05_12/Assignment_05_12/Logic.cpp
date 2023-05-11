
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
					PacketHeader pSCMSHeader;
					PacketCSMoveStart pCSMS;
					PacketSCMoveStart pSCMS;
					
					player.recvBuffer.Peek((char*)&pCSMS, sizeof(pCSMS)); // Assure <peeksize> is equal to <header.size>
					player.recvBuffer.Dequeue(sizeof(pCSMS));

					if (abs(player.x - pCSMS.x) > RANGE_MOVE_ERROR || abs(player.y - pCSMS.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.bMoving = TRUE;
					player.dir = pCSMS.dir;
					player.x = pCSMS.x;
					player.y = pCSMS.y;

					pSCMSHeader.code = 0x89;
					pSCMSHeader.size = sizeof(pSCMS);
					pSCMSHeader.type = (BYTE)ePacketType::PACKET_SC_MOVE_START;

					pSCMS.id = it->first;
					pSCMS.dir = pCSMS.dir;
					pSCMS.x = pCSMS.x;
					pSCMS.y = pCSMS.y;

					SendBroadcast(&player.id, 1, sizeof(pSCMSHeader), (char*)&pSCMSHeader);
					SendBroadcast(&player.id, 1, sizeof(pSCMS), (char*)&pSCMS);

					break;
				}
			case ePacketType::PACKET_CS_MOVE_STOP:
				{
					PacketHeader pSCMSHeader;
					PacketCSMoveStop pCSMS;
					PacketSCMoveStop pSCMS;

					player.recvBuffer.Peek((char*)&pCSMS, sizeof(pCSMS));
					player.recvBuffer.Dequeue(sizeof(pCSMS));

					if (abs(player.x - pCSMS.x) > RANGE_MOVE_ERROR || abs(player.y - pCSMS.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.bMoving = FALSE;
					player.dir = pCSMS.dir;
					player.x = pCSMS.x;
					player.y = pCSMS.y;

					pSCMSHeader.code = 0x89;
					pSCMSHeader.size = sizeof(pSCMS);
					pSCMSHeader.type = (BYTE)ePacketType::PACKET_SC_MOVE_STOP;

					pSCMS.id = it->first;
					pSCMS.dir = pCSMS.dir;
					pSCMS.x = pCSMS.x;
					pSCMS.y = pCSMS.y;

					SendBroadcast(&player.id, 1, sizeof(pSCMSHeader), (char*)&pSCMSHeader);
					SendBroadcast(&player.id, 1, sizeof(pSCMS), (char*)&pSCMS);

					break;
				}
			case ePacketType::PACKET_CS_ATTACK_1:
				{
					PacketHeader pSCATK1Header;
					PacketHeader pDMGHeader;
					PacketCSAttack1 pCSATK1;
					PacketSCAttack1 pSCATK1;
					PacketSCDamage pDMG;

					player.recvBuffer.Peek((char*)&pCSATK1, sizeof(pCSATK1));
					player.recvBuffer.Dequeue(sizeof(pCSATK1));

					if (abs(player.x - pCSATK1.x) > RANGE_MOVE_ERROR || abs(player.y - pCSATK1.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.dir = pCSATK1.dir;
					player.x = pCSATK1.x;
					player.y = pCSATK1.y;

					pSCATK1Header.code = 0x89;
					pSCATK1Header.size = sizeof(pSCATK1);
					pSCATK1Header.type = (BYTE)ePacketType::PACKET_SC_ATTACK_1;

					pDMGHeader.code = 0x89;
					pDMGHeader.size = sizeof(pDMG);
					pDMGHeader.type = (BYTE)ePacketType::PACKET_SC_DAMAGE;

					for (auto opponentIt = g_PlayerList.begin(); opponentIt != g_PlayerList.end(); ++opponentIt)
					{
						Player& opponentPlayer = opponentIt->second;

						if (opponentIt->first == it->first)
						{
							continue;
						}

						if (abs(player.x - opponentPlayer.x) > RANGE_ATTACK_1_X || abs(player.y - opponentPlayer.y) > RANGE_ATTACK_1_Y)
						{
							continue;
						}

						opponentPlayer.hp -= 1;

						pSCATK1.id = it->first;
						pSCATK1.dir = player.dir;
						pSCATK1.x = player.x;
						pSCATK1.y = player.y;

						pDMG.attackId = it->first;
						pDMG.damagedId = opponentIt->first;
						pDMG.damagedHp = opponentPlayer.hp;

						SendBroadcast(&player.id, 1, sizeof(pSCATK1Header), (char*)&pSCATK1Header);
						SendBroadcast(&player.id, 1, sizeof(pSCATK1), (char*)&pSCATK1);

						SendBroadcast(NULL, 0, sizeof(pDMGHeader), (char*)&pDMGHeader);
						SendBroadcast(NULL, 0, sizeof(pDMG), (char*)&pDMG);

						if (opponentPlayer.hp == 0)
						{
							DeleteUser(opponentIt->first);
						}
					}

					break;
				}
			case ePacketType::PACKET_CS_ATTACK_2:
				{
					PacketHeader pSCATK2Header;
					PacketHeader pDMGHeader;
					PacketCSAttack1 pCSATK2;
					PacketSCAttack1 pSCATK2;
					PacketSCDamage pDMG;

					player.recvBuffer.Peek((char*)&pCSATK2, sizeof(pCSATK2));
					player.recvBuffer.Dequeue(sizeof(pCSATK2));

					if (abs(player.x - pCSATK2.x) > RANGE_MOVE_ERROR || abs(player.y - pCSATK2.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.dir = pCSATK2.dir;
					player.x = pCSATK2.x;
					player.y = pCSATK2.y;

					pSCATK2Header.code = 0x89;
					pSCATK2Header.size = sizeof(pSCATK2);
					pSCATK2Header.type = (BYTE)ePacketType::PACKET_SC_ATTACK_2;

					pDMGHeader.code = 0x89;
					pDMGHeader.size = sizeof(pDMG);
					pDMGHeader.type = (BYTE)ePacketType::PACKET_SC_DAMAGE;

					for (auto opponentIt = g_PlayerList.begin(); opponentIt != g_PlayerList.end(); ++opponentIt)
					{
						Player& opponentPlayer = opponentIt->second;

						if (opponentIt->first == it->first)
						{
							continue;
						}

						if (abs(player.x - opponentPlayer.x) > RANGE_ATTACK_2_X || abs(player.y - opponentPlayer.y) > RANGE_ATTACK_2_Y)
						{
							continue;
						}

						if (opponentPlayer.hp < 2)
						{
							opponentPlayer.hp = 0;
						}
						else
						{
							opponentPlayer.hp -= 2;
						}

						pSCATK2.id = it->first;
						pSCATK2.dir = player.dir;
						pSCATK2.x = player.x;
						pSCATK2.y = player.y;

						pDMG.attackId = it->first;
						pDMG.damagedId = opponentIt->first;
						pDMG.damagedHp = opponentPlayer.hp;

						SendBroadcast(&player.id, 1, sizeof(pSCATK2Header), (char*)&pSCATK2Header);
						SendBroadcast(&player.id, 1, sizeof(pSCATK2), (char*)&pSCATK2);

						SendBroadcast(NULL, 0, sizeof(pDMGHeader), (char*)&pDMGHeader);
						SendBroadcast(NULL, 0, sizeof(pDMG), (char*)&pDMG);

						if (opponentPlayer.hp == 0)
						{
							DeleteUser(opponentIt->first);
						}
					}

					break;
				}
			case ePacketType::PACKET_CS_ATTACK_3:
				{
					PacketHeader pSCATK3Header;
					PacketHeader pDMGHeader;
					PacketCSAttack1 pCSATK3;
					PacketSCAttack1 pSCATK3;
					PacketSCDamage pDMG;

					player.recvBuffer.Peek((char*)&pCSATK3, sizeof(pCSATK3));
					player.recvBuffer.Dequeue(sizeof(pCSATK3));

					if (abs(player.x - pCSATK3.x) > RANGE_MOVE_ERROR || abs(player.y - pCSATK3.y) > RANGE_MOVE_ERROR)
					{
						break;
					}

					player.dir = pCSATK3.dir;
					player.x = pCSATK3.x;
					player.y = pCSATK3.y;

					pSCATK3Header.code = 0x89;
					pSCATK3Header.size = sizeof(pSCATK3);
					pSCATK3Header.type = (BYTE)ePacketType::PACKET_SC_ATTACK_3;

					pDMGHeader.code = 0x89;
					pDMGHeader.size = sizeof(pDMG);
					pDMGHeader.type = (BYTE)ePacketType::PACKET_SC_DAMAGE;

					for (auto opponentIt = g_PlayerList.begin(); opponentIt != g_PlayerList.end(); ++opponentIt)
					{
						Player& opponentPlayer = opponentIt->second;

						if (opponentIt->first == it->first)
						{
							continue;
						}

						if (abs(player.x - opponentPlayer.x) > RANGE_ATTACK_2_X || abs(player.y - opponentPlayer.y) > RANGE_ATTACK_2_Y)
						{
							continue;
						}

						if (opponentPlayer.hp < 3)
						{
							opponentPlayer.hp = 0;
							
							auto result = DeleteUser(opponentIt->first);
							/*
							DeleteUser 시 g_PlayerList에서 제거됨. 관련 문제 해결 필요.
							*/
						}
						else
						{
							opponentPlayer.hp -= 3;
						}

						pSCATK3.id = it->first;
						pSCATK3.dir = player.dir;
						pSCATK3.x = player.x;
						pSCATK3.y = player.y;

						pDMG.attackId = it->first;
						pDMG.damagedId = opponentIt->first;
						pDMG.damagedHp = opponentPlayer.hp;

						SendBroadcast(&player.id, 1, sizeof(pSCATK3Header), (char*)&pSCATK3Header);
						SendBroadcast(&player.id, 1, sizeof(pSCATK3), (char*)&pSCATK3);

						SendBroadcast(NULL, 0, sizeof(pDMGHeader), (char*)&pDMGHeader);
						SendBroadcast(NULL, 0, sizeof(pDMG), (char*)&pDMG);
					}

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
