#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Arduino.h"
#include "SD.h"

typedef struct DiveInoSettings {
	float seaLevelPressureSetting = 1013.25;
	float oxygenRateSetting = 0.21;
	bool testModeSetting = true;
	bool soundSetting = true;
	bool imperialUnitsSetting = false;
};

class Settings {
public:
	Settings();
	DiveInoSettings* loadDiveInoSettings();
	void saveDiveInoSettings(DiveInoSettings* diveInoSettings);
private:
	float readSeaLevelPressureSettings(File settingsFile);
	float readOxygenRateSetting(File settingsFile);
	bool isTestModeSetting(File settingsFile);
	bool isSoundSetting(File settingsFile);
	bool isImperialUnitsSetting(File settingsFile);
};

#endif /* SETTINGS_H_ */
