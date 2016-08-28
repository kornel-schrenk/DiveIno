#ifndef View_h
#define View_h

#include "Arduino.h"
#include "UTFT.h"

#include "Buhlmann.h"
#include "Logbook.h"
#include "Settings.h"

#define MENU_SCREEN 		0
#define DIVE_SCREEN 	    1
#define LOGBOOK_SCREEN      2
#define SURFACE_TIME_SCREEN 3
#define GAUGE_SCREEN     	4
#define SETTINGS_SCREEN     5
#define ABOUT_SCREEN        6
#define UI_TEST_SCREEN      7
#define PROFILE_SCREEN      8
#define DATETIME_SCREEN     9

// Position of first menu item from the top of the screen
#define MENU_TOP 60
#define SETTINGS_TOP 60

#define SETTINGS_SIZE 5
#define MENU_SIZE 6

class View {
public:
	View(UTFT* utft);

	void moveMenuSelection(byte selectedMenuItemIndex, byte menuItemIndex);

	void displayMenuScreen();
	void displayDiveScreen(float oxygenRateSetting);
	void displayLogbookScreen(LogbookData* logbookData);
	void displayProfileScreen(ProfileData* profileData, int profileNumber);
	void displaySurfaceTimeScreen(DiveResult* diveResult, unsigned long surfaceIntervalInMinutes, bool isDiveStopDisplay);
	void displayGaugeScreen(bool testModeSetting);
	void displaySettings(byte settingIndex, float seaLevelPressureSetting, float oxygenRateSetting,	bool testModeSetting, bool soundSetting, bool imperialUnitsSetting);
	void displaySettingsScreen(byte selectionIndex, float seaLevelPressureSetting, float oxygenRateSetting, bool testModeSetting, bool soundSetting, bool imperialUnitsSetting);
	void displayAboutScreen();
	void displayTestScreen();
	void displayDateTimeSettingScreen(byte settingIndex, DateTimeSettings* dateTimeSettings);
	void displayDateTimeSettings(byte settingIndex, DateTimeSettings* dateTimeSettings);

	void drawDepth(float depth);
	void drawMaximumDepth(float maximumDepth);
	void drawCurrentTemperature(float currentTemperature);
	void drawCurrentPressure(int currentPressure);
	void drawDiveDuration(unsigned long duration);
	void drawCurrentTime(String time);
	void drawOxigenPercentage(float oxigenPercentage);
	void drawPartialPressureWarning();
	void drawDecoArea(DiveInfo diveInfo);
	void drawSafetyStop(unsigned int safetyStopDurationInSeconds);
	void drawAscend(int ascendRate);

	void drawBatteryStateOfCharge(float soc);

private:
	UTFT* tft;
};

#endif
