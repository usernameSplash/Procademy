#pragma once
#include <Windows.h>

typedef struct BitMapFileInfo
{
	FILE* fp;
	BITMAPFILEHEADER* fileHeader;
	BITMAPINFOHEADER* infoHeader;
} BitMapFileInfo_t;

int OpenAndReadBitMapFile(const char* fileName, BitMapFileInfo_t* out_BitMapFile); //open and read bitmap file and NOT rewind file pointer.
void AlphaBlendTwoImages(const char* inputFileName1, const char* inputFileName2, const char* outputFileName);
void GrayScaleTransform(const char* inputFileName, const char* outputFileName);