#pragma once

#include <profileapi.h>
#include <chrono>

using namespace std;

constexpr chrono::high_resolution_clock::duration MAX_FRAME_TIME = 5000000ns; // Time per Frame in 50 FPS;
extern chrono::high_resolution_clock::duration g_DeltaTime;

void PacketProc(void);

void PacketMoveStartProc(Player* pPlayer, char* pPacket);
void PacketMoveStopProc(Player* pPlayer, char* pPacket);
void PacketAttack1Proc(Player* pPlayer, char* pPacket);
void PacketAttack2Proc(Player* pPlayer, char* pPacket);
void PacketAttack3Proc(Player* pPlayer, char* pPacket);

void LogicProc(void);
