#include "Settings.h"

#define SETTINGS_FILE_NAME "settings.txt"

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
					diveInoSettings->testModeSetting = readBoolFromLineEnd(line);
				} else if (counter == 3) {
					diveInoSettings->soundSetting = readBoolFromLineEnd(line);
				} else if (counter == 4) {
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

	settingsFile.print("seaLevelPressure = ");
	settingsFile.print(diveInoSettings->seaLevelPressureSetting);
	settingsFile.print("\n");
	settingsFile.print("oxygenRate = ");
	settingsFile.print(diveInoSettings->oxygenRateSetting);
	settingsFile.print("\n");
	settingsFile.print("testMode = ");
	if (!diveInoSettings->testModeSetting) {
		settingsFile.print('0');
	} else {
		settingsFile.print('1');
	}
	settingsFile.print("\n");
	settingsFile.print("sound = ");
	if (!diveInoSettings->soundSetting) {
		settingsFile.print('0');
	} else {
		settingsFile.print('1');
	}
	settingsFile.print("\n");
	settingsFile.print("units = ");
	if (!diveInoSettings->imperialUnitsSetting) {
		settingsFile.print('0');
	} else {
		settingsFile.print('1');
	}
	settingsFile.print("\n");
	settingsFile.flush();

	settingsFile.close();
}

/////////////////////////////
// Date and Time functions //
/////////////////////////////

String Settings::getCurrentTimeText() {

	String time = "";

	DateTimeSettings* dateTimeSettings = new DateTimeSettings;
	dateTimeSettings = getCurrentTime();

	time+=dateTimeSettings->year;
	time+="-";
	if (dateTimeSettings->month <10) {
		time+="0";
	}
	time+=dateTimeSettings->month;
	time+="-";
	if (dateTimeSettings->day<10) {
		time+="0";
	}
	time+=dateTimeSettings->day;
	time+=" ";

	if (dateTimeSettings->hour<10) {
		time+="0";
	}
	time+=dateTimeSettings->hour;
	time+=":";
	if (dateTimeSettings->minute<10) {
		time+="0";
	}
	time+=dateTimeSettings->minute;
	time+=":";
	if (dateTimeSettings->second<10) {
		time+="0";
	}
	time+=dateTimeSettings->second;

	return time;
}

DateTimeSettings* Settings::getCurrentTime() {

	DateTimeSettings* dateTimeSettings = new DateTimeSettings;

	tmElements_t tm;
	if (RTC.read(tm)) {
		dateTimeSettings->year = tmYearToCalendar(tm.Year);
		dateTimeSettings->month = tm.Month;
		dateTimeSettings->day = tm.Day;
		dateTimeSettings->hour = tm.Hour;
		dateTimeSettings->minute = tm.Minute;
		dateTimeSettings->second = tm.Second;
	}
	return dateTimeSettings;
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
