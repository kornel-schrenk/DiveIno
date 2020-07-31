#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_FT6206.h>  // Touch screen support

#include "..\Utils\ScreenUtils.h"

#define BUTTON_BACK     12
#define BUTTON_SAVE     13
#define BUTTON_CALENDAR 14
#define BUTTON_UNITS    15
#define BUTTON_SOUND    16
#define BUTTON_PRESSURE 17
#define BUTTON_SYNC     18
#define BUTTON_OXYGEN   19

extern SdFat SD;

struct DiveInoSettings {
	float seaLevelPressureSetting = 1013;
	float oxygenRateSetting = 21;
	bool soundSetting = true;
	bool imperialUnitsSetting = false;
};

class SettingsScreen {
public:
	SettingsScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils);

	void open();
	int handleTouch(Adafruit_FT6206* touchScreen);

private:
	Adafruit_ILI9341* tft;
	ScreenUtils* screenUtils;

	DiveInoSettings* settings;

	void drawButtonPress(int button);
};
