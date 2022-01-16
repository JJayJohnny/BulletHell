#include "enemy.h"
Enemy::Enemy()
{
	x = LEVEL_WIDTH / 5;
	y = LEVEL_HEIGHT / 5;
	width = 178;
	height = 285;
	sprite = SDL_LoadBMP(enemy1SpritePath);
	bulletSprite = SDL_LoadBMP(enemyBulletSpritePath);
	bullets = new Bullet[100];
	lastTimeShot = 0;
	cooldown = 1.5;
	bulletCount = 0;
	type = 0;
	speed = 0;
	health = 10;
	maxHealth = health;
}

void Enemy::Shoot(double currTime, double deltaTime)
{
	static char lastPattern = 'x';
	if (currTime - lastTimeShot >= cooldown)
	{
		lastTimeShot = currTime;
		switch (type)
		{
		case 1:
			if (lastPattern == 'x')
			{
				PatternPlus();
				lastPattern = '+';
			}
			else
			{
				PatternCross();
				lastPattern = 'x';
			}
			break;
		case 2:
			if (int(currTime) % 10 == 0)
				PatternCross();
			else
				PatternSpiral(currTime);
			break;
		case 3:
			if (int(currTime) % 2 == 0)
				PatternSpiral(currTime);
			else
				PatternCircle();
			break;
		}
	}
	for (int i = 0; i < bulletCount; i++)
	{
		bullets[i].CalcPos(deltaTime);
	}
}

void Enemy::PatternPlus()
{
	for (int i = 0; i < 4; i++)
	{
		if (bulletCount + 1 > 99)//wyzerowanie tablicy bulletow
		{
			delete[] bullets;
			bullets = new Bullet[100];
			bulletCount = 0;
		}
		bullets[bulletCount].x = x;
		bullets[bulletCount].y = y;
		switch (i)
		{
		case 0:
			bullets[bulletCount].xVel = -1;
			break;
		case 1:
			bullets[bulletCount].yVel = -1;
			break;
		case 2:
			bullets[bulletCount].xVel = 1;
			break;
		case 3:
			bullets[bulletCount].yVel = 1;
			break;
		}
		bullets[bulletCount].active = true;
		bulletCount++;
	}
}

void Enemy::PatternCross()
{
	for (int i = 0; i < 4; i++)
	{
		if (bulletCount + 1 > 99)//wyzerowanie tablicy bulletow
		{
			delete[] bullets;
			bullets = new Bullet[100];
			bulletCount = 0;
		}
		bullets[bulletCount].x = x;
		bullets[bulletCount].y = y;
		switch (i)
		{
		case 0:
			bullets[bulletCount].xVel = -1;
			bullets[bulletCount].yVel = -1;
			break;
		case 1:
			bullets[bulletCount].yVel = -1;
			bullets[bulletCount].xVel = 1;
			break;
		case 2:
			bullets[bulletCount].xVel = 1;
			bullets[bulletCount].yVel = 1;
			break;
		case 3:
			bullets[bulletCount].yVel = 1;
			bullets[bulletCount].xVel = -1;
			break;
		}
		bullets[bulletCount].active = true;
		bulletCount++;
	}
}

void Enemy::PatternSpiral(double currTime)
{
		if (bulletCount + 1 > 99)//wyzerowanie tablicy bulletow
		{
			delete[] bullets;
			bullets = new Bullet[100];
			bulletCount = 0;
		}
		bullets[bulletCount].x = x;
		bullets[bulletCount].y = y;

		bullets[bulletCount].xVel = sin(M_PI * currTime);
		bullets[bulletCount].yVel = cos(M_PI * currTime);

		bullets[bulletCount].active = true;
		bulletCount++;
}

void Enemy::PatternCircle()
{
	for (int i = 0; i < 12; i++)
	{
		if (bulletCount + 1 > 99)//wyzerowanie tablicy bulletow
		{
			delete[] bullets;
			bullets = new Bullet[100];
			bulletCount = 0;
		}
		bullets[bulletCount].x = x;
		bullets[bulletCount].y = y;

		bullets[bulletCount].xVel = sin(M_PI/6 * i);
		bullets[bulletCount].yVel = cos(M_PI/6 * i);

		bullets[bulletCount].active = true;
		bulletCount++;
	}
}

void Enemy::Move(double currTime, double deltaTime)
{
	//DrawSurface(eti,
			//SCREEN_WIDTH / 2 + sin(distance) * SCREEN_HEIGHT / 3,
			//SCREEN_HEIGHT / 2 + cos(distance) * SCREEN_HEIGHT / 3);

	x += sin(currTime) * deltaTime * speed;
	y += cos(currTime) * deltaTime * speed;
}