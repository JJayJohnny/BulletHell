#pragma once

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#include "common.h"
#include "bullet.h"

const char characterLeftSpritePath[] = "./sprites/characterL.bmp";
const char characterRightSpritePath[] = "./sprites/characterR.bmp";
const char playerBulletSpritePath[] = "./sprites/bulletPlayer.bmp";
const char playerShieldPath[] = "./sprites/playerShield.bmp";

const double playerShootCooldown = 0.5;
const double playerInvincibilityLength = 3;

class Player {
public:
	Player();
	SDL_Surface* spriteL, *spriteR, *activeSprite, *bulletSprite, *shield;
	double x, y;
	int xVel, yVel;
	double xOff, yOff;
	double width, height;
	Bullet* bullets;
	int bulletCount;
	double lastTimeShot;
	bool invincible;
	double timeOfHit;
	int health, maxHealth;

	int speed;

	int score;
	int multiplier;
	
	void Move(double delta);
	void CalcOffset();
	void Shoot(double currTime, char direction);
	void MoveBullets(double deltaTime);
	void InvincibilityCountdown(double currTime);
	void CalcScore(char change);
};