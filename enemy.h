#pragma once

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#include "common.h"
#include "bullet.h"

const char enemy1SpritePath[] = "./sprites/enemy1.bmp";
const char enemy2SpritePath[] = "./sprites/enemy2.bmp";
const char enemy3SpritePath[] = "./sprites/enemy3.bmp";
const char enemyBulletSpritePath[] = "./sprites/bulletEnemy.bmp";

class Enemy {
public:
	Enemy();
	int type;
	double x, y;
	double width, height;
	SDL_Surface* sprite;
	SDL_Surface* bulletSprite;
	Bullet* bullets;
	int bulletCount;
	double lastTimeShot;
	double cooldown;
	double speed;
	int health, maxHealth;

	void Shoot(double currTime, double deltaTime);
	void Move(double currTime, double deltaTime);
	void PatternPlus();
	void PatternCross();
	void PatternSpiral(double currTime);
	void PatternCircle();
};