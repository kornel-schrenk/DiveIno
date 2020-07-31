#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "HomeScreen.h"

HomeScreen::HomeScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* screenUtils) {
	tft = adafruitTft;
	utils = screenUtils;
}

void HomeScreen::open() {
	// Clear the screen
	tft->fillScreen(ILI9341_BLACK);

	utils->drawBmpFile((char *)"Touch/dive.bmp", 30, 15);
	utils->drawButtonFrame(20, 5, 90, 90, 10, false);
	utils->drawBmpFile((char *)"Touch/pressure.bmp", 140, 15);
	utils->drawButtonFrame(130, 5, 90, 90, 10, false);

	utils->drawBmpFile((char *)"Touch/logbook.bmp", 30, 125);
	utils->drawButtonFrame(20, 115, 90, 90, 10, false);
	utils->drawBmpFile((char *)"Touch/gauge.bmp", 140, 125);
	utils->drawButtonFrame(130, 115, 90, 90, 10, false);

	utils->drawBmpFile((char *)"Touch/settings.bmp", 30, 235);
	utils->drawButtonFrame(20, 225, 90, 90, 10, false);
	utils->drawBmpFile((char *)"Touch/home.bmp", 140, 235);
	utils->drawButtonFrame(130, 225, 90, 90, 10, false);
}

int HomeScreen::handleTouch(Adafruit_FT6206* touchScreen)
{
	if (touchScreen->touched()) {
		TS_Point touchPoint = touchScreen->getPoint();

		// flip it around to match the screen.
		touchPoint.x = map(touchPoint.x, 0, 240, 240, 0);
		touchPoint.y = map(touchPoint.y, 0, 320, 320, 0);

		if (20 < touchPoint.x && touchPoint.x < 110 &&
			5 < touchPoint.y && touchPoint.y < 95) {
			drawButtonPress(BUTTON_DIVE);
			return GO_DIVE;
		} else if (130 < touchPoint.x && touchPoint.x < 220 &&
				5 < touchPoint.y && touchPoint.y < 95) {
			drawButtonPress(BUTTON_GAUGE);
			return GO_GAUGE;
		} else if (20 < touchPoint.x && touchPoint.x < 110 &&
				115 < touchPoint.y && touchPoint.y < 205) {
			drawButtonPress(BUTTON_LOGBOOK);
			return GO_LOGBOOK;
		} else if (130 < touchPoint.x && touchPoint.x < 220 &&
				115 < touchPoint.y && touchPoint.y < 205) {
			drawButtonPress(BUTTON_SURFACE);
			return GO_SURFACE;
		} else if (20 < touchPoint.x && touchPoint.x < 110 &&
				225 < touchPoint.y && touchPoint.y < 315) {
			drawButtonPress(BUTTON_SETTINGS);
			return GO_SETTINGS;
		} else if (130 < touchPoint.x && touchPoint.x < 220 &&
				225 < touchPoint.y && touchPoint.y < 315) {
			drawButtonPress(BUTTON_ABOUT);
			return GO_ABOUT;
		}
	}
	return NO_ACTION;
}

void HomeScreen::drawButtonPress(int button) {
	switch (button) {
	case BUTTON_DIVE:
		utils->drawButtonFrame(20, 5, 90, 90, 10, true);
		break;
	case BUTTON_GAUGE:
		utils->drawButtonFrame(130, 5, 90, 90, 10, true);
		break;
	case BUTTON_LOGBOOK:
		utils->drawButtonFrame(20, 115, 90, 90, 10, true);
		break;
	case BUTTON_SURFACE:
		utils->drawButtonFrame(130, 115, 90, 90, 10, true);
		break;
	case BUTTON_SETTINGS:
		utils->drawButtonFrame(20, 225, 90, 90, 10, true);
		break;
	case BUTTON_ABOUT:
		utils->drawButtonFrame(130, 225, 90, 90, 10, true);
		break;
	}

	delay(100);

	switch (button) {
	case BUTTON_DIVE:
		utils->drawButtonFrame(20, 5, 90, 90, 10, false);
		break;
	case BUTTON_GAUGE:
		utils->drawButtonFrame(130, 5, 90, 90, 10, false);
		break;
	case BUTTON_LOGBOOK:
		utils->drawButtonFrame(20, 115, 90, 90, 10, false);
		break;
	case BUTTON_SURFACE:
		utils->drawButtonFrame(130, 115, 90, 90, 10, false);
		break;
	case BUTTON_SETTINGS:
		utils->drawButtonFrame(20, 225, 90, 90, 10, false);
		break;
	case BUTTON_ABOUT:
		utils->drawButtonFrame(130, 225, 90, 90, 10, false);
		break;
	}

}

