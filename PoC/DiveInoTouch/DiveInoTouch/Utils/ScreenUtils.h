#ifndef SCREENUTILS_H_
#define SCREENUTILS_H_

#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <SdFat.h>

#include "../Fonts/DroidSansMono8pt.h"
#include "../Fonts/DroidSansMono12pt.h"
#include "../Fonts/DroidSansMono16pt.h"

#include "../Fonts/DjbDigital8pt.h"
#include "../Fonts/DjbDigital12pt.h"
#include "../Fonts/DjbDigital16pt.h"
#include "../Fonts/DjbDigital24pt.h"
#include "../Fonts/DjbDigital32pt.h"
#include "../Fonts/DjbDigital48pt.h"

#define NO_ACTION   42
#define GO_HOME     43
#define GO_DIVE     44
#define GO_GAUGE    45
#define GO_LOGBOOK  46
#define GO_SURFACE  47
#define GO_SETTINGS 48
#define GO_ABOUT    49
#define GO_CALENDAR 50

#define SCREEN_HOME     210
#define SCREEN_ABOUT    220
#define SCREEN_SETTINGS 230
#define SCREEN_CALENDAR 240
#define SCREEN_GAUGE    260
#define SCREEN_DIVE     270

extern SdFat SD;

class ScreenUtils {
public:
	ScreenUtils(Adafruit_ILI9341* adafruitTft);

	void drawButtonFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, bool selected);
	void drawBmpFile(char *filename, int16_t x, int16_t y);
	void drawBatteryStateOfCharge(float soc);

private:
	Adafruit_ILI9341* tft;

	uint16_t read16(SdFile &f);
	uint32_t read32(SdFile &f);
};

#endif /* SCREENUTILS_H_ */
