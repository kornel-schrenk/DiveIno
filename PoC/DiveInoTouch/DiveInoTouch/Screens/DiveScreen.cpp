#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "DiveScreen.h"

DiveScreen::DiveScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils, DiveUtils* dUtils,
		TemperatureDisplay* temperatureComponent, DepthDisplay* depthComponent) {
	tft = adafruitTft;

	screenUtils = sUtils;
	diveUtils = dUtils;

	temperatureDisplay = temperatureComponent;
	depthDisplay = depthComponent;

	maximumDepth = 0.0;
}

void DiveScreen::open(float pressureInMilliBar, float temperatureInCelsius)
{
	// Clear the screen
	tft->fillScreen(ILI9341_BLACK);

	tft->drawRect(0, 0, 240, 320, ILI9341_LIGHTGREY);
	tft->drawFastVLine(120, 0, 40, ILI9341_LIGHTGREY);
	tft->drawFastHLine(0, 40, 240, ILI9341_LIGHTGREY);
	tft->drawFastHLine(0, 150, 240, ILI9341_LIGHTGREY);
	tft->fillRect(0, 40, 50, 110, ILI9341_LIGHTGREY);

	screenUtils->drawBatteryStateOfCharge(85);

	screenUtils->drawBmpFile((char *)"Touch/diveGrey.bmp", 3, 60);

	tft->setTextWrap(false);

	// Maximum depth
	tft->setFont(&DroidSansMono8pt7b);
	tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
	tft->setCursor(195, 30);
	tft->print("m");

	temperatureDisplay->init(temperatureInCelsius);
	depthDisplay->init(pressureInMilliBar, 1013, false);

	drawAscend(ASCEND_NORMAL);
	drawMaximumDepth();

	drawOxigenPercentage(32.5);
}

int DiveScreen::handleTouch(Adafruit_FT6206* touchScreen)
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

void DiveScreen::drawAscend(int ascendRate)
{
	tft->setFont(&DroidSansMono8pt7b);
	switch (ascendRate) {
		case ASCEND_OK:
			tft->setTextColor(ILI9341_GREEN);
			tft->setCursor(10, 305);
			tft->print(F("----"));
			break;
		case ASCEND_SLOW:
			tft->setTextColor(ILI9341_WHITE);
			tft->setCursor(10, 305);
			tft->print(F("---|"));
			break;
		case ASCEND_NORMAL:
			tft->setTextColor(ILI9341_YELLOW);
			tft->setCursor(10, 305);
			tft->print(F("--||"));
			break;
		case ASCEND_ATTENTION:
			tft->setTextColor(ILI9341_MAROON);
			tft->setCursor(10, 305);
			tft->print(F("-|||"));
			break;
		case ASCEND_CRITICAL:
			tft->setTextColor(ILI9341_RED);
			tft->setCursor(10, 305);
			tft->print(F("||||"));
			break;
		case ASCEND_DANGER:
			tft->setTextColor(ILI9341_RED);
			tft->setCursor(10, 305);
			tft->print(F("SLOW"));
			break;
		default:
			tft->setTextColor(ILI9341_GREEN);
			tft->setCursor(10, 305);
			tft->print(F("<<--"));
	}
}

void DiveScreen::drawOxigenPercentage(float oxigenPercentage)
{
	if (oxigenPercentage <= 23) {
		tft->setTextColor(ILI9341_GREEN);
	} else {
		tft->setTextColor(ILI9341_MAGENTA);
	}

	tft->setFont(&DroidSansMono8pt7b);
	tft->setCursor(94, 308);
	tft->print(oxigenPercentage, 1);

	tft->setTextColor(ILI9341_WHITE);
	tft->print(F(" %"));
}

void DiveScreen::drawCurrentTemperature(float temperatureInCelsius)
{
	temperatureDisplay->update(temperatureInCelsius);
}

void DiveScreen::drawCurrentDepth(float pressureInMilliBar)
{
	depthDisplay->update(pressureInMilliBar);

	if (maximumDepth < depthDisplay->depth()) {
		maximumDepth = depthDisplay->depth();
		drawMaximumDepth();
	}
}

void DiveScreen::drawMaximumDepth()
{
	// Clear the digits
	tft->fillRect(130, 8, 60, 23, ILI9341_BLACK);

	tft->setFont(&DjbDigital16pt7b);
	tft->setTextColor(ILI9341_DARKCYAN, ILI9341_BLACK);
	tft->setCursor(130, 30);
	tft->print(maximumDepth, 1);
}
