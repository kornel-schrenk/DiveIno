#ifndef DATETIMECOUNTER_H_
#define DATETIMECOUNTER_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support
#include <RTClib.h>

#include "..\Utils\ScreenUtils.h"

class DateTimeCounter {
public:
	DateTimeCounter(Adafruit_ILI9341* adafruitTft);

	void init(DateTime* dateTime);
	void update(DateTime* newDateTime);
	DateTime* current();
private:
	DateTime currentDateTime;
	Adafruit_ILI9341* tft;

	int16_t startX, startY = 0;
	uint8_t digitWidth, digitHeight = 0;

	void updateTwoDigits(uint8_t to, int16_t x, int16_t y);
	void updateAll(int16_t year, int16_t month, int16_t day, int16_t hour, int16_t minute, int16_t second);
};
#endif /* DATETIMECOUNTER_H_ */
