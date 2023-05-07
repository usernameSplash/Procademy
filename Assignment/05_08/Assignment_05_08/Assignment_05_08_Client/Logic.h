#pragma once

#include <unordered_map>
#include "Player.h"

extern std::unordered_map<PlayerID_t, Player> g_PlayerList;

void ProcessPacket(void* packet);
void RenderObjects(void);
bool Update(void* out_packet);