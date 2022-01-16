#pragma once

#include "common.h"

class Bullet {
public:
	Bullet();
	double x, y;
	double xVel, yVel;
	double speed;
	bool active;

	void CalcPos(double deltaTime);
	bool CheckHitbox(double targetX, double targetY, double targetWidth, double targetHeight);
};