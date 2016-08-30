#include "Settings.h"

#define SETTINGS_FILE_NAME "settings.txt"

const String NEW_LINE = "\n";
const char ZERO = '0';
const char ONE = '1';

Settings::Settings() {
}

//////////////////////
// Public interface //
//////////////////////

DiveInoSettings* Settings::loadDiveInoSettings()
{
	DiveInoSettings* diveInoSettings = new DiveInoSettings;

	if (SD.exists(SETTINGS_FILE_NAME)) {
		File settingsFile = SD.open(SETTINGS_FILE_NAME);
		if (settingsFile) {
			String line;
			int counter = 0;
			while (settingsFile.available()) {
				line = settingsFile.readStringUntil('\n');

				if (counter == 0) {
					diveInoSettings->seaLevelPressureSetting = readFloatFromLineEnd(line);
				} else if (counter == 1) {
					diveInoSettings->oxygenRateSetting = readFloatFromLineEnd(line);
				} else if (counter == 2) {
					diveInoSettings->soundSetting = readBoolFromLineEnd(line);
				} else if (counter == 3) {
					diveInoSettings->imperialUnitsSetting = readBoolFromLineEnd(line);
				}
				counter++;
			}
			settingsFile.close();
		}
	} else {
		//Create a new Settings file, if it is not on the SD card with default values
		saveDiveInoSettings(diveInoSettings);
	}
	return diveInoSettings;
}

void Settings::saveDiveInoSettings(DiveInoSettings* diveInoSettings)
{
	SD.remove(SETTINGS_FILE_NAME);

	File settingsFile = SD.open(SETTINGS_FILE_NAME, FILE_WRITE);

	settingsFile.print(F("seaLevelPressure = "));
	settingsFile.print(diveInoSettings->seaLevelPressureSetting);
	settingsFile.print(NEW_LINE);
	settingsFile.print(F("oxygenRate = "));
	settingsFile.print(diveInoSettings->oxygenRateSetting);
	settingsFile.print(NEW_LINE);
	settingsFile.print(F("sound = "));
	if (!diveInoSettings->soundSetting) {
		settingsFile.print(ZERO);
	} else {
		settingsFile.print(ONE);
	}
	settingsFile.print(NEW_LINE);
	settingsFile.print(F("units = "));
	if (!diveInoSettings->imperialUnitsSetting) {
		settingsFile.print(ZERO);
	} else {
		settingsFile.print(ONE);
	}
	settingsFile.print(NEW_LINE);
	settingsFile.flush();

	settingsFile.close();
}

/////////////////////////////
// Date and Time functions //
/////////////////////////////

String Settings::getCurrentTimeText() {

	String time = "";

	tmElements_t tm;
	if (RTC.read(tm)) {
		time+=tmYearToCalendar(tm.Year);
		time+="-";
		if (tm.Month <10) {
			time+=ZERO;
		}
		time+=tm.Month;
		time+="-";
		if (tm.Day<10) {
			time+=ZERO;
		}
		time+=tm.Day;
		time+=" ";

		if (tm.Hour<10) {
			time+=ZERO;
		}
		time+=tm.Hour;
		time+=":";
		if (tm.Minute<10) {
			time+=ZERO;
		}
		time+=tm.Minute;
		time+=":";
		if (tm.Second<10) {
			time+=ZERO;
		}
		time+=tm.Second;
	} else {
		time+="N/A";
	}

	return time;
}

DateTimeSettings* Settings::getCurrentTime() {

	tmElements_t tm;
	if (RTC.read(tm)) {
		DateTimeSettings* dateTimeSettings = new DateTimeSettings;
		dateTimeSettings->year = tmYearToCalendar(tm.Year);
		dateTimeSettings->month = tm.Month;
		dateTimeSettings->day = tm.Day;
		dateTimeSettings->hour = tm.Hour;
		dateTimeSettings->minute = tm.Minute;
		dateTimeSettings->second = tm.Second;
		return dateTimeSettings;
	}
	return NULL;
}

void Settings::setCurrentTime(DateTimeSettings* dateTimeSettings) {
	tmElements_t newTime;

	newTime.Year = CalendarYrToTm(dateTimeSettings->year); //It is converted to years since 1970 according to the Time library
	newTime.Month = dateTimeSettings->month;
	newTime.Day = dateTimeSettings->day;
	newTime.Hour = dateTimeSettings->hour;
	newTime.Minute = dateTimeSettings->minute;
	newTime.Second = 0;

	RTC.write(newTime);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	//Convert the new time to Unix time format
	setTime(makeTime(newTime));
#endif
}


/////////////////////
// Private methods //
/////////////////////

bool Settings::readBoolFromLineEnd(String line) {
	int value = line.substring(line.indexOf('=')+1).toInt();
	if (value > 0) {
		return true;
	}
	return false;
}

float Settings::readFloatFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toFloat();
}
