#include "Arduino.h"
#include "View.h"

extern uint8_t Grotesk16x32[];
extern uint8_t SevenSegNumFontPlusPlus[];  //32x50 pixel
extern uint8_t BigFont[];                  //16x16 pixel
extern uint8_t SevenSeg_XXXL[];            //64x100 pixel

char* settingsList[] = {" Sea level",
		" Oxygen %",
		" Test mode",
		" Sound",
		" Units"};

char* mainMenu[] = {" DiveIno - Main Menu ",
        " Dive          ",
        " Logbook       ",
		" Surface Time  ",
		" Gauge         ",
        " Settings      ",
        " About         "};

View::View(UTFT* utft) {
	tft = utft;
}

void View::moveMenuSelection(byte selectedMenuItemIndex, byte menuItemIndex)
{
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
	tft->print(" Logbook - Summary", 48, 10);

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
	tft->printNumI(logbookData->totalDiveHours, paddingLeft, 120);
	tft->print(":", paddingLeft + 32, 120);
	tft->printNumI(logbookData->totalDiveMinutes, paddingLeft + 48, 120);
	tft->setColor(VGA_PURPLE);
	tft->printNumF(logbookData->totalMaximumDepth, 1, paddingLeft, 160);
	tft->setColor(VGA_TEAL);
	tft->print(logbookData->lastDiveDateTime, paddingLeft, 200);
	tft->setColor(VGA_LIME);
	tft->printNumI(logbookData->numberOfStoredProfiles, paddingLeft, 240);
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

void View::displaySurfaceTimeScreen(DiveResult* previousDiveResult)
{
	tft->clrScr();
	tft->setBackColor(VGA_BLACK);
	tft->setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft->setColor(VGA_LIME);
	tft->print("Surface time", 64, 10);

	// Draw separation line
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);

	tft->setColor(VGA_WHITE);
	tft->print("No fly:", 20, 70);
	tft->print("Max depth:", 20, 110);
	tft->print("Duration:", 20, 150);

	if (previousDiveResult != NULL) {
		tft->setColor(VGA_RED);
		tft->printNumI(previousDiveResult->noFlyTimeInMinutes/60, 220, 70, 2, '0');
		tft->print(":", 252, 70);
		tft->printNumI(previousDiveResult->noFlyTimeInMinutes%60, 268, 70, 2 , '0');
		tft->setColor(VGA_PURPLE);
		tft->printNumF(previousDiveResult->maxDepthInMeters, 1, 220, 110);
		tft->setColor(VGA_YELLOW);
		tft->printNumI(previousDiveResult->durationInSeconds/60, 204, 150, 3, ' ');
		tft->print(":", 252, 150);
		tft->printNumI(previousDiveResult->durationInSeconds%60, 268, 150, 2 , '0');

		tft->setColor(VGA_WHITE);
		tft->setFont(BigFont);
		tft->print("hh:mm", 310, 83);
		tft->print("m", 310, 123);
		tft->print("mm:ss", 310, 163);

		//Draw the remaining compartment ppN2 values

		float max = previousDiveResult->compartmentPartialPressures[0];
		for (int i=1; i < COMPARTMENT_COUNT; i++) {
			Serial.print(i);
			Serial.print(" - ppN2: ");
			Serial.println(previousDiveResult->compartmentPartialPressures[i], 0);

			if (max < previousDiveResult->compartmentPartialPressures[i]) {
				max = previousDiveResult->compartmentPartialPressures[i];
			}
		}
		Serial.print("Maximum ppN2: ");
		Serial.println(max, 0);

		tft->setColor(VGA_AQUA);
		for (int i=0; i < COMPARTMENT_COUNT; i++) {
			int x1 = i*30+13;
			int y1 = tft->getDisplayYSize()-((130/(max-755))*(previousDiveResult->compartmentPartialPressures[i]-755));
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
	tft->print("DiveIno - Settings", 48, 10);

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
	tft->print("Save", 120, 210 + SETTINGS_TOP);

	if (settingIndex == 6) {
		tft->setColor(VGA_WHITE);
		tft->setBackColor(VGA_BLUE);
	} else {
		tft->setColor(VGA_MAROON);
		tft->setBackColor(VGA_BLACK);
	}
	tft->print("Default", 210, 210 + SETTINGS_TOP);
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
	tft->drawLine(0, MENU_TOP-10, tft->getDisplayXSize()-1, MENU_TOP-10);	tft->setFont(Grotesk16x32);

	tft->setColor(VGA_WHITE);
	tft->print("Version: 0.8.3", CENTER, 140);
	tft->setColor(VGA_GREEN);
	tft->print("info@diveino.hu", CENTER, 180);
}

void View::displayTestScreen()
{
	displayDiveScreen(0.21);

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

	drawOxigenPercentage(21);
	//drawPartialPressureWarning();
}


void View::drawDepth(float depth)
{
	tft->setFont(SevenSeg_XXXL);
	tft->setColor(VGA_YELLOW);

	int paddingLeft = 240;
	int paddingTop = 15;

	tft->printNumI(depth, paddingLeft, paddingTop, 2, '/'); //Two digits - the / is the SPACE in the font

	// Draw the decimal point
	tft->fillRect(paddingLeft + tft->getFontXsize()*2 + 2, paddingTop + tft->getFontYsize() - 10, paddingLeft + tft->getFontXsize()*2 + 10, paddingTop + tft->getFontYsize() - 2);
	// Draw the first decimal fraction digit
	tft->printNumI(((int)(depth*10))%10, paddingLeft + tft->getFontXsize()*2 + 12, paddingTop, 1); //One digits

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("m", paddingLeft + 64*3 + 12, 100 + paddingTop - tft->getFontYsize());
}

void View::drawMaximumDepth(float maximumDepth)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_PURPLE);

	int paddingLeft = 10;
	int paddingTop = 130;

	tft->printNumF(maximumDepth, 1, paddingLeft, paddingTop, '.', 4, '/');

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("m", paddingLeft + 32*4 + 5, paddingTop + 50 - tft->getFontYsize());
}

void View::drawCurrentTemperature(float currentTemperature)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_LIME);

	int paddingLeft = 10;
	int paddingTop = 10;

	tft->printNumF(currentTemperature, 1, paddingLeft, paddingTop, '.', 4, '/'); //the / is the SPACE in the font

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("cel", paddingLeft + 32*4 + 5, paddingTop + 50 - tft->getFontYsize());
}

void View::drawCurrentPressure(int currentPressure)
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_AQUA);

	int paddingLeft = 10; // 10
	int paddingTop = 70;  // 10 + 50 + 10

	tft->printNumI(currentPressure, paddingLeft, paddingTop, 4, '/');

	tft->setFont(BigFont);
	tft->setColor(VGA_SILVER);
	tft->print("mBar", paddingLeft + 32*4 + 5, paddingTop + 50 - tft->getFontYsize());
}

void View::drawDiveDuration(int duration) // The dive duration is always in seconds
{
	tft->setFont(SevenSegNumFontPlusPlus);
	tft->setColor(VGA_WHITE);

	byte hours = duration / 3600;
	duration = duration % 3600;
	byte minutes = duration / 60;
	byte seconds = duration % 60;

	int paddingLeft = 240;
	int paddingTop = 130;

	tft->printNumI(seconds, paddingLeft + tft->getFontXsize() * 5, paddingTop, 2, '0');
	tft->print(":", paddingLeft + tft->getFontXsize() * 4, paddingTop, 1);
	tft->printNumI(minutes, paddingLeft + tft->getFontXsize() * 2, paddingTop, 2, '0');
	tft->print(":", paddingLeft + tft->getFontXsize() * 1, paddingTop, 1);
	tft->printNumI(hours, paddingLeft, paddingTop, 1); // Max 9 hours
}

void View::drawCurrentTime(String time)
{
	tft->setFont(Grotesk16x32);
	tft->setColor(VGA_TEAL);
	tft->print(time, 80, 265);
}

void View::drawOxigenPercentage(float oxigenPercentage)
{
	int paddingLeft = 290;
	int paddingTop = 217;

	tft->setFont(BigFont);

	if (oxigenPercentage <= 23) {
		tft->setColor(VGA_GREEN);
	} else {
		tft->setColor(VGA_MAROON);
	}
	tft->printNumI(oxigenPercentage, paddingLeft + tft->getFontXsize() * 3, paddingTop, 2, ' ');

	tft->setColor(VGA_WHITE);
	tft->print("%", paddingLeft + tft->getFontXsize() * 5, paddingTop);
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

		int paddingLeft = 90;
		int paddingTop = 200;

		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_YELLOW);
		tft->printNumI(diveInfo.minutesToDeco, paddingLeft, paddingTop, 2, '0');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print("min", paddingLeft + 32*2 + 5, paddingTop + 50 - tft->getFontYsize());
	} else {

		tft->setFont(Grotesk16x32);
		tft->setColor(VGA_RED);
		tft->print("Deco", 10, 209);

		int paddingLeft = 90;
		int paddingTop = 200;

		tft->setFont(SevenSegNumFontPlusPlus);
		tft->setColor(VGA_LIME);
		if (diveInfo.decoStopDurationInMinutes < 100) {
			tft->printNumI(diveInfo.decoStopDurationInMinutes, paddingLeft , paddingTop, 2, '/');
		} else {
			tft->printNumI(99, paddingLeft , paddingTop, 2, '/');
		}
		tft->setColor(VGA_FUCHSIA);
		tft->printNumI(diveInfo.decoStopInMeters, paddingLeft + tft->getFontXsize() * 4, paddingTop, 2, '/');

		tft->setFont(BigFont);
		tft->setColor(VGA_SILVER);
		tft->print("m", paddingLeft + 32*6 + 5, paddingTop + 50 - tft->getFontYsize());
		tft->print("min", paddingLeft + 32*2 + 5, paddingTop + 50 - tft->getFontYsize());
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

	byte minutes = duration / 60;
	byte seconds = duration % 60;

	int paddingLeft = 90;
	int paddingTop = 260;

	tft->printNumI(seconds, paddingLeft + tft->getFontXsize() * 3, paddingTop, 2, '0');
	tft->print(":", paddingLeft + tft->getFontXsize() * 2, paddingTop, 1);
	tft->printNumI(minutes, paddingLeft, paddingTop, 2, '0');
}

void View::drawAscend(int ascendRate)
{
	int paddingLeft = 400;
	int paddingTop = 210;

	tft->setFont(Grotesk16x32);
	switch (ascendRate) {
		case ASCEND_OK:
			tft->setColor(VGA_GREEN);
			tft->print("----", paddingLeft, paddingTop);
			break;
		case ASCEND_SLOW:
			tft->setColor(VGA_WHITE);
			tft->print("---|", paddingLeft, paddingTop);
			break;
		case ASCEND_NORMAL:
			tft->setColor(VGA_YELLOW);
			tft->print("--||", paddingLeft, paddingTop);
			break;
		case ASCEND_ATTENTION:
			tft->setColor(VGA_MAROON);
			tft->print("-|||", paddingLeft, paddingTop);
			break;
		case ASCEND_CRITICAL:
			tft->setColor(VGA_RED);
			tft->print("||||", paddingLeft, paddingTop);
			break;
		case ASCEND_DANGER:
			tft->setColor(VGA_RED);
			tft->print("SLOW", paddingLeft, paddingTop);
			break;
		default:
			tft->setColor(VGA_GREEN);
			tft->print("<<--", paddingLeft, paddingTop);
	}
}



