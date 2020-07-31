#ifndef DIVESCREEN_H_
#define DIVESCREEN_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support

#include "..\Utils\ScreenUtils.h"
#include "..\Utils\DiveUtils.h"

#include "..\Components\TemperatureDisplay.h"
#include "..\Components\DepthDisplay.h"

enum ascendRates
{
	DESCEND = -1,
	ASCEND_OK = 0,
	ASCEND_SLOW = 1,
	ASCEND_NORMAL = 2,
	ASCEND_ATTENTION = 3,
	ASCEND_CRITICAL = 4,
	ASCEND_DANGER = 5,
};

class DiveScreen {
public:
	DiveScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils, DiveUtils* dUtils,
			TemperatureDisplay* temperature, DepthDisplay* depthComponent);

	void open(float pressureInMilliBar, float temperatureInCelsius);
	int handleTouch(Adafruit_FT6206* touchScreen);
	void drawAscend(int ascendRate);
	void drawCurrentTemperature(float temperatureInCelsius);
	void drawCurrentDepth(float pressureInMilliBar);

private:
	Adafruit_ILI9341* tft;

	ScreenUtils* screenUtils;
	DiveUtils* diveUtils;

	TemperatureDisplay* temperatureDisplay;
	DepthDisplay* depthDisplay;

	float maximumDepth;
	void drawMaximumDepth();
	void drawOxigenPercentage(float oxigenPercentage);
};

#endif /* DIVESCREEN_H_ */
