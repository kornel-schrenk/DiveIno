#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "DepthDisplay.h"

float SEA_LEVEL_PRESSURE = 1013.2;

DepthDisplay::DepthDisplay(Adafruit_ILI9341* adafruitTft, DiveUtils* dUtils)
{
	tft = adafruitTft;
	diveUtils = dUtils;

	pressure = 1013;
	seaLevelPressure = 1013;
	isImperialValue = false;

	currentDepth = 0.0;
}

float DepthDisplay::depth()
{
	return currentDepth;
}

void DepthDisplay::init(float pressureInMilliBar, int seaLevelPressureInMilliBar, bool isImperial)
{
	pressure = (int)pressureInMilliBar;
	seaLevelPressure = seaLevelPressureInMilliBar;
	isImperialValue = isImperial;

	currentDepth = diveUtils->calculateDepthFromPressure(pressureInMilliBar, seaLevelPressure);

	tft->setFont(&DroidSansMono16pt7b);
	tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
	tft->setCursor(215, 130);
	if (isImperial) {
		tft->print(F("ft"));
	} else {
		tft->print(F("m"));
	}

	tft->setFont(&DjbDigital48pt7b);
	tft->setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
	tft->setCursor(55, 130);
	if (isImperial) {
		tft->print(currentDepth*3.28, 0);
	} else {
		tft->print(currentDepth, 1);
	}
}

void DepthDisplay::update(float pressureInMilliBar)
{
	int integerPressure = (int)pressureInMilliBar;
	if (pressure != integerPressure) {

		// Clear the digits
		tft->fillRect(55, 60, 155, 75, ILI9341_BLACK);

		tft->setFont(&DjbDigital48pt7b);
		tft->setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
		tft->setCursor(55, 130);

		currentDepth = diveUtils->calculateDepthFromPressure(pressureInMilliBar, seaLevelPressure);
		if (isImperialValue) {
			tft->print(currentDepth*3.28, 0);
		} else {
			tft->print(currentDepth, 1);
		}

		pressure = integerPressure;
	}
}
