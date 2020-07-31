#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "DateTimeCounter.h"

const char ZERO = '0';
const char SEPARATOR = '-';
const char COLON = ':';
const char SPACE = ' ';

DateTimeCounter::DateTimeCounter(Adafruit_ILI9341* adafruitTft)
{
	tft = adafruitTft;

	startX = 0;
	startY = 0;
	digitWidth = 0;
	digitHeight = 0;
}

DateTime* DateTimeCounter::current()
{
	return &currentDateTime;
}

void DateTimeCounter::init(DateTime* dateTime)
{
	currentDateTime = DateTime(*dateTime);

	//Set font specific height and width
	tft->setTextWrap(false);
	tft->setFont(&DroidSansMono8pt7b);
	tft->setTextColor(ILI9341_DARKCYAN, ILI9341_BLACK);

	digitWidth = 8;
	digitHeight = 12;
	startX = 30;
	startY = 260;

	updateAll(currentDateTime.year(), currentDateTime.month(), currentDateTime.day(),
			currentDateTime.hour(), currentDateTime.minute(), currentDateTime.second());
}

void DateTimeCounter::update(DateTime* newDateTime)
{
	if (newDateTime->unixtime() != currentDateTime.unixtime()) {

		tft->setFont(&DroidSansMono8pt7b);
		tft->setTextColor(ILI9341_DARKCYAN, ILI9341_BLACK);

		if (newDateTime->year() != currentDateTime.year()) {
			updateAll(newDateTime->year(), newDateTime->month(), newDateTime->day(),
								newDateTime->hour(), newDateTime->minute(), newDateTime->minute());
		}
		if (newDateTime->month() != currentDateTime.month()) {
			updateAll(newDateTime->year(), newDateTime->month(), newDateTime->day(),
								newDateTime->hour(), newDateTime->minute(), newDateTime->minute());
		}
		if (newDateTime->day() != currentDateTime.day()) {
			updateAll(newDateTime->year(), newDateTime->month(), newDateTime->day(),
								newDateTime->hour(), newDateTime->minute(), newDateTime->minute());
		}
		if (newDateTime->hour() != currentDateTime.hour()) {
			updateAll(newDateTime->year(), newDateTime->month(), newDateTime->day(),
								newDateTime->hour(), newDateTime->minute(), newDateTime->minute());
		}
		if (newDateTime->minute() != currentDateTime.minute()) {
			uint8_t to = newDateTime->minute();
			int16_t x = startX+16*digitWidth+2;
			int16_t y = startY;

			updateTwoDigits(to, x, y);
		}
		if (newDateTime->second() != currentDateTime.second()) {
			uint8_t to = newDateTime->second();
			int16_t x = startX+19*digitWidth+4;
			int16_t y = startY;

			updateTwoDigits(to, x, y);
		}
		currentDateTime = DateTime(*newDateTime);
	}
}

void DateTimeCounter::updateTwoDigits(uint8_t to, int16_t x, int16_t y)
{
	//Clear the digits
	tft->fillRect(x+1, y-digitHeight+1, digitWidth*2+3, digitHeight, ILI9341_BLACK);

	tft->setCursor(x, y);
	if (to < 10) {
		tft->print(ZERO);
	}
	tft->print(to);
}

void DateTimeCounter::updateAll(int16_t year, int16_t month, int16_t day, int16_t hour, int16_t minute, int16_t second)
{
	//Clear the whole text on the screen
	tft->fillRect(startX, startY-digitHeight+1, digitWidth*20+6, digitHeight, ILI9341_BLACK);

	tft->setCursor(startX, startY);
	tft->print(year);

	tft->setCursor(startX+5*digitWidth, startY);
	tft->print(SEPARATOR);

	tft->setCursor(startX+6*digitWidth+1, startY);
	if (month < 10) {
		tft->print(ZERO);
	}
	tft->print(month);

	tft->setCursor(startX+8*digitWidth+2, startY);
	tft->print(SEPARATOR);

	tft->setCursor(startX+9*digitWidth+2, startY);
	if (day < 10) {
		tft->print(ZERO);
	}
	tft->print(day);

	tft->setCursor(startX+13*digitWidth, startY);
	if (hour < 10) {
		tft->print(ZERO);
	}
	tft->print(hour);

	tft->setCursor(startX+15*digitWidth+2, startY);
	tft->print(COLON);

	tft->setCursor(startX+16*digitWidth+2, startY);
	if (minute < 10) {
		tft->print(ZERO);
	}
	tft->print(minute);

	tft->setCursor(startX+18*digitWidth+4, startY);
	tft->print(COLON);

	tft->setCursor(startX+19*digitWidth+4, startY);
	if (second < 10) {
		tft->print(ZERO);
	}
	tft->print(second);
}

