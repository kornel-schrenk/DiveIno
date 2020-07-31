#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "GaugeScreen.h"

GaugeScreen::GaugeScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils, TimeUtils* tUtils,
		DateTimeCounter* counterComponent, PressureDisplay* pressureComponent,
		TemperatureDisplay* temperatureComponent, DepthDisplay* depthComponent) {
	tft = adafruitTft;

	screenUtils = sUtils;
	timeUtils = tUtils;

	dateTimeCounter = counterComponent;
	pressureDisplay = pressureComponent;
	temperatureDisplay = temperatureComponent;
	depthDisplay = depthComponent;
}

void GaugeScreen::open(float pressureInMilliBar, float temperatureInCelsius)
{
	// Clear the screen
	tft->fillScreen(ILI9341_BLACK);

	tft->drawRect(0, 0, 240, 320, ILI9341_LIGHTGREY);
	tft->drawFastVLine(120, 0, 40, ILI9341_LIGHTGREY);
	tft->drawFastHLine(0, 40, 240, ILI9341_LIGHTGREY);
	tft->drawFastHLine(0, 150, 240, ILI9341_LIGHTGREY);
	tft->fillRect(0, 40, 50, 110, ILI9341_LIGHTGREY);

	screenUtils->drawBatteryStateOfCharge(85);

	screenUtils->drawBmpFile((char *)"Touch/pressureGrey.bmp", 3, 60);

	tft->setTextWrap(false);

	pressureDisplay->init(pressureInMilliBar);
	temperatureDisplay->init(temperatureInCelsius);
	depthDisplay->init(pressureInMilliBar, 1013, false);

	dateTimeCounter->init(timeUtils->getCurrentTime());
}

int GaugeScreen::handleTouch(Adafruit_FT6206* touchScreen)
{
	if (touchScreen->touched()) {
		TS_Point touchPoint = touchScreen->getPoint();

		// flip it around to match the screen.
		touchPoint.x = map(touchPoint.x, 0, 240, 240, 0);
		touchPoint.y = map(touchPoint.y, 0, 320, 320, 0);

		if (0 < touchPoint.x && touchPoint.x < 240 &&
			40 < touchPoint.y && touchPoint.y < 150) {
			return GO_HOME;
		}
		delay(100);
	}

	return NO_ACTION;
}

void GaugeScreen::drawCurrentTime()
{
	dateTimeCounter->update(timeUtils->getCurrentTime());
}

void GaugeScreen::drawCurrentPressure(float pressureInMilliBar)
{
	pressureDisplay->update(pressureInMilliBar);
}

void GaugeScreen::drawCurrentTemperature(float temperatureInCelsius)
{
	temperatureDisplay->update(temperatureInCelsius);
}

void GaugeScreen::drawCurrentDepth(float pressureInMilliBar)
{
	depthDisplay->update(pressureInMilliBar);
}
