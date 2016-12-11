#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Arduino.h"
#include "SdFat.h"
#include "TimeLib.h"
#include "DS1307RTC.h"
#include "ArduinoJson.h"

extern SdFat SD;

typedef struct DiveInoSettings {
	float seaLevelPressureSetting = 1013.2;
	float oxygenRateSetting = 0.21;
	bool soundSetting = true;
	bool imperialUnitsSetting = false;
};

typedef struct DateTimeSettings {
	int year = 2000;
	int month = 1;
	int day = 1;
	int hour = 10;
	int minute = 59;
	int second = 0;
};

class Settings {
public:
	Settings();
	DiveInoSettings* loadDiveInoSettings();
	void saveDiveInoSettings(DiveInoSettings* diveInoSettings);
	String getCurrentTimeText();
	DateTimeSettings* getCurrentTime();
	void setCurrentTime(DateTimeSettings* dateTimeSettings);
private:
};

#endif /* SETTINGS_H_ */
