#include <stdio.h>
#include "ImageConverter.h"

#define LINE_LENGTH 1024

int main(void)
{
	printf("=== Image Converter ===\n");
	printf("Input Images must be loacated in './input' directory.\n");

	while (1)
	{
		char inputFileName1[LINE_LENGTH + 10];
		char inputFileName2[LINE_LENGTH + 10];
		char outputFileName[LINE_LENGTH + 10];

		char line[LINE_LENGTH];
		int input = -1;

		printf("1. Alpha Blending  2. Gray Scaling  Other. Quit Program\n");
		printf(" >>> : ");

		fgets(line, LINE_LENGTH, stdin);
		if (sscanf(line, "%d", &input) == 0)
		{
			goto quit_main_loop;
		}

		switch (input)
		{
		case 1:
			printf("Blending File Name 1 : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &line) == 0)
			{
				goto wrong_file_name;
			}
			sprintf(inputFileName1, "./input/%s", line);
			
			printf("Blending File Name 2 : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &line) == 0)
			{
				goto wrong_file_name;
			}
			sprintf(inputFileName2, "./input/%s", line);

			printf("Output File Name 3 : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &line) == 0)
			{
				goto wrong_file_name;
			}
			sprintf(outputFileName, "./output/%s", line);

			AlphaBlendTwoImages(inputFileName1, inputFileName2, outputFileName);
			break;

		case 2:
			printf("Apply Gray-Scaling File Name : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &line) == 0)
			{
				goto wrong_file_name;
			}
			sprintf(inputFileName1, "./input/%s", line);

			printf("Output File Name : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &line) == 0)
			{
				goto wrong_file_name;
			}
			sprintf(outputFileName, "./output/%s", line);

			GrayScaleTransform(inputFileName1, outputFileName);
			break;

		default:
			goto quit_main_loop;
		}

		printf("Done!\n");
		continue;

	wrong_file_name:
		printf("Wrong Input File Name.\n");
	quit_main_loop:
		break;
	}
	return 0;
}