#pragma once

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#include <time.h>
#include "player.h"
#include "enemy.h"
#include "common.h"
#include "bullet.h"
#include "healthPack.h"

const char background1Path[] = "./sprites/background.bmp";
const char background2Path[] = "./sprites/background2.bmp";
const char background3Path[] = "./sprites/background3.bmp";
const char deathScreenPath[] = "./sprites/deathScreen.bmp";
const char victoryScreenPath[] = "./sprites/victoryScreen.bmp";
const char healthPackPath[] = "./sprites/healthPack.bmp";

const int maxHealthPackCount = 3;
const double healthPackCooldown = 3;


class Game {
public:
	Game();
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Surface** backgrounds, *deathScreen, *victoryScreen, *healthPack;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps;
	int currLevel, nextLevel;

	Player player;
	Enemy enemy;

	HealthPack* healthPacks;
	double healthPackLastPlaced;

	void DrawString(int x, int y, const char* text);
	void DrawSurface(SDL_Surface* sprite, int x, int y);
	void DrawLine(int x, int y, int l, int dx, int dy, Uint32 color);
	void DrawRectangle(int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
	SDL_Surface* LoadGraphic(char path[]);
	bool Update();
	char GetInput();
	void DrawInterface();
	void ExitGame();
	void SelectLevel();
	void PrintTitleScreen();
	void SetupLevel();
	void ManageBullets();
	void DisplayBullets();
	void DisplayHealthBar(int health, int maxHealth, double x, double y);
	void DeathScreen();
	void VictoryScreen();
	void ManageHealthPacks();
	void DisplayHealthPacks();
};