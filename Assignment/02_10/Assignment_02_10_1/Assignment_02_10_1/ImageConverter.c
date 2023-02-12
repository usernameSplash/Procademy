#include <stdio.h>
#include <string.h>
#include "ImageConverter.h"

int OpenAndReadBitMapFile(const char* fileName, BitMapFileInfo_t* out_BitMapFile)
{
	out_BitMapFile->fileHeader = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
	out_BitMapFile->infoHeader = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));

	if (out_BitMapFile->fileHeader == NULL || out_BitMapFile->infoHeader == NULL)
	{
		printf("Memory Allocation Error.\n");
		return -1;
	}

	out_BitMapFile->fp = fopen(fileName, "r");

	if (out_BitMapFile->fp == NULL)
	{
		printf("Wrong File Name.\n");
		return -1;
	}

	fread(out_BitMapFile->fileHeader, 1, sizeof(BITMAPFILEHEADER), out_BitMapFile->fp);

	if (out_BitMapFile->fileHeader->bfType != 0x4d42)
	{
		printf("This File is Not BitMap. Only BitMap Files are Allowed.\n");
		return -1;
	}

	fread(out_BitMapFile->infoHeader, 1, sizeof(BITMAPINFOHEADER), out_BitMapFile->fp);

	return 0;
}

void AlphaBlendTwoImages(const char* inputFileName1, const char* inputFileName2, const char* outputFileName)
{
	BitMapFileInfo_t inputFile1;
	BitMapFileInfo_t inputFile2;
	int* pa_PixelData1;
	int* pa_PixelData2;

	FILE* outputFile;
	
	if (OpenAndReadBitMapFile(inputFileName1, &inputFile1) != 0)
	{
		return;
	}

	if (OpenAndReadBitMapFile(inputFileName2, &inputFile2) != 0)
	{
		return;
	}

	outputFile = fopen(outputFileName, "w");

	//outputFileHeader.bfType = 0x4d42; //little endian of "BM"
	//outputFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (inputFile1.infoHeader->biWidth * inputFile1.infoHeader->biHeight * 4);
	//outputFileHeader.bfReserved1 = 0;
	//outputFileHeader.bfReserved2 = 0;
	//outputFileHeader.bfOffBits = 0;

	//outputInfoHeader.biSize = inputFile1.infoHeader->biSize;
	//outputInfoHeader.biWidth = inputFile1.infoHeader->biWidth;
	//outputInfoHeader.biHeight = inputFile1.infoHeader->biHeight;
	//outputInfoHeader.biPlanes = 1;
	//outputInfoHeader.biBitCount = 32;
	//outputInfoHeader.biCompression = BI_RGB;
	//outputInfoHeader.biSizeImage = inputFile1.infoHeader->biSizeImage;
	//outputInfoHeader.biXPelsPerMeter = inputFile1.infoHeader->biXPelsPerMeter;
	//outputInfoHeader.biYPelsPerMeter = inputFile1.infoHeader->biYPelsPerMeter;;
	//outputInfoHeader.biClrUsed = 0;
	//outputInfoHeader.biClrImportant = 0;

	fwrite(inputFile1.fileHeader, 1, sizeof(BITMAPFILEHEADER), outputFile);
	fwrite(inputFile1.infoHeader, 1, sizeof(BITMAPINFOHEADER), outputFile);

	pa_PixelData1 = (int*)malloc(inputFile1.infoHeader->biSizeImage);
	pa_PixelData2 = (int*)malloc(inputFile2.infoHeader->biSizeImage);

	if (pa_PixelData1 == NULL || pa_PixelData2 == NULL)
	{
		printf("Memory Allocation Error\n");
		return;
	}

	fread(pa_PixelData1, 1, inputFile1.infoHeader->biSizeImage, inputFile1.fp);
	fread(pa_PixelData2, 1, inputFile2.infoHeader->biSizeImage, inputFile2.fp);

	DWORD iCnt;
	for (iCnt = 0; iCnt < inputFile1.infoHeader->biWidth * inputFile1.infoHeader->biHeight; iCnt++)
	{
		int pixel1;
		int pixel2;
		int resultPixel;

		pixel1 = pa_PixelData1[iCnt];
		pixel2 = pa_PixelData2[iCnt];
		
		resultPixel = ((pixel1 / 2) & 0x7f7f7f7f) + ((pixel2 / 2) & 0x7f7f7f7f);

		fwrite(&resultPixel, 1, sizeof(int), outputFile);
	}

	printf("%u Pixels are Read.\n", iCnt);

	fclose(inputFile1.fp);
	fclose(inputFile2.fp);
	fclose(outputFile);

	free(inputFile1.fileHeader);
	free(inputFile1.infoHeader);
	free(inputFile2.fileHeader);
	free(inputFile2.infoHeader);

	return;
}

void GrayScaleTransform(const char* inputFileName, const char* outputFileName)
{
	BitMapFileInfo_t inputFile;
	int* pa_PixelData;

	FILE* outputFile;

	if (OpenAndReadBitMapFile(inputFileName, &inputFile) != 0)
	{
		return;
	}

	outputFile = fopen(outputFileName, "w");

	fwrite(inputFile.fileHeader, 1, sizeof(BITMAPFILEHEADER), outputFile);
	fwrite(inputFile.infoHeader, 1, sizeof(BITMAPINFOHEADER), outputFile);

	pa_PixelData = (int*)malloc(inputFile.infoHeader->biSizeImage);

	if (pa_PixelData == NULL)
	{
		printf("Memory Allocation Error\n");
		return;
	}

	fread(pa_PixelData, 1, inputFile.infoHeader->biSizeImage, inputFile.fp);

	DWORD iCnt;
	for (iCnt = 0; iCnt < inputFile.infoHeader->biWidth * inputFile.infoHeader->biHeight; iCnt++)
	{
		int pixel;
		int red;
		int green;
		int blue;
		int grayScaleValue;
		int resultPixel;

		pixel = pa_PixelData[iCnt];
		red = (pixel & 0x00ff0000) >> 16;
		green = (pixel & 0x0000ff00) >> 8;
		blue = (pixel & 0x000000ff);
		grayScaleValue = red / 3 + green / 3 + blue / 3;
		resultPixel = (grayScaleValue << 16) + (grayScaleValue << 8) + grayScaleValue;

		fwrite(&resultPixel, 1, sizeof(int), outputFile);
	}

	printf("%u Pixels are Read.\n", iCnt);

	fclose(inputFile.fp);
	fclose(outputFile);

	free(inputFile.fileHeader);
	free(inputFile.infoHeader);
	free(pa_PixelData);
}