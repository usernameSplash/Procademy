#pragma once

#include <profileapi.h>
#include <chrono>

using namespace std;

constexpr chrono::high_resolution_clock::duration MAX_FRAME_TIME = 5000000ns; // Time per Frame in 50 FPS;
extern chrono::high_resolution_clock::duration g_DeltaTime;

void LogicProc(void);
