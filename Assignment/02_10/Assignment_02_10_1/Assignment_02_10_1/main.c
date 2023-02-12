#include <stdio.h>
#include "ImageConverter.h"

#define LINE_LENGTH 1024

int main(void)
{
	printf("=== Image Converter ===\n");

	while (1)
	{
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
			char fileName1[LINE_LENGTH];
			char fileName2[LINE_LENGTH];
			char fileName3[LINE_LENGTH];
		case 1:
			printf("Blending File Name 1 : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &fileName1) == 0)
			{
				goto wrong_file_name;
			}
			
			printf("Blending File Name 2 : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &fileName2) == 0)
			{
				goto wrong_file_name;
			}

			printf("Output File Name 3 : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &fileName3) == 0)
			{
				goto wrong_file_name;
			}

			AlphaBlendTwoImages(fileName1, fileName2, fileName3);
			break;

		case 2:
			printf("Apply Gray-Scaling File Name : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &fileName1) == 0)
			{
				goto wrong_file_name;
			}

			printf("Output File Name : ");
			fgets(line, LINE_LENGTH, stdin);
			if (sscanf(line, "%s", &fileName2) == 0)
			{
				goto wrong_file_name;
			}

			GrayScaleTransform(fileName1, fileName2);
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