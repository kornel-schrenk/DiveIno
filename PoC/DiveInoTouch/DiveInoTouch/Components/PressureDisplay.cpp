#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "PressureDisplay.h"

PressureDisplay::PressureDisplay(Adafruit_ILI9341* adafruitTft)
{
	tft = adafruitTft;
	pressure = 1013.2;
}

int PressureDisplay::currentPressure()
{
	return pressure;
}

void PressureDisplay::init(float pressureInMilliBar)
{
	pressure = (int)pressureInMilliBar;

	tft->setFont(&DroidSansMono8pt7b);
	tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
	tft->setCursor(195, 30);
	tft->print("mBar");

	tft->setFont(&DjbDigital16pt7b);
	tft->setTextColor(ILI9341_PINK, ILI9341_BLACK);
	if (pressure < 1000) {
		tft->setCursor(143, 30);
	} else {
		tft->setCursor(130, 30);
	}
	tft->print(pressure);
}

void PressureDisplay::update(float pressureInMilliBar)
{
	int integerPressure = (int)pressureInMilliBar;
	if (pressure != integerPressure) {

		// Clear the digits
		tft->fillRect(130, 8, 60, 23, ILI9341_BLACK);

		tft->setFont(&DjbDigital16pt7b);
		tft->setTextColor(ILI9341_PINK, ILI9341_BLACK);
		if (integerPressure < 1000) {
			tft->setCursor(143, 30);
		} else {
			tft->setCursor(130, 30);
		}
		tft->print(integerPressure);

		pressure = integerPressure;
	}
}

