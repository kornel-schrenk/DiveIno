#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <RTClib.h>

#include "CalendarScreen.h"

uint16_t monthMax[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

CalendarScreen::CalendarScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils) {
	tft = adafruitTft;
	screenUtils = sUtils;

	year = 2018;
	month = 1;
	day = 1;
	hour = 12;
	minute = 34;
	second = 56;
}

void CalendarScreen::open()
{
	DateTime now = rtc.now();
	year = now.year();
	month = now.month();
	day = now.day();
	hour = now.hour();
	minute = now.minute();
	second = now.second();

	// Clear the screen
	tft->fillScreen(ILI9341_BLACK);

	char buffer[2];

	//---------------------DATE----------------------------------

	uint16_t startX = 5;
	uint16_t startY = 50;

	tft->setFont(&DroidSansMono16pt7b);
	tft->setTextColor(ILI9341_DARKCYAN, ILI9341_BLACK);

	tft->setCursor(startX+7, startY+30);
	tft->print(year);
	screenUtils->drawButtonFrame(startX, startY, 90, 40, 5, false);

	tft->setCursor(startX+91, startY+30);
	tft->print('-');

	tft->setCursor(startX+117, startY+30);
	sprintf(buffer, "%02d", month);
	tft->print(buffer);
	screenUtils->drawButtonFrame(startX+110, startY, 50, 40, 5, false);

	tft->setCursor(startX+161, startY+30);
	tft->print('-');

	tft->setCursor(startX+187, startY+30);
	sprintf(buffer, "%02d", day);
	tft->print(buffer);
	screenUtils->drawButtonFrame(startX+180, startY, 50, 40, 5, false);

	//---------------------TIME----------------------------------
	startX = 30;
	startY = 130;

	tft->setCursor(startX+7, startY+30);
	sprintf(buffer, "%02d", hour);
	tft->print(buffer);
	screenUtils->drawButtonFrame(startX, startY, 50, 40, 5, false);

	tft->setCursor(startX+51, startY+30);
	tft->print(':');

	tft->setCursor(startX+77, startY+30);
	sprintf(buffer, "%02d", minute);
	tft->print(buffer);
	screenUtils->drawButtonFrame(startX+70, startY, 50, 40, 5, false);

	tft->setCursor(startX+121, startY+30);
	tft->print(':');

	tft->setCursor(startX+147, startY+30);
	sprintf(buffer, "%02d", second);
	tft->print(buffer);
	screenUtils->drawButtonFrame(startX+140, startY, 50, 40, 5, false);

	screenUtils->drawBmpFile((char *)"Touch/back.bmp", 50, 250);
	screenUtils->drawButtonFrame(45, 245, 70, 70, 5, false);

	screenUtils->drawBmpFile((char *)"Touch/settingsSave.bmp", 130, 250);
	screenUtils->drawButtonFrame(125, 245, 70, 70, 5, false);
}

int CalendarScreen::handleTouch(Adafruit_FT6206* touchScreen)
{
	if (touchScreen->touched()) {
		TS_Point touchPoint = touchScreen->getPoint();

		// flip it around to match the screen.
		touchPoint.x = map(touchPoint.x, 0, 240, 240, 0);
		touchPoint.y = map(touchPoint.y, 0, 320, 320, 0);

		if (45 < touchPoint.x && touchPoint.x < 45+70 &&
			245 < touchPoint.y && touchPoint.y < 245+70) {
			drawButtonPress(BUTTON_BACK);
			return GO_SETTINGS;
		} else if (125 < touchPoint.x && touchPoint.x < 125+70 &&
				   245 < touchPoint.y && touchPoint.y < 245+70) {
			drawButtonPress(BUTTON_SAVE);
			return NO_ACTION; //TODO Modify it to SAVE
		} else if (5 < touchPoint.x && touchPoint.x < 5+90 &&
				   50 < touchPoint.y && touchPoint.y < 50+40) {
			incrementDateTime(BUTTON_YEAR);
			drawButtonPress(BUTTON_YEAR);
			return NO_ACTION;
		} else if (115 < touchPoint.x && touchPoint.x < 115+50 &&
				   50 < touchPoint.y && touchPoint.y < 50+40) {
			incrementDateTime(BUTTON_MONTH);
			drawButtonPress(BUTTON_MONTH);
			return NO_ACTION;
		} else if (185 < touchPoint.x && touchPoint.x < 185+50 &&
				   50 < touchPoint.y && touchPoint.y < 50+40) {
			incrementDateTime(BUTTON_DAY);
			drawButtonPress(BUTTON_DAY);
			return NO_ACTION;
		} else if (30 < touchPoint.x && touchPoint.x < 30+50 &&
				   130 < touchPoint.y && touchPoint.y < 130+40) {
			incrementDateTime(BUTTON_HOUR);
			drawButtonPress(BUTTON_HOUR);
			return NO_ACTION;
		} else if (100 < touchPoint.x && touchPoint.x < 100+50 &&
				   130 < touchPoint.y && touchPoint.y < 130+40) {
			incrementDateTime(BUTTON_MINUTE);
			drawButtonPress(BUTTON_MINUTE);
			return NO_ACTION;
		} else if (170 < touchPoint.x && touchPoint.x < 170+50 &&
				   130 < touchPoint.y && touchPoint.y < 130+40) {
			incrementDateTime(BUTTON_SECOND);
			drawButtonPress(BUTTON_SECOND);
			return NO_ACTION;
		}

		delay(100);
	}

	return NO_ACTION;
}

void CalendarScreen::drawButtonPress(int button) {
	switch (button) {
	case BUTTON_BACK:
		screenUtils->drawButtonFrame(45, 245, 70, 70, 5, true);
		break;
	case BUTTON_SAVE:
		screenUtils->drawButtonFrame(125, 245, 70, 70, 5, true);
		break;
	case BUTTON_YEAR:
		screenUtils->drawButtonFrame(5, 50, 90, 40, 5, true);
		break;
	case BUTTON_MONTH:
		screenUtils->drawButtonFrame(115, 50, 50, 40, 5, true);
		break;
	case BUTTON_DAY:
		screenUtils->drawButtonFrame(185, 50, 50, 40, 5, true);
		break;
	case BUTTON_HOUR:
		screenUtils->drawButtonFrame(30, 130, 50, 40, 5, true);
		break;
	case BUTTON_MINUTE:
		screenUtils->drawButtonFrame(100, 130, 50, 40, 5, true);
		break;
	case BUTTON_SECOND:
		screenUtils->drawButtonFrame(170, 130, 50, 40, 5, true);
		break;
	}

	delay(100);

	switch (button) {
	case BUTTON_BACK:
		screenUtils->drawButtonFrame(45, 245, 70, 70, 5, false);
		break;
	case BUTTON_SAVE:
		screenUtils->drawButtonFrame(125, 245, 70, 70, 5, false);
		break;
	case BUTTON_YEAR:
		screenUtils->drawButtonFrame(5, 50, 90, 40, 5, false);
		break;
	case BUTTON_MONTH:
		screenUtils->drawButtonFrame(115, 50, 50, 40, 5, false);
		break;
	case BUTTON_DAY:
		screenUtils->drawButtonFrame(185, 50, 50, 40, 5, false);
		break;
	case BUTTON_HOUR:
		screenUtils->drawButtonFrame(30, 130, 50, 40, 5, false);
		break;
	case BUTTON_MINUTE:
		screenUtils->drawButtonFrame(100, 130, 50, 40, 5, false);
		break;
	case BUTTON_SECOND:
		screenUtils->drawButtonFrame(170, 130, 50, 40, 5, false);
		break;
	}
}

void CalendarScreen::incrementDateTime(int button)
{
	char buffer[2];
	uint16_t monthMaxDays = monthMax[month-1];

	tft->setTextColor(ILI9341_YELLOW, ILI9341_BLACK);

	switch (button) {
	case BUTTON_YEAR:
		if (year < 2050) {
			year++;
		} else {
			year = 2018;
		}

		tft->fillRect(10, 55, 80, 30, ILI9341_BLACK);

		tft->setCursor(12, 80);
		tft->print(year);
		break;
	case BUTTON_MONTH:
		if (month < 12) {
			month++;
		} else {
			month = 1;
		}

		tft->fillRect(120, 55, 40, 30, ILI9341_BLACK);

		tft->setCursor(123, 80);
		sprintf(buffer, "%02d", month);
		tft->print(buffer);
		break;
	case BUTTON_DAY:
		if (day < monthMaxDays) {
			day++;
		} else {
			day = 1;
		}

		tft->fillRect(190, 55, 40, 30, ILI9341_BLACK);

		tft->setCursor(192, 80);
		sprintf(buffer, "%02d", day);
		tft->print(buffer);
		break;
	case BUTTON_HOUR:
		if (hour < 23) {
			hour++;
		} else {
			hour = 0;
		}

		tft->fillRect(35, 135, 40, 30, ILI9341_BLACK);

		tft->setCursor(37, 160);
		sprintf(buffer, "%02d", hour);
		tft->print(buffer);
		break;
	case BUTTON_MINUTE:
		if (minute < 59) {
			minute++;
		} else {
			minute = 0;
		}

		tft->fillRect(105, 135, 40, 30, ILI9341_BLACK);

		tft->setCursor(107, 160);
		sprintf(buffer, "%02d", minute);
		tft->print(buffer);
		break;
	case BUTTON_SECOND:
		if (second < 59) {
			second++;
		} else {
			second = 0;
		}

		tft->fillRect(175, 135, 40, 30, ILI9341_BLACK);

		tft->setCursor(177, 160);
		sprintf(buffer, "%02d", second);
		tft->print(buffer);
		break;
	}
}
