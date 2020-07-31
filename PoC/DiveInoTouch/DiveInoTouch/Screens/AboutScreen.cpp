#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "AboutScreen.h"

AboutScreen::AboutScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils, TimeUtils* tUtils, DateTimeCounter* counter) {
	tft = adafruitTft;
	screenUtils = sUtils;
	timeUtils = tUtils;
	dateTimeCounter = counter;
}

void AboutScreen::open(String versionNumber)
{
	tft->fillScreen(ILI9341_BLACK);

	screenUtils->drawBatteryStateOfCharge(101);

	tft->setTextWrap(false);
	tft->setFont(&DroidSansMono8pt7b);

	tft->setCursor(50, 30);
	tft->setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	tft->println("www.diveino.hu");

	screenUtils->drawBmpFile((char *)"Touch/logo.bmp", 50, 70);
	screenUtils->drawButtonFrame(40, 60, 160, 160, 10, false);

	tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft->setCursor(10, 310);
	tft->println(versionNumber);

	dateTimeCounter->init(timeUtils->getCurrentTime());
}

int AboutScreen::handleTouch(Adafruit_FT6206* touchScreen)
{
	if (touchScreen->touched()) {
		TS_Point touchPoint = touchScreen->getPoint();

		// flip it around to match the screen.
		touchPoint.x = map(touchPoint.x, 0, 240, 240, 0);
		touchPoint.y = map(touchPoint.y, 0, 320, 320, 0);

		if (50 < touchPoint.x && touchPoint.x < 50+140 &&
			70 < touchPoint.y && touchPoint.y < 70+140) {
			drawButtonPress(BUTTON_HOME);
			return GO_HOME;
		}
		delay(100);
	}

	return NO_ACTION;
}

void AboutScreen::drawButtonPress(int button) {
	switch (button) {
	case BUTTON_HOME:
		screenUtils->drawButtonFrame(40, 60, 160, 160, 10, true);
		break;
	}

	delay(100);

	switch (button) {
	case BUTTON_HOME:
		screenUtils->drawButtonFrame(40, 60, 160, 160, 10, false);
		break;
	}
}

void AboutScreen::drawCurrentTime()
{
	dateTimeCounter->update(timeUtils->getCurrentTime());
}
