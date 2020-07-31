#ifndef PRESSUREDISPLAY_H_
#define PRESSUREDISPLAY_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library

#include "..\Utils\ScreenUtils.h"

class PressureDisplay {
public:
	PressureDisplay(Adafruit_ILI9341* adafruitTft);

	void init(float pressureInMilliBar);
	void update(float pressureInMilliBar);
	int currentPressure();
private:
	Adafruit_ILI9341* tft;
	int pressure;
};

#endif /* PRESSUREDISPLAY_H_ */
