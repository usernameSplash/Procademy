#include "LoadData.h"
#include "Logger.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define STAGE_NAME_LEN_MAX 20

static char(*s_StageFileNames)[STAGE_NAME_LEN_MAX]; //! must be double pointer.


/*
	Load All Stage File Names From <STAGE_INFO_FILE_NAME> File.
*/
void LoadAllStageFileName(void)
{
	FILE* stageFile;
	size_t stageNum;
	
	stageFile = fopen(STAGE_INFO_FILE_NAME, "r");
	assert(stageFile != NULL);

	if (fscanf(stageFile, "%zu\n", &stageNum) == 0)
	{
		PrintLog("Failed Load Stage Data", error_t::ERROR_);
		return;
	}
	
	s_StageFileNames = (char(*)[STAGE_NAME_LEN_MAX])malloc(sizeof(char(*)[STAGE_NAME_LEN_MAX]) * stageNum);
	if (s_StageFileNames == NULL)
	{
		PrintLog("Memory Allocation Error", error_t::ERROR_);
		return;
	}

	for (size_t iCnt = 0; iCnt < stageNum; iCnt++)
	{
		char stageFileName[STAGE_NAME_LEN_MAX];
		fgets(stageFileName, STAGE_NAME_LEN_MAX, stageFile);
		strcpy(s_StageFileNames[iCnt], stageFileName); // There are no errors in this line. C6385 warning is annoying me.
	}

	PrintLog("Load Stage Data Successfully", error_t::NOTE);

	return;
}