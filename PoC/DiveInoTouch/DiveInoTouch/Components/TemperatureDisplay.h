#ifndef TEMPERATUREDISPLAY_H_
#define TEMPERATUREDISPLAY_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library

#include "..\Utils\ScreenUtils.h"

class TemperatureDisplay {
public:
	TemperatureDisplay(Adafruit_ILI9341* adafruitTft);

	void init(float temperatureInCelsius);
	void update(float temperatureInCelsius);
	float currentTemperature();
private:
	Adafruit_ILI9341* tft;
	int temperature; // Rounded temperatureInCelsius * 10
};

#endif /* TEMPERATUREDISPLAY_H_ */
