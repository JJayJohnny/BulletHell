#include "player.h"

Player::Player()
{
	spriteL = SDL_LoadBMP(characterLeftSpritePath);
	spriteR = SDL_LoadBMP(characterRightSpritePath);
	bulletSprite = SDL_LoadBMP(playerBulletSpritePath);
	shield = SDL_LoadBMP(playerShieldPath);
	activeSprite = spriteL;
	x = LEVEL_WIDTH / 2;
	y = LEVEL_HEIGHT / 2;
	speed = 400;
	xVel = 0;
	yVel = 0;
	width = 90;
	height = 90;
	bullets = new Bullet[100];
	bulletCount = 0;
	lastTimeShot = 0;
	CalcOffset();
	invincible = false;
	timeOfHit = 0;
	health = 5;
	maxHealth = health;
	score = 0;
	multiplier = 0;
}

void Player::Move(double delta)
{
	x += speed * xVel * delta;
	y += speed * yVel * delta;

	CalcOffset();

	if (x < width/2) x = width/2;
	else if (x > LEVEL_WIDTH - width/2) x = LEVEL_WIDTH - width/2;
	if (y < height/2) y = height/2;
	else if (y > LEVEL_HEIGHT - height/2) y = LEVEL_HEIGHT - height/2;
}

void Player::CalcOffset()
{
	if (x > SCREEN_WIDTH / 2 && x < LEVEL_WIDTH - (SCREEN_WIDTH / 2))
		xOff = SCREEN_WIDTH / 2 - x;
	else if(x>= LEVEL_WIDTH - (SCREEN_WIDTH / 2))
		xOff = SCREEN_WIDTH - LEVEL_WIDTH;
	else
		xOff = 0;

	if (y > SCREEN_HEIGHT / 2 && y < LEVEL_HEIGHT - (SCREEN_HEIGHT / 2))
		yOff = SCREEN_HEIGHT / 2 - y;
	else if (y >= LEVEL_HEIGHT - (SCREEN_HEIGHT / 2))
		yOff = SCREEN_HEIGHT - LEVEL_HEIGHT;
	else
		yOff = 0;
}

void Player::Shoot(double currTime, char direction)
{
	if (currTime - lastTimeShot >= playerShootCooldown)
	{
		lastTimeShot = currTime;
		if (bulletCount > 99)
			bulletCount = 0;
		bullets[bulletCount].x = x;
		bullets[bulletCount].y = y;
		switch (direction)
		{
		case 'N':
			bullets[bulletCount].yVel = -1;
			break;
		case 'E':
			bullets[bulletCount].xVel = 1;
			break;
		case 'S':
			bullets[bulletCount].yVel = 1;
			break;
		case 'W':
			bullets[bulletCount].xVel = -1;
			break;
		}
		bullets[bulletCount].active = true;
		bulletCount++;
	}
}

void Player::MoveBullets(double deltaTime)
{
	for (int i = 0; i < bulletCount; i++)
	{
		bullets[i].CalcPos(deltaTime);
	}
}

void Player::InvincibilityCountdown(double currTime)
{
	if (currTime - timeOfHit >= playerInvincibilityLength)
		invincible = false;
}

void Player::CalcScore(char change)
{
	switch (change)
	{
	case '+':
		if (multiplier <= 0)
			multiplier = 1;
		else
			multiplier *= 2;
		break;
	case '-':
		if (multiplier >= 0)
			multiplier = -1;
		else
			multiplier *= 2;
		break;
	}
	score += multiplier;
}