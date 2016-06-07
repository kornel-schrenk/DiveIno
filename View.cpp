#include "Arduino.h"
#include "View.h"

extern uint8_t Grotesk16x32[];             //16x32 pixel
extern uint8_t SevenSegNumFontPlusPlus[];  //32x50 pixel
extern uint8_t BigFont[];                  //16x16 pixel
extern uint8_t SevenSeg_XXXL[];            //64x100 pixel

char* mainMenu[] = {" DiveIno - Main Menu ",
        " Dive          ",
        " Logbook       ",
		" Surface Time  ",
		" Gauge         ",
        " Settings      ",
        " About         "};

char* settingsList[] = {" Sea level",
		" Oxygen %",
		" Test mode",
		" Sound",
		" Units"};

char* dateTimeSettingsList[] = {" Year ",
		" Month",
		" Day",
		" Hour",
		" Minute"};

View::View(UTFT* utft) {
	tft = utft;
}

void View::moveMenuSelection(byte selectedMenuItemIndex, byte menuItemIndex)
{
	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_YELLOW);

	// Remove the highlight from the currently selected menu item
	tft->setBackColor(VGA_BLACK);
	tft->print(mainMenu[selectedMenuItemIndex], 0, ((selectedMenuItemIndex - 1) * 40) + MENU_TOP);

	// Highlight the new selected menu item
	tft->setBackColor(VGA_BLUE);
	tft->print(mainMenu[menuItemIndex], 0, ((menuItemIndex - 1) * 40) + MENU_TOP);
}

void View::displayMenuScreen()
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print(mainMenu[0], 48, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);

	// Display other menu items
	tft->setColor(VGA_YELLOW);
	for (int i = 1; i <= MENU_SIZE; i++) {
		tft->print(mainMenu[i], 0, ((i - 1) * 40) + MENU_TOP);
	}
}

void View::displayDiveScreen(float oxygenRateSetting)
{
	tft->clrScr();

	tft->setColor(VGA_SILVER);
	tft->setBackColor(VGA_BLACK);
	tft->drawRect(0, 0, tft->getDisplayXSize()-1, tft->getDisplayYSize()-1);

	tft->drawLine(0, 190, tft->getDisplayXSize()-1, 190);
	tft->drawLine(220, 0, 220, 190);

	drawOxigenPercentage(oxygenRateSetting * 100);
}

void View::displayLogbookScreen(LogbookData* logbookData)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print("DiveIno - Logbook", 64, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);

	tft->setColor(VGA_WHITE);
	tft->print("Dives no:", 20, 80);
	tft->print("Duration:", 20, 120);
	tft->print("Max depth:", 20, 160);
	tft->print("Last dive:", 20, 200);
	tft->print("Profiles:", 20, 240);

	int paddingLeft = 200;

	tft->setColor(VGA_AQUA);
	tft->printNumI(logbookData->totalNumberOfDives, paddingLeft, 80);
	tft->setColor(VGA_YELLOW);
	if (logbookData->totalDiveHours < 10) {
		tft->print("00", paddingLeft, 120);
		tft->printNumI(logbookData->totalDiveHours, paddingLeft + 32, 120);
	} else if (logbookData->totalDiveHours < 100) {
		tft->print("0", paddingLeft, 120);
		tft->printNumI(logbookData->totalDiveHours, paddingLeft + 16, 120);
	} else {
		tft->printNumI(logbookData->totalDiveHours, paddingLeft, 120);
	}
	tft->print(":", paddingLeft + 48, 120);
	if (logbookData->totalDiveMinutes < 10) {
		tft->print("0", paddingLeft + 64, 120);
		tft->printNumI(logbookData->totalDiveMinutes, paddingLeft + 80, 120);
	} else {
		tft->printNumI(logbookData->totalDiveMinutes, paddingLeft + 64, 120);
	}
	tft->setColor(VGA_PURPLE);
	tft->printNumF(logbookData->totalMaximumDepth, 1, paddingLeft, 160);
	tft->setColor(VGA_TEAL);
	tft->print(logbookData->lastDiveDateTime, paddingLeft, 200);
	tft->setColor(VGA_LIME);
	tft->printNumI(logbookData->numberOfStoredProfiles, paddingLeft, 240);

	tft->setColor(VGA_WHITE);
	tft->setFont(BigFont);
	tft->print("hh:mm", 300, 134);
	tft->print("m", 270, 174);
}

void View::displayProfileScreen(ProfileData* profileData, int profileNumber)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setColor(VGA_SILVER);
	tft->drawRect(0, 0, tft->getDisplayXSize()-1, tft->getDisplayYSize()-1);
	tft->drawLine(0, 150, tft->getDisplayXSize()-1, 150);

	tft->setColor(VGA_LIME);
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->printNumI(profileNumber, 25, 52, 3, '0');

	int paddingLeft = 150;

	tft->setColor(VGA_TEAL);
	tft->setFont(Grotesk16x32);
	tft->print(profileData->diveDate, paddingLeft, 20);
	tft->print(profileData->diveTime, paddingLeft+176, 20);
	tft->setColor(VGA_PURPLE);
	tft->printNumF(profileData->maximumDepth, 1, paddingLeft, 60);
	tft->setColor(VGA_YELLOW);
	tft->printNumI(profileData->diveDuration/60, 280, 60, 3);
	tft->print(":", 328, 60);
	tft->printNumI(profileData->diveDuration%60, 344, 60, 2 , '0');
	tft->setColor(VGA_BLUE);
	tft->printNumF(profileData->minimumTemperature, 1, paddingLeft, 100);
	tft->setColor(VGA_OLIVE);
	tft->printNumF(profileData->oxigenPercentage, 1, 296, 100);

	tft->setColor(VGA_WHITE);
	tft->setFont(BigFont);
	tft->print("m", paddingLeft + 68, 74);
	tft->print("mm:ss", 380, 74);
	tft->print("cel", paddingLeft + 68, 114);
	tft->print("O2%", 364, 114);
}

void View::displaySurfaceTimeScreen(DiveResult* diveResult, unsigned long surfaceIntervalInMinutes, bool isDiveStopDisplay)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	if (isDiveStopDisplay) {
		tft->print("DiveIno - Dive time", 64, 10);
	} else {
		tft->print("DiveIno - Surface time", 64, 10);
	}

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);

	if (diveResult != NULL) {

		if (isDiveStopDisplay) {
			tft->setColor(VGA_RED);
			tft->printNumI(diveResult->noFlyTimeInMinutes/60, 220, 70, 2, '0');
			tft->print(":", 252, 70);
			tft->printNumI(diveResult->noFlyTimeInMinutes%60, 268, 70, 2 , '0');
			tft->setColor(VGA_YELLOW);
			tft->printNumI(diveResult->durationInSeconds/60, 204, 110, 3, ' ');
			tft->print(":", 252, 110);
			tft->printNumI(diveResult->durationInSeconds%60, 268, 110, 2 , '0');
			tft->setColor(VGA_FUCHSIA);
			tft->printNumF(diveResult->maxDepthInMeters, 1, 220, 150);

			tft->setColor(VGA_WHITE);
			tft->print("No fly:", 20, 70);
			tft->print("Duration:", 20, 110);
			tft->print("Max depth:", 20, 150);
			tft->setFont(BigFont);
			tft->print("hh:mm", 310, 83);
			tft->print("mm:ss", 310, 123);
			tft->print("m", 310, 163);
		} else {
			tft->setColor(VGA_RED);
			tft->printNumI(diveResult->noFlyTimeInMinutes/60, 220, 70, 2, '0');
			tft->print(":", 252, 70);
			tft->printNumI(diveResult->noFlyTimeInMinutes%60, 268, 70, 2 , '0');
			tft->setColor(VGA_GREEN);
			if (surfaceIntervalInMinutes == 0 || surfaceIntervalInMinutes > 2880) {
				//After 48 hours
				tft->print("00:00", 220, 110);
			} else {
				//Within 48 hours
				tft->printNumI(surfaceIntervalInMinutes/60, 220, 110, 2, '0');
				tft->print(":", 252, 110);
				tft->printNumI(surfaceIntervalInMinutes%60, 268, 110, 2 , '0');
			}

			tft->setColor(VGA_WHITE);
			tft->print("No fly:", 20, 70);
			tft->print("Duration:", 20, 110);
			tft->setFont(BigFont);
			tft->print("hh:mm", 310, 83);
			tft->print("hh:mm", 310, 123);
		}

		//Draw the remaining compartment ppN2 values

		float max = diveResult->compartmentPartialPressures[0];
		for (int i=0; i < COMPARTMENT_COUNT; i++) {
			if (max < diveResult->compartmentPartialPressures[i]) {
				max = diveResult->compartmentPartialPressures[i];
			}
		}

		tft->setColor(VGA_AQUA);
		for (int i=0; i < COMPARTMENT_COUNT; i++) {
			int x1 = i*30+13;
			int y1 = tft->getDisplayYSize()-((130/(max-755))*(diveResult->compartmentPartialPressures[i]-755));
			int x2 = (i+1)*30+10;
			int y2 = tft->getDisplayYSize()-1;
			tft->fillRect(x1, y1, x2, y2);
		}
	} else {
		tft->setColor(VGA_RED);
		tft->print("N/A", 220, 70);
		tft->setColor(VGA_PURPLE);
		tft->print("N/A", 220, 110);
		tft->setColor(VGA_YELLOW);
		tft->print("N/A", 220, 150);
	}
}

void View::displayGaugeScreen(bool testModeSetting)
{
	tft->clrScr();
	tft->setColor(VGA_SILVER);
	tft->setBackColor(VGA_BLACK);
	tft->drawRect(0, 0, tft->getDisplayXSize()-1, tft->getDisplayYSize()-1);

	tft->drawLine(0, 190, tft->getDisplayXSize()-1, 190);
	tft->drawLine(220, 0, 220, 190);

	int paddingLeft = 50;

	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_GREEN);
	tft->print("GAUGE", paddingLeft + tft->getFontXsize() * 4, 215);

	if (testModeSetting) {
		tft->setColor(VGA_RED);
		tft->print("TEST", paddingLeft + tft->getFontXsize() * 15, 215);
	}
}

void View::displaySettingsScreen(byte selectionIndex, float seaLevelPressureSetting, float oxygenRateSetting, bool testModeSetting, bool soundSetting, bool imperialUnitsSetting)
{
	// Settings:
	//
	// From SETTINGS.TXT:
	//  seaLevelPressure = 1013.25
	//  oxygenRate = 0.21
	//  testMode = 1 :  0 = Off, 1 = On
	//  sound = 1 : 0 = Off, 1 = On
	//  units = 0 : 0 = metric, 1 = imperial
	//
	// Real Time Clock settings - for this a separate application can be used:
	//  time = 18:05
	//  date = 2015-11-14

	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the screen
	tft->setColor(VGA_LIME);
	tft->print("DiveIno - Settings", 64, 10);

	// Draw separation line
	tft->drawLine(0, SETTINGS_TOP-10, tft->getDisplayXSize()-1, SETTINGS_TOP-10);

	// Display settings
	tft->setColor(VGA_YELLOW);
	for (int i = 0; i < SETTINGS_SIZE; i++) {
		tft->print(settingsList[i], 0, (i * 40) + SETTINGS_TOP);
	}

	displaySettings(selectionIndex, seaLevelPressureSetting, oxygenRateSetting, testModeSetting, soundSetting, imperialUnitsSetting);
}

void View::displaySettings(byte settingIndex, float seaLevelPressureSetting, float oxygenRateSetting, bool testModeSetting, bool soundSetting, bool imperialUnitsSetting)
{
	if (settingIndex == 0) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumF(seaLevelPressureSetting, 2, 240, SETTINGS_TOP);

	if (settingIndex == 1) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumF(oxygenRateSetting, 2, 240, 40 + SETTINGS_TOP);

	if (settingIndex == 2) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	if (testModeSetting) {
		tft->print("On ", 240, 80 + SETTINGS_TOP);
	} else {
		tft->print("Off", 240, 80 + SETTINGS_TOP);
	}

	if (settingIndex == 3) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	if (soundSetting) {
		tft->print("On ", 240, 120 + SETTINGS_TOP);
	} else {
		tft->print("Off", 240, 120 + SETTINGS_TOP);
	}

	if (settingIndex == 4) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	if (imperialUnitsSetting) {
		tft->print("Imperial", 240, 160 + SETTINGS_TOP);
	} else {
		tft->print("Metric  ", 240, 160 + SETTINGS_TOP);
	}

	if (settingIndex == 5) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_GREEN);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Save", 10, 211 + SETTINGS_TOP);

	if (settingIndex == 6) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_RED);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Cancel", 90, 211 + SETTINGS_TOP);

	if (settingIndex == 7) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_BLUE);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Default", 200, 210 + SETTINGS_TOP);

	if (settingIndex == 8) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_FUCHSIA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Date&Time", 330, 210 + SETTINGS_TOP);
}

void View::displayDateTimeSettingScreen(byte settingIndex, DateTimeSettings* dateTimeSettings)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the screen
	tft->setColor(VGA_LIME);
	tft->print("DiveIno - Date & Time", 64, 10);

	// Draw separation line
	tft->drawLine(0, SETTINGS_TOP-10, tft->getDisplayXSize()-1, SETTINGS_TOP-10);

	// Display settings
	tft->setColor(VGA_YELLOW);
	for (int i = 0; i < SETTINGS_SIZE; i++) {
		tft->print(dateTimeSettingsList[i], 0, (i * 40) + SETTINGS_TOP);
	}

	displayDateTimeSettings(settingIndex, dateTimeSettings);
}

void View::displayDateTimeSettings(byte settingIndex, DateTimeSettings* dateTimeSettings)
{
	if (settingIndex == 0) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumI(dateTimeSettings->year, 160, SETTINGS_TOP, 4);

	if (settingIndex == 1) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumI(dateTimeSettings->month, 160, 40 + SETTINGS_TOP, 2, '0');

	if (settingIndex == 2) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumI(dateTimeSettings->day, 160, 80 + SETTINGS_TOP, 2, '0');

	if (settingIndex == 3) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumI(dateTimeSettings->hour, 160, 120 + SETTINGS_TOP, 2, '0');

	if (settingIndex == 4) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumI(dateTimeSettings->minute, 160, 160 + SETTINGS_TOP, 2, '0');

	if (settingIndex == 5) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_GREEN);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Save", 120, 210 + SETTINGS_TOP);

	if (settingIndex == 6) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_RED);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Cancel", 210, 210 + SETTINGS_TOP);
}

void View::displayAboutScreen()
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print("DiveIno - About", 64, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, 479, MENU_TOP-10);	tft->setFont(Grotesk16x32);

	tft->setColor(VGA_WHITE);
	tft->print("Version: 0.9.7", CENTER, 140);
	tft->setColor(VGA_GREEN);
	tft->print("www.diveino.hu", CENTER, 180);
}

void View::displayTestScreen()
{
	displayDiveScreen(0.21);

	drawBatteryStateOfCharge(86.5);

	drawCurrentPressure(2345.67);
	drawCurrentTemperature(22.4);
	drawMaximumDepth(36.5);

	drawDepth(24.3);
	drawDiveDuration(2048);

	DiveInfo diveInfo;
//	diveInfo.decoNeeded = false;
//	diveInfo.minutesToDeco = 46;
//	drawDecoArea(diveInfo);

	diveInfo.decoNeeded = true;
    diveInfo.decoStopInMeters = 24;
    diveInfo.decoStopDurationInMinutes = 367;
	drawDecoArea(diveInfo);

	drawSafetyStop(134);
	drawAscend(ASCEND_NORMAL);

	//drawOxigenPercentage(21);
	//drawPartialPressureWarning();
}

void View::drawBatteryStateOfCharge(float soc)
{
	tft->setFont(BigFont); //16x16 pixel
	tft->setBackColor(VGA_BLACK);

	if (100 <= soc && soc < 125) {
		tft->setColor(VGA_AQUA);
	} else if (75 <= soc && soc < 100) {
		tft->setColor(VGA_GREEN);
	} else if (50 <= soc && soc < 75) {
		tft->setColor(VGA_LIME);
	} else if (25 <= soc && soc < 50) {
		tft->setColor(VGA_YELLOW);
	} else if (soc < 25) {
		tft->setColor(VGA_RED);
	} else {
		return;
	}

	tft->fillRect(401, 277, 410, 288);
	tft->drawRect(410, 270, 470, 295);
	tft->drawRect(411, 269, 469, 294);

	if (100 <= soc && soc < 125) {
		tft->print("OOO", 420, 275);
	} else if (soc < 100) {
		tft->printNumI(soc, 420, 275, 2, ' ');
		tft->print("%", 452, 275);
	}
}

void View::printBatteryData(float volt, float soc)
{
	tft->setFont(BigFont); //16x16 pixel

	tft->setColor(VGA_WHITE);
	tft->printNumI(soc, 420, 225, 3, ' ');

	tft->setColor(VGA_FUCHSIA);
	tft->printNumF(volt, 2, 420, 245, '.', 3);
}

void View::drawDepth(float depth)
{
	tft->setFont(SevenSeg_XXXL);
	tft->setColor(VGA_YELLOW);

	tft->printNumI(depth, 240, 15, 2, '/');           //Two digits - the / is the SPACE in the font
	tft->fillRect(370, 105, 378, 113);	              // Draw the decimal point
	tft->printNumI(((int)(depth*10))%10, 380, 15, 1); // Draw the first decimal fraction digit

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("m", 444, 98);
}

void View::drawMaximumDepth(float maximumDepth)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_PURPLE);
	tft->printNumF(maximumDepth, 1, 10, 130, '.', 4, '/');

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("m", 143, 164);
}

void View::drawCurrentTemperature(float currentTemperature)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_LIME);
	tft->printNumF(currentTemperature, 1, 10, 10, '.', 4, '/'); //the / is the SPACE in the font

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("cel", 143, 44);
}

void View::drawCurrentPressure(int currentPressure)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_AQUA);
	tft->printNumI(currentPressure, 10, 70, 4, '/');

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("mBar", 143, 104);
}

void View::drawDiveDuration(int duration) // The dive duration is always in seconds
{
	int hours = duration / 3600;
	duration = duration % 3600;
	int minutes = duration / 60;
	int seconds = duration % 60;

	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_WHITE);

	tft->printNumI(hours, 240, 130, 1); // Max 9 hours
	tft->print(":", 272, 130, 1);
	tft->printNumI(minutes, 304, 130, 2, '0');
	tft->print(":", 368, 130, 1);
	tft->printNumI(seconds, 400, 130, 2, '0');
}

void View::drawCurrentTime(String time)
{
	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_TEAL);
	tft->print(time, 55, 265);
}

void View::drawOxigenPercentage(float oxigenPercentage)
{
	tft->setFont(BigFont);

	if (oxigenPercentage <= 23) {
		tft->setColor(VGA_GREEN);
	} else {
		tft->setColor(VGA_MAROON);
	}
	tft->printNumI(oxigenPercentage, 338, 217, 2, ' ');

	tft->setColor(VGA_WHITE);
	tft->print("%", 370, 217);
}

void View::drawPartialPressureWarning()
{
	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_MAROON);
	tft->print("PPO2", 10, 269); // Overrides the stay in the safety stop text
}

void View::drawDecoArea(DiveInfo diveInfo)
{
	if (!diveInfo.decoNeeded) {

		tft->setFont(Grotesk16x32);
		tft->setColor(VGA_WHITE);
		tft->print("Safe", 10, 209);

		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_YELLOW);
		tft->printNumI(diveInfo.minutesToDeco, 90, 200, 2, '0');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print("min", 159, 234);
	} else {

		tft->setFont(Grotesk16x32);
		tft->setColor(VGA_RED);
		tft->print("Deco", 10, 209);

		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_LIME);
		if (diveInfo.decoStopDurationInMinutes < 100) {
			tft->printNumI(diveInfo.decoStopDurationInMinutes, 90, 200, 2, '/');
		} else {
			tft->printNumI(99, 90, 200, 2, '/');
		}
		tft->setColor(VGA_FUCHSIA);
		tft->printNumI(diveInfo.decoStopInMeters, 218, 200, 2, '/');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print("m", 287, 234);
		tft->print("min", 159, 234);
	}
}

void View::drawSafetyStop(unsigned int safetyStopDurationInSeconds)
{
	tft->setFont(Grotesk16x32);

	if (safetyStopDurationInSeconds < 180) {
		tft->setColor(VGA_BLUE);
		tft->print("Stay", 10, 269);
	} else {
		tft->setColor(VGA_GREEN);
		tft->print("Done", 10, 269);
	}

	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_AQUA);

	unsigned int duration = 0;
	if (safetyStopDurationInSeconds <= 180) {
		duration = 180 - safetyStopDurationInSeconds; // Counting backwards
	}

	int minutes = duration / 60;
	int seconds = duration % 60;

	tft->printNumI(minutes, 90, 260, 2, '0');
	tft->print(":", 154, 260, 1);
	tft->printNumI(seconds, 186, 260, 2, '0');
}

void View::drawAscend(int ascendRate)
{

	tft->setFont(Grotesk16x32);
	switch (ascendRate) {
		case ASCEND_OK:
			tft->setColor(VGA_GREEN);
			tft->print("----", 400, 210);
			break;
		case ASCEND_SLOW:
			tft->setColor(VGA_WHITE);
			tft->print("---|", 400, 210);
			break;
		case ASCEND_NORMAL:
			tft->setColor(VGA_YELLOW);
			tft->print("--||", 400, 210);
			break;
		case ASCEND_ATTENTION:
			tft->setColor(VGA_MAROON);
			tft->print("-|||", 400, 210);
			break;
		case ASCEND_CRITICAL:
			tft->setColor(VGA_RED);
			tft->print("||||", 400, 210);
			break;
		case ASCEND_DANGER:
			tft->setColor(VGA_RED);
			tft->print("SLOW", 400, 210);
			break;
		default:
			tft->setColor(VGA_GREEN);
			tft->print("<<--", 400, 210);
	}
}



