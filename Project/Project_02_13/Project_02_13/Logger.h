#pragma once

typedef enum error
{
	NOTE,
	WARNING,
	ERROR_ /* conflict name problem with winapi macro */
} error_t;

void PrintLog(const char* message, error_t errorType);