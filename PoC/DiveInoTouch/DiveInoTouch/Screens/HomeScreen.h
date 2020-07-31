#ifndef TouchView_h
#define TouchView_h

#include <SdFat.h>
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support

#include "..\Utils\ScreenUtils.h"

#define BUTTON_DIVE 0
#define BUTTON_GAUGE 1
#define BUTTON_LOGBOOK 2
#define BUTTON_SURFACE 3
#define BUTTON_SETTINGS 4
#define BUTTON_ABOUT 5

extern SdFat SD;

class HomeScreen {
public:
	HomeScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* screenUtils);

	void open();
	int handleTouch(Adafruit_FT6206* touchScreen);

private:
	Adafruit_ILI9341* tft;
	ScreenUtils* utils;

	void drawButtonPress(int button);
};

#endif
