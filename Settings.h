#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Arduino.h"
#include "SD.h"
#include "DS3232RTC.h"
#include "Time.h"

typedef struct DiveInoSettings {
	float seaLevelPressureSetting = 1013.25;
	float oxygenRateSetting = 0.21;
	bool testModeSetting = true;
	bool soundSetting = true;
	bool imperialUnitsSetting = false;
};

typedef struct DateTimeSettings {
	int year = 2000;
	int month = 1;
	int day = 1;
	int hour = 10;
	int minute = 59;
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
	float readSeaLevelPressureSettings(File settingsFile);
	float readOxygenRateSetting(File settingsFile);
	bool isTestModeSetting(File settingsFile);
	bool isSoundSetting(File settingsFile);
	bool isImperialUnitsSetting(File settingsFile);
};

#endif /* SETTINGS_H_ */
