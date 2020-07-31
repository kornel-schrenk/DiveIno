#ifndef DEPTHDISPLAY_H_
#define DEPTHDISPLAY_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library

#include "..\Utils\ScreenUtils.h"
#include "..\Utils\DiveUtils.h"

class DepthDisplay {
public:
	DepthDisplay(Adafruit_ILI9341* adafruitTft, DiveUtils* dUtils);

	void init(float pressureInMilliBar, int seaLevelPressureInMilliBar, bool isImperial);
	void update(float pressureInMilliBar);
	float depth();
private:
	Adafruit_ILI9341* tft;
	DiveUtils* diveUtils;

	int pressure;
	int seaLevelPressure;
	bool isImperialValue;

	float currentDepth;
};

#endif /* DEPTHDISPLAY_H_ */
