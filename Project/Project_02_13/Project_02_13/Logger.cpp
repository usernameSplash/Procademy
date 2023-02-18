#include "Logger.h"

#include <cstdio>
#include <ctime>

void PrintLog(const char* message, error_t errorType)
{
	FILE* logFile = fopen("./log.txt", "a");

	char datetimeString[50];
	char errorTypeString[12];

	time_t timer;
	struct tm* t;
	timer = time(NULL);
	t = localtime(&timer);

	sprintf(datetimeString, "[%04d - %02d - %02d %02d:%02d:%02d] ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	switch (errorType)
	{
	case error_t::NOTE:
		sprintf(errorTypeString, "<NOTE> : ");
		break;
	case error_t::WARNING:
		sprintf(errorTypeString, "<WARNING> : ");
		break;
	case error_t::ERROR_:
		sprintf(errorTypeString, "<ERROR> : ");
		break;
	default:
		break;
	}

	fprintf(logFile, "%s %s %s\n", datetimeString, errorTypeString, message);
	fclose(logFile);

	return;
}