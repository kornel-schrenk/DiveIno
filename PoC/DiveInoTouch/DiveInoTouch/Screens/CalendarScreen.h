#ifndef CALENDARSCREEN_H_
#define CALENDARSCREEN_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support

#include "..\Utils\ScreenUtils.h"

#define BUTTON_BACK   12
#define BUTTON_SAVE   13

#define BUTTON_YEAR   14
#define BUTTON_MONTH  15
#define BUTTON_DAY    16
#define BUTTON_HOUR   17
#define BUTTON_MINUTE 18
#define BUTTON_SECOND 19

extern RTC_DS1307 rtc;

class CalendarScreen {
public:
	CalendarScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils);

	void open();
	int handleTouch(Adafruit_FT6206* touchScreen);

private:
	Adafruit_ILI9341* tft;
	ScreenUtils* screenUtils;

	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t hour;
	uint16_t minute;
	uint16_t second;

	void drawButtonPress(int button);
	void incrementDateTime(int button);
};

#endif /* CALENDARSCREEN_H_ */
