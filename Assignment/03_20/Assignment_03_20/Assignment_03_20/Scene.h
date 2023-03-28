#pragma once

typedef enum sceneType
{
	MENU,
	LOAD,
	GAMESTAGE
};

void UpdateMenu(void);
void UpdateGameStage(void);