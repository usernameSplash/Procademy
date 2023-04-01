#include <cassert>
#include <cstdio>
#include <cstring>
#include "DataFileNameLoader.h"


DataFileNameLoader::DataFileNameLoader()
{
	LoadStageDataFilesName();
}

DataFileNameLoader::~DataFileNameLoader()
{
	for (size_t iCnt = 0; iCnt < mStageCount; iCnt++)
	{
		delete mStageFileNames[iCnt];
	}

	delete mStageFileNames;
}

DataFileNameLoader* DataFileNameLoader::GetInstance(void)
{
	static DataFileNameLoader instance;
	return &instance;
}

const char* DataFileNameLoader::GetStageFileName(size_t stageNum) const
{
	if (mStageCount <= stageNum)
	{
		return nullptr;
	}

	return mStageFileNames[stageNum];
}

void DataFileNameLoader::LoadStageDataFilesName(void)
{
	FILE* stageFile;
	int ret;
	
	stageFile = fopen(FILE_PATH(STAGE_INFO_FILE_DIR, STAGE_INFO_FILE_NAME), "w");
	assert(stageFile != NULL);

	ret = fscanf(stageFile, "%zu\n", &mStageCount);
	assert(ret != 0);

	mStageFileNames = new char* [mStageCount];
	assert(mStageFileNames != nullptr);

	for (size_t iCnt = 0; iCnt < mStageCount; iCnt++)
	{
		char stageFileNameBuffer[1024];
		size_t fileNameLen;

		fgets(stageFileNameBuffer, 1024, stageFile);

		fileNameLen = strlen(stageFileNameBuffer);
		mStageFileNames[iCnt] = new char[fileNameLen + 1];

		strncpy(mStageFileNames[iCnt], stageFileNameBuffer, fileNameLen);
	}

	fclose(stageFile);
}