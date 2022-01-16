#pragma once

#include "common.h"

class HealthPack {
public:
	HealthPack();

	double x, y;
	bool active;

	bool CheckHitbox(double targetX, double targetY, double targetWidth, double targetHeight);
};