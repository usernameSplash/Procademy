#pragma once

#define FILE_PATH_NX(dir, src) dir ## src
#define FILE_PATH(dir, src) FILE_PATH_NX(dir, src)
#define STAGE_INFO_FILE_DIR "./stage/"
#define STAGE_INFO_FILE_NAME "stage.txt"

class DataFileNameLoader
{
private:
	DataFileNameLoader();
	~DataFileNameLoader();

public:
	static DataFileNameLoader* GetInstance(void);
	const char* GetStageFileName(size_t stageNum) const;
	size_t GetStageCount(void) const;

private:
	void LoadStageDataFilesName(void);

private:
	char** mStageFileNames;
	size_t mStageCount;
};

