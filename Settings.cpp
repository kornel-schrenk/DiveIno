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
			diveInoSettings->seaLevelPressureSetting = readSeaLevelPressureSettings(settingsFile);
			diveInoSettings->oxygenRateSetting = readOxygenRateSetting(settingsFile);
			diveInoSettings->testModeSetting = isTestModeSetting(settingsFile);
			diveInoSettings->soundSetting = isSoundSetting(settingsFile);
			diveInoSettings->imperialUnitsSetting = isImperialUnitsSetting(settingsFile);

			settingsFile.close();
		}
	}
	return diveInoSettings;
}

void Settings::saveDiveInoSettings(DiveInoSettings* diveInoSettings)
{
	File settingsFile = SD.open(SETTINGS_FILE_NAME, FILE_WRITE);
	if (settingsFile) {

		String seaLevelPressureToSave = String(diveInoSettings->seaLevelPressureSetting, 2);
		settingsFile.seek(19);
		settingsFile.write(seaLevelPressureToSave.charAt(0));
		settingsFile.write(seaLevelPressureToSave.charAt(1));
		settingsFile.write(seaLevelPressureToSave.charAt(2));
		settingsFile.write(seaLevelPressureToSave.charAt(3));
		settingsFile.write(seaLevelPressureToSave.charAt(4));
		settingsFile.write(seaLevelPressureToSave.charAt(5));
		settingsFile.write(seaLevelPressureToSave.charAt(6));

		String oxygenRateToSave = String(diveInoSettings->oxygenRateSetting, 2);
		settingsFile.seek(41);
		settingsFile.write(oxygenRateToSave.charAt(0));
		settingsFile.write(oxygenRateToSave.charAt(1));
		settingsFile.write(oxygenRateToSave.charAt(2));
		settingsFile.write(oxygenRateToSave.charAt(3));

		settingsFile.seek(58);
		if (!diveInoSettings->testModeSetting) {
			settingsFile.write('0');
		} else {
			settingsFile.write('1');
		}

		settingsFile.seek(69);
		if (!diveInoSettings->soundSetting) {
			settingsFile.write('0');
		} else {
			settingsFile.write('1');
		}

		settingsFile.seek(80);
		if (!diveInoSettings->imperialUnitsSetting) {
			settingsFile.write('0');
		} else {
			settingsFile.write('1');
		}

		settingsFile.close();
	}
}

/////////////////////
// Private methods //
/////////////////////

float Settings::readSeaLevelPressureSettings(File settingsFile)
{
	settingsFile.seek(19);
	char digits[8];
	digits[0] = settingsFile.read();
	digits[1] = settingsFile.read();
	digits[2] = settingsFile.read();
	digits[3] = settingsFile.read();
	digits[4] = settingsFile.read();
	digits[5] = settingsFile.read();
	digits[6] = settingsFile.read();
	digits[7] = '\0';
	return String(digits).toFloat();
}

float Settings::readOxygenRateSetting(File settingsFile)
{
	settingsFile.seek(41);
	char digits[5];
	digits[0] = settingsFile.read();
	digits[1] = settingsFile.read();
	digits[2] = settingsFile.read();
	digits[3] = settingsFile.read();
	digits[4] = '\0';
	return String(digits).toFloat();
}

bool Settings::isTestModeSetting(File settingsFile)
{
	settingsFile.seek(58);
	if (settingsFile.read() == '0') {
		return false;
	}
	return true;
}

bool Settings::isSoundSetting(File settingsFile)
{
	settingsFile.seek(69);
	if (settingsFile.read() == '0') {
		return false;
	}
	return true;
}

bool Settings::isImperialUnitsSetting(File settingsFile)
{
	settingsFile.seek(80);
	if (settingsFile.read() == '0') {
		return false;
	}
	return true;
}
