
#include "Network.h"
#include "Logic.h"
#include "Player.h"

//using namespace std;

chrono::high_resolution_clock::duration g_DeltaTime;

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
