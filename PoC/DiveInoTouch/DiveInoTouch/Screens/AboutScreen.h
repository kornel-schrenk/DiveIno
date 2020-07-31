#ifndef ABOUTSCREEN_H_
#define ABOUTSCREEN_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support

#include "..\Utils\ScreenUtils.h"
#include "..\Utils\TimeUtils.h"
#include "..\Components\DateTimeCounter.h"

#define BUTTON_HOME 8

class AboutScreen {
public:
	AboutScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils, TimeUtils* tUtils, DateTimeCounter* counter);

	void open(String versionNumber);
	int handleTouch(Adafruit_FT6206* touchScreen);
	void drawCurrentTime();

private:
	Adafruit_ILI9341* tft;
	ScreenUtils* screenUtils;
	TimeUtils* timeUtils;

	DateTimeCounter* dateTimeCounter;

	void drawButtonPress(int button);
};
#endif /* ABOUTSCREEN_H_ */
