#include "bullet.h"

Bullet::Bullet()
{
	x = 0;
	y = 0;
	xVel = 0;
	yVel = 0;
	speed = 600;
	active = false;
}

void Bullet::CalcPos(double deltaTime)
{
	if (active)
	{
		x += xVel * speed * deltaTime;
		y += yVel * speed * deltaTime;

		if (x <= 0 || x >= LEVEL_WIDTH || y <= 0 || y >= LEVEL_HEIGHT)
			active = false;
	}
}

bool Bullet::CheckHitbox(double targetX, double targetY, double targetWidth, double targetHeight)
{
	if (x >= targetX - targetWidth / 2 && x <= targetX + targetWidth / 2 && y >= targetY - targetHeight / 2 && y <= targetY + targetHeight / 2)
		return true;
	else
		return false;
}