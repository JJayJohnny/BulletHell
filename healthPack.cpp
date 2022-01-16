#include "healthPack.h"

HealthPack::HealthPack()
{
	active = false;
	x = 0;
	y = 0;
}

bool HealthPack::CheckHitbox(double targetX, double targetY, double targetWidth, double targetHeight)
{
	if (x >= targetX - targetWidth / 2 && x <= targetX + targetWidth / 2 && y >= targetY - targetHeight / 2 && y <= targetY + targetHeight / 2)
		return true;
	else
		return false;
}