#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#include "SettingsScreen.h"

SettingsScreen::SettingsScreen(Adafruit_ILI9341* adafruitTft, ScreenUtils* sUtils) {
	tft = adafruitTft;
	screenUtils = sUtils;
	settings = NULL;
}

void SettingsScreen::open()
{
	settings = new DiveInoSettings;

	// Clear the screen
	tft->fillScreen(ILI9341_BLACK);

	tft->setFont(&DroidSansMono16pt7b);
	tft->setTextColor(ILI9341_DARKCYAN, ILI9341_BLACK);

	screenUtils->drawBmpFile((char *)"Touch/settingsPressure.bmp", 10, 10);  // 50x50
	tft->setCursor(80, 45);
	tft->print(settings->seaLevelPressureSetting, 0);
	screenUtils->drawButtonFrame(60, 10, 120, 50, 5, false);
	screenUtils->drawBmpFile((char *)"Touch/settingsSync.bmp", 180, 15);
	screenUtils->drawButtonFrame(175, 10, 50, 50, 5, false);

	screenUtils->drawBmpFile((char *)"Touch/settingsOxygen.bmp", 10, 80);
	tft->setCursor(80, 115);
	tft->print(settings->oxygenRateSetting, 1);
	tft->print(" %");
	screenUtils->drawButtonFrame(60, 80, 165, 50, 5, false);

	if (settings->imperialUnitsSetting) {
		screenUtils->drawBmpFile((char *)"Touch/settingsFarenheit.bmp", 55, 160);
	} else {
		screenUtils->drawBmpFile((char *)"Touch/settingsCelsius.bmp", 55, 160);
	}
	screenUtils->drawButtonFrame(50, 155, 60, 60, 5, false);

	if (settings->soundSetting) {
		screenUtils->drawBmpFile((char *)"Touch/settingsSound.bmp", 135, 160);
	} else {
		screenUtils->drawBmpFile((char *)"Touch/settingsMute.bmp", 135, 160);
	}
	screenUtils->drawButtonFrame(130, 155, 60, 60, 5, false);

	screenUtils->drawBmpFile((char *)"Touch/back.bmp", 10, 250);
	screenUtils->drawButtonFrame(5, 245, 70, 70, 5, false);

	screenUtils->drawBmpFile((char *)"Touch/settingsSave.bmp", 90, 250);
	screenUtils->drawButtonFrame(85, 245, 70, 70, 5, false);

	screenUtils->drawBmpFile((char *)"Touch/settingsCalendar.bmp", 170, 250);
	screenUtils->drawButtonFrame(165, 245, 70, 70, 5, false);

}

int SettingsScreen::handleTouch(Adafruit_FT6206* touchScreen)
{
	if (touchScreen->touched()) {
		TS_Point touchPoint = touchScreen->getPoint();

		// flip it around to match the screen.
		touchPoint.x = map(touchPoint.x, 0, 240, 240, 0);
		touchPoint.y = map(touchPoint.y, 0, 320, 320, 0);

		if (5 < touchPoint.x && touchPoint.x < 5+70 &&
			245 < touchPoint.y && touchPoint.y < 245+70) {
			drawButtonPress(BUTTON_BACK);
			return GO_HOME;
		} else if (85 < touchPoint.x && touchPoint.x < 85+70 &&
				   245 < touchPoint.y && touchPoint.y < 245+70) {
			drawButtonPress(BUTTON_SAVE);
			return NO_ACTION; //TODO Modify it to SAVE
		} else if (165 < touchPoint.x && touchPoint.x < 165+70 &&
				   245 < touchPoint.y && touchPoint.y < 245+70) {
			drawButtonPress(BUTTON_CALENDAR);
			return GO_CALENDAR;
		} else if (50 < touchPoint.x && touchPoint.x < 50+60 &&
				   155 < touchPoint.y && touchPoint.y < 255+60) {
			settings->imperialUnitsSetting = !settings->imperialUnitsSetting;
			if (settings->imperialUnitsSetting) {
				screenUtils->drawBmpFile((char *)"Touch/settingsFarenheit.bmp", 55, 160);
			} else {
				screenUtils->drawBmpFile((char *)"Touch/settingsCelsius.bmp", 55, 160);
			}
			drawButtonPress(BUTTON_UNITS);
			return NO_ACTION;
		} else if (130 < touchPoint.x && touchPoint.x < 130+60 &&
				   155 < touchPoint.y && touchPoint.y < 155+60) {
			settings->soundSetting = !settings->soundSetting;
			if (settings->soundSetting) {
				screenUtils->drawBmpFile((char *)"Touch/settingsSound.bmp", 135, 160);
			} else {
				screenUtils->drawBmpFile((char *)"Touch/settingsMute.bmp", 135, 160);
			}
			Serial.print("Sound: ");
			Serial.print(settings->soundSetting);
			drawButtonPress(BUTTON_SOUND);
			return NO_ACTION;
		} else if (60 < touchPoint.x && touchPoint.x < 60+120 &&
				   10 < touchPoint.y && touchPoint.y < 10+50) {
			if (settings->seaLevelPressureSetting < 1029) {
				settings->seaLevelPressureSetting = settings->seaLevelPressureSetting + 1.0;
			} else {
				settings->seaLevelPressureSetting = 990;
			}

			tft->fillRect(65, 15, 100, 40, ILI9341_BLACK);
			tft->setCursor(80, 45);
			tft->print(settings->seaLevelPressureSetting, 0);

			drawButtonPress(BUTTON_PRESSURE);
			return NO_ACTION;
		} else if (175 < touchPoint.x && touchPoint.x < 175+50 &&
				   10 < touchPoint.y && touchPoint.y < 10+50) {

			//TODO Sync based on the real pressure from the sensor
			settings->seaLevelPressureSetting = 1013.0;

			tft->fillRect(65, 15, 100, 40, ILI9341_BLACK);
			tft->setCursor(80, 45);
			tft->print(settings->seaLevelPressureSetting, 0);

			drawButtonPress(BUTTON_SYNC);
			return NO_ACTION;
		} else if (60+85 < touchPoint.x && touchPoint.x < 60+165 &&
				   80 < touchPoint.y && touchPoint.y < 80+50) {

			if (settings->oxygenRateSetting < 36.4) {
				settings->oxygenRateSetting  = settings->oxygenRateSetting + 0.1;
			}

			tft->fillRect(65, 85, 100, 40, ILI9341_BLACK);
			tft->setCursor(80, 115);
			tft->print(settings->oxygenRateSetting, 1);

			drawButtonPress(BUTTON_OXYGEN);
			return NO_ACTION;
		} else if (60 < touchPoint.x && touchPoint.x < 60+85 &&
				   80 < touchPoint.y && touchPoint.y < 80+50) {

			if (settings->oxygenRateSetting > 21.0) {
				settings->oxygenRateSetting  = settings->oxygenRateSetting - 0.1;
			}

			tft->fillRect(65, 85, 100, 40, ILI9341_BLACK);
			tft->setCursor(80, 115);
			tft->print(settings->oxygenRateSetting, 1);

			drawButtonPress(BUTTON_OXYGEN);
			return NO_ACTION;
		}

		delay(100);
	}

	return NO_ACTION;
}

void SettingsScreen::drawButtonPress(int button) {
	switch (button) {
	case BUTTON_BACK:
		screenUtils->drawButtonFrame(5, 245, 70, 70, 5, true);
		break;
	case BUTTON_SAVE:
		screenUtils->drawButtonFrame(85, 245, 70, 70, 5, true);
		break;
	case BUTTON_CALENDAR:
		screenUtils->drawButtonFrame(165, 245, 70, 70, 5, true);
		break;
	case BUTTON_UNITS:
		screenUtils->drawButtonFrame(50, 155, 60, 60, 5, true);
		break;
	case BUTTON_SOUND:
		screenUtils->drawButtonFrame(130, 155, 60, 60, 5, true);
		break;
	case BUTTON_PRESSURE:
		screenUtils->drawButtonFrame(60, 10, 120, 50, 5, true);
		break;
	case BUTTON_SYNC:
		screenUtils->drawButtonFrame(175, 10, 50, 50, 5, true);
		break;
	case BUTTON_OXYGEN:
		screenUtils->drawButtonFrame(60, 80, 165, 50, 5, true);
		break;
	}

	delay(100);

	switch (button) {
	case BUTTON_BACK:
		screenUtils->drawButtonFrame(5, 245, 70, 70, 5, false);
		break;
	case BUTTON_SAVE:
		screenUtils->drawButtonFrame(85, 245, 70, 70, 5, false);
		break;
	case BUTTON_CALENDAR:
		screenUtils->drawButtonFrame(165, 245, 70, 70, 5, false);
		break;
	case BUTTON_UNITS:
		screenUtils->drawButtonFrame(50, 155, 60, 60, 5, false);
		break;
	case BUTTON_SOUND:
		screenUtils->drawButtonFrame(130, 155, 60, 60, 5, false);
		break;
	case BUTTON_PRESSURE:
		screenUtils->drawButtonFrame(60, 10, 120, 50, 5, false);
		break;
	case BUTTON_SYNC:
		screenUtils->drawButtonFrame(175, 10, 50, 50, 5, false);
		break;
	case BUTTON_OXYGEN:
		screenUtils->drawButtonFrame(60, 80, 165, 50, 5, false);
		break;
	}
}


