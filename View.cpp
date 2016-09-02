#include "Arduino.h"
#include "View.h"

extern uint8_t Grotesk16x32[];             //16x32 pixel
extern uint8_t SevenSegNumFontPlusPlus[];  //32x50 pixel
extern uint8_t BigFont[];                  //16x16 pixel
extern uint8_t SevenSeg_XXXL[];            //64x100 pixel

#if defined(__SAM3X8E__) || defined(__SAM3X8H__)
//Main menu images
extern unsigned short Dive[4096];
extern unsigned short Gauge[4096];
extern unsigned short Surface[4096];
extern unsigned short Logbook[4096];
extern unsigned short Settings[4096];
extern unsigned short About[4096];
#endif

char* mainMenu[] = {" DiveIno - Main Menu ",
        " Dive          ",
        " Logbook       ",
		" Surface Time  ",
		" Gauge         ",
        " Settings      ",
        " About         "};

char* settingsList[] = {" Sea level",
		" Oxygen %",
		" Sound",
		" Units"};

char* dateTimeSettingsList[] = {" Year ",
		" Month",
		" Day",
		" Hour",
		" Minute"};

View::View(UTFT* utft, UTFT_SdRaw* sdFatFiles) {
	tft = utft;
	sdFiles = sdFatFiles;
}

void View::moveMenuSelection(byte selectedMenuItemIndex, byte menuItemIndex)
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_YELLOW);

	// Remove the highlight from the currently selected menu item
	tft->setBackColor(VGA_BLACK);
	tft->print(mainMenu[selectedMenuItemIndex], 0, ((selectedMenuItemIndex - 1) * 40) + MENU_TOP);

	// Highlight the new selected menu item
	tft->setBackColor(VGA_BLUE);
	tft->print(mainMenu[menuItemIndex], 0, ((menuItemIndex - 1) * 40) + MENU_TOP);

#elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
	switch (selectedMenuItemIndex) {
		case 1: {
			tft->setColor(VGA_TEAL);
			tft->fillRoundRect(70, 90, 160, 180);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(80, 100, 150, 170);
			tft->drawBitmap(83, 103, 64, 64, Dive);
		} break;
		case 2: {
			tft->setColor(VGA_TEAL);
			tft->fillRoundRect(190, 90, 280, 180);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(200, 100, 270, 170);
			tft->drawBitmap(203, 103, 64, 64, Logbook);
		} break;
		case 3: {
			tft->setColor(VGA_TEAL);
			tft->fillRoundRect(310, 90, 400, 180);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(320, 100, 390, 170);
			tft->drawBitmap(323, 103, 64, 64, Surface);
		} break;
		case 4: {
			tft->setColor(VGA_TEAL);
			tft->fillRoundRect(70, 210, 160, 300);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(80, 220, 150, 290);
			tft->drawBitmap(83, 223, 64, 64, Gauge);
		} break;
		case 5: {
			tft->setColor(VGA_TEAL);
			tft->fillRoundRect(190, 210, 280, 300);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(200, 220, 270, 290);
			tft->drawBitmap(203, 223, 64, 64, Settings);
		} break;
		case 6: {
			tft->setColor(VGA_TEAL);
			tft->fillRoundRect(310, 210, 400, 300);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(320, 220, 390, 290);
			tft->drawBitmap(323, 223, 64, 64, About);
		} break;
	}

	switch (menuItemIndex) {
		case 1: {
			tft->setColor(VGA_YELLOW);
			tft->fillRoundRect(70, 90, 160, 180);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(80, 100, 150, 170);
			tft->drawBitmap(83, 103, 64, 64, Dive);
		} break;
		case 2: {
			tft->setColor(VGA_YELLOW);
			tft->fillRoundRect(190, 90, 280, 180);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(200, 100, 270, 170);
			tft->drawBitmap(203, 103, 64, 64, Logbook);
		} break;
		case 3: {
			tft->setColor(VGA_YELLOW);
			tft->fillRoundRect(310, 90, 400, 180);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(320, 100, 390, 170);
			tft->drawBitmap(323, 103, 64, 64, Surface);
		} break;
		case 4: {
			tft->setColor(VGA_YELLOW);
			tft->fillRoundRect(70, 210, 160, 300);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(80, 220, 150, 290);
			tft->drawBitmap(83, 223, 64, 64, Gauge);
		} break;
		case 5: {
			tft->setColor(VGA_YELLOW);
			tft->fillRoundRect(190, 210, 280, 300);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(200, 220, 270, 290);
			tft->drawBitmap(203, 223, 64, 64, Settings);
		} break;
		case 6: {
			tft->setColor(VGA_YELLOW);
			tft->fillRoundRect(310, 210, 400, 300);
			tft->setColor(VGA_BLACK);
			tft->fillRoundRect(320, 220, 390, 290);
			tft->drawBitmap(323, 223, 64, 64, About);
		} break;
	}
#endif
}

void View::displayMenuScreen()
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print(mainMenu[0], 68, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	//Display other menu items
	tft->setColor(VGA_YELLOW);
	for (int i = 1; i <= MENU_SIZE; i++) {
		tft->print(mainMenu[i], 0, ((i - 1) * 40) + MENU_TOP);
	}
#elif defined(__SAM3X8E__) || defined(__SAM3X8H__)

	tft->setColor(VGA_TEAL);
	tft->fillRoundRect(70, 90, 160, 180);
	tft->fillRoundRect(190, 90, 280, 180);
	tft->fillRoundRect(310, 90, 400, 180);

	tft->fillRoundRect(70, 210, 160, 300);
	tft->fillRoundRect(190, 210, 280, 300);
	tft->fillRoundRect(310, 210, 400, 300);

	tft->setColor(VGA_BLACK);
	tft->fillRoundRect(80, 100, 150, 170);
	tft->fillRoundRect(200, 100, 270, 170);
	tft->fillRoundRect(320, 100, 390, 170);

	tft->fillRoundRect(80, 220, 150, 290);
	tft->fillRoundRect(200, 220, 270, 290);
	tft->fillRoundRect(320, 220, 390, 290);

	tft->drawBitmap(83, 103, 64, 64, Dive);
	tft->drawBitmap(203, 103, 64, 64, Logbook);
	tft->drawBitmap(323, 103, 64, 64, Surface);

	tft->drawBitmap(83, 223, 64, 64, Gauge);
	tft->drawBitmap(203, 223, 64, 64, Settings);
	tft->drawBitmap(323, 223, 64, 64, About);
#endif
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

void View::displayLogbookScreen(LogbookData* logbookData, bool isImperial)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print(F("DiveIno - Logbook"), 64, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);

	tft->setColor(VGA_WHITE);
	tft->print(F("Dives no:"), 20, 80);
	tft->print(F("Duration:"), 20, 120);
	tft->print(F("Max depth:"), 20, 160);
	tft->print(F("Last dive:"), 20, 200);
	tft->print(F("Profiles:"), 20, 240);

	int paddingLeft = 200;

	tft->setColor(VGA_AQUA);
	tft->printNumI(logbookData->totalNumberOfDives, paddingLeft, 80);
	tft->setColor(VGA_YELLOW);
	if (logbookData->totalDiveHours < 10) {
		tft->print(F("00"), paddingLeft, 120);
		tft->printNumI(logbookData->totalDiveHours, paddingLeft + 32, 120);
	} else if (logbookData->totalDiveHours < 100) {
		tft->print(F("0"), paddingLeft, 120);
		tft->printNumI(logbookData->totalDiveHours, paddingLeft + 16, 120);
	} else {
		tft->printNumI(logbookData->totalDiveHours, paddingLeft, 120);
	}
	tft->print(F(":"), paddingLeft + 48, 120);
	if (logbookData->totalDiveMinutes < 10) {
		tft->print(F("0"), paddingLeft + 64, 120);
		tft->printNumI(logbookData->totalDiveMinutes, paddingLeft + 80, 120);
	} else {
		tft->printNumI(logbookData->totalDiveMinutes, paddingLeft + 64, 120);
	}
	tft->setColor(VGA_FUCHSIA);
	if (isImperial) {
		tft->printNumI(logbookData->totalMaximumDepth*3.28, paddingLeft, 160, 3);
	} else {
		tft->printNumF(logbookData->totalMaximumDepth, 1, paddingLeft, 160);
	}
	tft->setColor(VGA_TEAL);
	tft->print(logbookData->lastDiveDateTime, paddingLeft, 200);
	tft->setColor(VGA_LIME);
	tft->printNumI(logbookData->numberOfStoredProfiles, paddingLeft, 240);

	tft->setColor(VGA_WHITE);
	tft->setFont(BigFont);
	tft->print(F("hh:mm"), 300, 134);
	if (isImperial) {
		tft->print(F("ft"), 254, 174);
	} else {
		tft->print(F("m"), 270, 174);
	}
}

void View::displayProfileScreen(ProfileData* profileData, int profileNumber, bool isImperial)
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
	tft->setColor(VGA_YELLOW);
	tft->printNumI(profileData->diveDuration/60, 280, 60, 3);
	tft->print(":", 328, 60);
	tft->printNumI(profileData->diveDuration%60, 344, 60, 2 , '0');
	tft->setColor(VGA_OLIVE);
	tft->printNumF(profileData->oxigenPercentage, 1, 296, 100);
	if (isImperial) {
		tft->setColor(VGA_FUCHSIA);
		tft->printNumI(profileData->maximumDepth*3.28, paddingLeft, 60, 3);
		tft->setColor(VGA_BLUE);
		tft->printNumI((profileData->minimumTemperature*1.8)+32, paddingLeft, 100, 3);
	} else {
		tft->setColor(VGA_FUCHSIA);
		tft->printNumF(profileData->maximumDepth, 1, paddingLeft, 60);
		tft->setColor(VGA_BLUE);
		tft->printNumF(profileData->minimumTemperature, 1, paddingLeft, 100);
	}

	tft->setColor(VGA_WHITE);
	tft->setFont(BigFont);
	tft->print(F("mm:ss"), 380, 74);
	tft->print(F("O2%"), 364, 114);
	if (isImperial) {
		tft->print(F("ft"), paddingLeft + 52, 74);
		tft->print(F("Fahr"), paddingLeft + 52, 114);
	} else {
		tft->print(F("m"), paddingLeft + 68, 74);
		tft->print(F("cel"), paddingLeft + 68, 114);
	}
}

void View::displaySurfaceTimeScreen(DiveResult* diveResult, unsigned long surfaceIntervalInMinutes, bool isDiveStopDisplay, bool isImperial)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	if (isDiveStopDisplay) {
		tft->print(F("DiveIno - Dive time"), 64, 10);
	} else {
		tft->print(F("DiveIno - Surface time"), 64, 10);
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
			if (isImperial) {
				tft->printNumI(diveResult->maxDepthInMeters*3.28, 220, 150, 3);
			} else {
				tft->printNumF(diveResult->maxDepthInMeters, 1, 220, 150);
			}

			tft->setColor(VGA_WHITE);
			tft->print(F("No fly:"), 20, 70);
			tft->print(F("Duration:"), 20, 110);
			tft->print(F("Max depth:"), 20, 150);
			tft->setFont(BigFont);
			tft->print(F("hh:mm"), 310, 83);
			tft->print(F("mm:ss"), 310, 123);
			if (isImperial) {
				tft->print(F("ft"), 278, 163);
			} else {
				tft->print(F("m"), 310, 163);
			}
		} else {
			tft->setColor(VGA_RED);
			tft->printNumI(diveResult->noFlyTimeInMinutes/60, 220, 70, 2, '0');
			tft->print(F(":"), 252, 70);
			tft->printNumI(diveResult->noFlyTimeInMinutes%60, 268, 70, 2 , '0');
			tft->setColor(VGA_GREEN);
			if (surfaceIntervalInMinutes == 0 || surfaceIntervalInMinutes > 2880) {
				//After 48 hours
				tft->print(F("00:00"), 220, 110);
			} else {
				//Within 48 hours
				tft->printNumI(surfaceIntervalInMinutes/60, 220, 110, 2, '0');
				tft->print(F(":"), 252, 110);
				tft->printNumI(surfaceIntervalInMinutes%60, 268, 110, 2 , '0');
			}

			tft->setColor(VGA_WHITE);
			tft->print(F("No fly:"), 20, 70);
			tft->print(F("Duration:"), 20, 110);
			tft->setFont(BigFont);
			tft->print(F("hh:mm"), 310, 83);
			tft->print(F("hh:mm"), 310, 123);
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
		tft->print(F("N/A"), 220, 70);
		tft->setColor(VGA_PURPLE);
		tft->print(F("N/A"), 220, 110);
		tft->setColor(VGA_YELLOW);
		tft->print(F("N/A"), 220, 150);
	}
}

void View::displayGaugeScreen()
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
	tft->print(F("GAUGE"), paddingLeft + tft->getFontXsize() * 4, 215);
}

void View::displaySettingsScreen(byte selectionIndex, float seaLevelPressureSetting, float oxygenRateSetting, bool soundSetting, bool imperialUnitsSetting)
{
	// Settings:
	//
	// From SETTINGS.TXT:
	//  seaLevelPressure = 1013.25
	//  oxygenRate = 0.21
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
	tft->print(F("DiveIno - Settings"), 64, 10);

	// Draw separation line
	tft->drawLine(0, SETTINGS_TOP-10, tft->getDisplayXSize()-1, SETTINGS_TOP-10);

	// Display settings
	tft->setColor(VGA_YELLOW);
	for (int i = 0; i < SETTINGS_SIZE; i++) {
		tft->print(settingsList[i], 0, (i * 40) + SETTINGS_TOP);
	}

	displaySettings(selectionIndex, seaLevelPressureSetting, oxygenRateSetting, soundSetting, imperialUnitsSetting);
}

void View::displaySettings(byte settingIndex, float seaLevelPressureSetting, float oxygenRateSetting, bool soundSetting, bool imperialUnitsSetting)
{
	if (settingIndex == 0) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->printNumF(seaLevelPressureSetting, 1, 240, SETTINGS_TOP);

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
	if (soundSetting) {
		tft->print(F("On "), 240, 80 + SETTINGS_TOP);
	} else {
		tft->print(F("Off"), 240, 80 + SETTINGS_TOP);
	}

	if (settingIndex == 3) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_AQUA);
		tft->setBackColor(VGA_BLACK);
	}
	if (imperialUnitsSetting) {
		tft->print(F("Imperial"), 240, 120 + SETTINGS_TOP);
	} else {
		tft->print(F("Metric  "), 240, 120 + SETTINGS_TOP);
	}

	if (settingIndex == 4) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_GREEN);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print(F("Save"), 10, 211 + SETTINGS_TOP);

	if (settingIndex == 5) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_RED);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print(F("Cancel"), 90, 211 + SETTINGS_TOP);

	if (settingIndex == 6) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_BLUE);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print(F("Default"), 200, 210 + SETTINGS_TOP);

	if (settingIndex == 7) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_FUCHSIA);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print(F("Date&Time"), 330, 210 + SETTINGS_TOP);
}

void View::displayDateTimeSettingScreen(byte settingIndex, DateTimeSettings* dateTimeSettings)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the screen
	tft->setColor(VGA_LIME);
	tft->print(F("DiveIno - Date & Time"), 64, 10);

	// Draw separation line
	tft->drawLine(0, SETTINGS_TOP-10, tft->getDisplayXSize()-1, SETTINGS_TOP-10);

	// Display settings
	tft->setColor(VGA_YELLOW);
	for (int i = 0; i < SETTINGS_SIZE+1; i++) {
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
	tft->print(F("Save"), 120, 210 + SETTINGS_TOP);

	if (settingIndex == 6) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_RED);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print(F("Cancel"), 210, 210 + SETTINGS_TOP);
}

void View::displayAboutScreen()
{
//	tft->clrScr();
//	tft->setBackColor(VGA_BLACK);
//	tft->setFont(Grotesk16x32);
//
//	// Display the header of the menu - the header is the first item
//	tft->setColor(VGA_LIME);
//	tft->print(F("DiveIno - About"), 64, 10);
//
//	// Draw separation line
//	tft->drawLine(0, MENU_TOP-10, 479, MENU_TOP-10);
//
//	tft->setColor(VGA_WHITE);
//	tft->print(F("Version: 1.0.1"), CENTER, 140);
//	tft->setColor(VGA_GREEN);
//	tft->print(F("www.diveino.hu"), CENTER, 180);

	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print(F("DiveIno - About"), 110, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, 479, MENU_TOP-10);

	tft->setColor(VGA_WHITE);
	sdFiles->load(50, 90, 128, 122, "Images/LogoGreen.raw", 8, 0);

	tft->print(F("Version: 1.0.2"), 200 , 110);
	tft->setColor(VGA_GREEN);
	tft->print(F("www.diveino.hu"), 200, 160);
}

void View::displayTestScreen()
{
	displayDiveScreen(0.21);

	drawBatteryStateOfCharge(86.5);

	drawCurrentPressure(2345.67);
	drawCurrentTemperature(22.4, true);
	drawMaximumDepth(36.5, true);

	drawDepth(34.6, true);
	drawDiveDuration(2048);

	DiveInfo diveInfo;
//	diveInfo.decoNeeded = false;
//	diveInfo.minutesToDeco = 46;
//	drawDecoArea(diveInfo, false);

	diveInfo.decoNeeded = true;
    diveInfo.decoStopInMeters = 24;
    diveInfo.decoStopDurationInMinutes = 367;
	drawDecoArea(diveInfo, true);

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
		tft->print(F("OOO"), 420, 275);
	} else if (soc < 100) {
		tft->printNumI(soc, 420, 275, 2, ' ');
		tft->print(F("%"), 452, 275);
	}
}

void View::drawDepth(float depth, bool isImperial)
{
	if (isImperial) {
		tft->setFont(SevenSeg_XXXL);
		tft->setColor(VGA_YELLOW);

		tft->printNumI(depth*3.28, 240, 15, 3, '/');           //Two digits - the / is the SPACE in the font

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("ft"), 435, 98);
	} else {
		tft->setFont(SevenSeg_XXXL);
		tft->setColor(VGA_YELLOW);

		tft->printNumI(depth, 240, 15, 2, '/');           //Two digits - the / is the SPACE in the font
		tft->fillRect(370, 105, 378, 113);	              // Draw the decimal point
		tft->printNumI(((int)(depth*10))%10, 380, 15, 1); // Draw the first decimal fraction digit

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("m"), 444, 98);
	}
}

void View::drawMaximumDepth(float maximumDepth, bool isImperial)
{
	if (isImperial) {
		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_FUCHSIA);
		tft->printNumI(maximumDepth*3.28, 10, 130, 4, '/');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("ft"), 143, 164);
	} else {
		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_FUCHSIA);
		tft->printNumF(maximumDepth, 1, 10, 130, '.', 4, '/');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("m"), 143, 164);
	}
}

void View::drawCurrentTemperature(float currentTemperature, bool isImperial)
{
	if (isImperial) {
		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_LIME);
		tft->printNumI((currentTemperature*1.8)+32, 10, 10, 4, '/'); //the / is the SPACE in the font

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("Fahr"), 143, 44);
	} else {
		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_LIME);
		tft->printNumF(currentTemperature, 1, 10, 10, '.', 4, '/'); //the / is the SPACE in the font

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("cel"), 143, 44);
	}
}

void View::drawCurrentPressure(int currentPressure)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_AQUA);
	tft->printNumI(currentPressure, 10, 70, 4, '/');

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print(F("mBar"), 143, 104);
}

void View::drawDiveDuration(unsigned long duration) // The dive duration is always in seconds
{
	int hours = duration / 3600;
	duration = duration % 3600;
	int minutes = duration / 60;
	int seconds = duration % 60;

	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_WHITE);

	tft->printNumI(hours, 240, 130, 1); // Max 9 hours
	tft->print(F(":"), 272, 130, 1);
	tft->printNumI(minutes, 304, 130, 2, '0');
	tft->print(F(":"), 368, 130, 1);
	tft->printNumI(seconds, 400, 130, 2, '0');
}

void View::drawCurrentTime(String time)
{
	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_TEAL);
	tft->print(time, 40, 265);
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
	tft->print(F("%"), 370, 217);
}

void View::drawDecoArea(DiveInfo diveInfo, bool isImperial)
{
	if (!diveInfo.decoNeeded) {

		tft->setFont(Grotesk16x32);
		tft->setColor(VGA_WHITE);
		tft->print(F("Safe"), 10, 209);

		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_YELLOW);
		tft->printNumI(diveInfo.minutesToDeco, 90, 200, 2, '0');
		//Remove the deco depth value from the screen
		tft->setColor(VGA_BLACK);
		tft->printNumI(0, 218, 200, 2, '/');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print(F("min"), 159, 234);

		//Remove the deco depth from the screen
		tft->setColor(VGA_BLACK);
		tft->print(F("  "), 287, 234);
	} else {

		tft->setFont(Grotesk16x32);
		tft->setColor(VGA_RED);
		tft->print(F("Deco"), 10, 209);

		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_LIME);
		if (diveInfo.decoStopDurationInMinutes < 100) {
			tft->printNumI(diveInfo.decoStopDurationInMinutes, 90, 200, 2, '/');
		} else {
			tft->printNumI(99, 90, 200, 2, '/');
		}
		tft->setColor(VGA_FUCHSIA);
		if (isImperial) {
			tft->printNumI(diveInfo.decoStopInMeters*3.28, 218, 200, 2, '/');
		} else {
			tft->printNumI(diveInfo.decoStopInMeters, 218, 200, 2, '/');
		}

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		if (isImperial) {
			tft->print(F("ft"), 287, 234);
		} else {
			tft->print(F("m"), 287, 234);
		}
		tft->print(F("min"), 159, 234);
	}
}

void View::drawSafetyStop(unsigned int safetyStopDurationInSeconds)
{
	tft->setFont(Grotesk16x32);

	if (safetyStopDurationInSeconds < 180) {
		tft->setColor(VGA_BLUE);
		tft->print(F("Stay"), 10, 269);
	} else {
		tft->setColor(VGA_GREEN);
		tft->print(F("Done"), 10, 269);
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
	tft->print(F(":"), 154, 260, 1);
	tft->printNumI(seconds, 186, 260, 2, '0');
}

void View::drawAscend(int ascendRate)
{

	tft->setFont(Grotesk16x32);
	switch (ascendRate) {
		case ASCEND_OK:
			tft->setColor(VGA_GREEN);
			tft->print(F("----"), 400, 210);
			break;
		case ASCEND_SLOW:
			tft->setColor(VGA_WHITE);
			tft->print(F("---|"), 400, 210);
			break;
		case ASCEND_NORMAL:
			tft->setColor(VGA_YELLOW);
			tft->print(F("--||"), 400, 210);
			break;
		case ASCEND_ATTENTION:
			tft->setColor(VGA_MAROON);
			tft->print(F("-|||"), 400, 210);
			break;
		case ASCEND_CRITICAL:
			tft->setColor(VGA_RED);
			tft->print(F("||||"), 400, 210);
			break;
		case ASCEND_DANGER:
			tft->setColor(VGA_RED);
			tft->print(F("SLOW"), 400, 210);
			break;
		default:
			tft->setColor(VGA_GREEN);
			tft->print(F("<<--"), 400, 210);
	}
}



