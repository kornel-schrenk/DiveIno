#ifndef UTILS_DIVEUTILS_H_
#define UTILS_DIVEUTILS_H_

#include <Arduino.h>

class DiveUtils {
public:
	DiveUtils();

	float calculateDepthFromPressure(int pressureInMilliBar, int seaLevelPressure);
private:

};

#endif /* UTILS_DIVEUTILS_H_ */
