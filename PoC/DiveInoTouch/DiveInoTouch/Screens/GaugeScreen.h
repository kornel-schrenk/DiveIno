#ifndef GAUGESCREEN_H_
#define GAUGESCREEN_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support

#include "..\Utils\ScreenUtils.h"
#include "..\Utils\TimeUtils.h"

#include "..\Components\DateTimeCounter.h"
#include "..\Components\PressureDisplay.h"
#include "..\Components\TemperatureDisplay.h"
#include "..\Components\DepthDisplay.h"

class GaugeScreen {
public:
	GaugeScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils, TimeUtils* tUtils,
			DateTimeCounter* counterComponent, PressureDisplay* pressureComponent,
			TemperatureDisplay* temperatureComponent, DepthDisplay* depthComponent);

	void open(float pressureInMilliBar, float temperatureInCelsius);
	int handleTouch(Adafruit_FT6206* touchScreen);
	void drawCurrentTime();
	void drawCurrentPressure(float pressureInMilliBar);
	void drawCurrentTemperature(float temperatureInCelsius);
	void drawCurrentDepth(float pressureInMilliBar);

private:
	Adafruit_ILI9341* tft;

	ScreenUtils* screenUtils;
	TimeUtils* timeUtils;

	DateTimeCounter* dateTimeCounter;
	PressureDisplay* pressureDisplay;
	TemperatureDisplay* temperatureDisplay;
	DepthDisplay* depthDisplay;
};


#endif /* GAUGESCREEN_H_ */
