#include "Settings.h"

#define SETTINGS_FILE_NAME "Settings.json"

const String NEW_LINE = "\n";
const char ZERO = '0';

Settings::Settings() {
}

//////////////////////
// Public interface //
//////////////////////

DiveInoSettings* Settings::loadDiveInoSettings()
{
	DiveInoSettings* diveInoSettings = new DiveInoSettings;

	if (SD.exists(SETTINGS_FILE_NAME)) {
		SdFile settingsFile;
		if (settingsFile.open(SETTINGS_FILE_NAME, O_READ)) {

			char fileContent[settingsFile.fileSize()];
			int counter = 0;
			while (settingsFile.available()) {
				fileContent[counter] = settingsFile.read();
				counter++;
			}
			settingsFile.close();

			StaticJsonBuffer<JSON_OBJECT_SIZE(5)> jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(fileContent);

			diveInoSettings->seaLevelPressureSetting = root["seaLevelPressure"];
			diveInoSettings->oxygenRateSetting = root["oxygenPercentage"];
			diveInoSettings->soundSetting = root["soundEnabled"];
			diveInoSettings->imperialUnitsSetting = root["isImperialUnits"];
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

	SdFile settingsFile;
	if (settingsFile.open(SETTINGS_FILE_NAME, O_WRITE | O_CREAT | O_APPEND )) {

		StaticJsonBuffer<JSON_OBJECT_SIZE(5)> jsonBuffer;

		JsonObject& root = jsonBuffer.createObject();
		root["seaLevelPressure"] = double_with_n_digits(diveInoSettings->seaLevelPressureSetting, 5);
		root.set("oxygenPercentage", diveInoSettings->oxygenRateSetting);
		root.set("soundEnabled", diveInoSettings->soundSetting);
		root.set("isImperialUnits", diveInoSettings->imperialUnitsSetting);

		SdFile settingsFile;
		if (settingsFile.open(SETTINGS_FILE_NAME, O_WRITE | O_CREAT | O_APPEND )) {
			root.prettyPrintTo(settingsFile);
			settingsFile.flush();
			settingsFile.close();
		}
	}
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

