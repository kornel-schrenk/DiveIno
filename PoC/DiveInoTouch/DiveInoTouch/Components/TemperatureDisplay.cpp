#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "TemperatureDisplay.h"

TemperatureDisplay::TemperatureDisplay(Adafruit_ILI9341* adafruitTft)
{
	tft = adafruitTft;
	temperature = 4;
}

float TemperatureDisplay::currentTemperature()
{
	return ((float)temperature)/10;
}

void TemperatureDisplay::init(float temperatureInCelsius)
{
	temperature = (int)(temperatureInCelsius * 10);

	tft->setFont(&DroidSansMono8pt7b);
	tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
	tft->setCursor(70, 30);
	tft->print("cel");

	tft->setFont(&DjbDigital16pt7b);
	tft->setTextColor(ILI9341_DARKGREEN, ILI9341_BLACK);
	tft->setCursor(10, 30);
	tft->print(((float)temperature)/10, 1);
}

void TemperatureDisplay::update(float temperatureInCelsius)
{
	int integerTemperature = (int)(temperatureInCelsius * 10);
	if (temperature != integerTemperature) {

		// Clear the digits
		tft->fillRect(10, 8, 54, 23, ILI9341_BLACK);

		tft->setFont(&DjbDigital16pt7b);
		tft->setTextColor(ILI9341_DARKGREEN, ILI9341_BLACK);
		tft->setCursor(10, 30);
		tft->print(((float)integerTemperature)/10, 1);

		temperature = integerTemperature;
	}
}



