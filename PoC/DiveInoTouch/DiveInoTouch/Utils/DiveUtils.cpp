#include "DiveUtils.h"

#include <Arduino.h>

DiveUtils::DiveUtils() {
}

float DiveUtils::calculateDepthFromPressure(int pressureInMilliBar, int seaLevelPressure)
{
	float depthInMeter = 0;
	if (pressureInMilliBar > seaLevelPressure) {
		depthInMeter = (pressureInMilliBar - seaLevelPressure) / (9.80665 * 10.25);
	}
	return depthInMeter;
}
