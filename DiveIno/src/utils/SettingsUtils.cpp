#include "utils/SettingsUtils.h"

DiveInoSettings SettingsUtils::getDiveInoSettings()
{
    struct DiveInoSettings diveInoSettings;

    Preferences prefs;
    prefs.begin("DiveIno", true);	// read-only
    diveInoSettings.seaLevelPressureSetting = prefs.getInt("pressure", 1013);
    diveInoSettings.oxygenRateSetting = prefs.getFloat("oxygen", 0.21);
    diveInoSettings.soundSetting = prefs.getBool("sound", false);
    diveInoSettings.imperialUnitsSetting = prefs.getBool("imperial", false);
    prefs.end();

    _defaultSeaLevelPressure = diveInoSettings.seaLevelPressureSetting;

    return diveInoSettings;    
}

float SettingsUtils::getDefaultSeaLevelPressure()
{
    return (float)_defaultSeaLevelPressure;
}

void SettingsUtils::storeSeaLevelPressureSetting(int value)
{
    //Save the modified Sea Level Pressure setting
    Preferences prefs;
    prefs.begin("DiveIno");
    prefs.putInt("pressure", value);
    prefs.end();

    _defaultSeaLevelPressure = value;
}

void SettingsUtils::storeOxygenRateSetting(float value)
{
    //Save the modified Oxygen Rate setting
    Preferences prefs;
    prefs.begin("DiveIno");
    prefs.putFloat("oxygen", value);
    prefs.end();
}

void SettingsUtils::storeSoundSetting(bool value)
{
    //Save the modified sound setting
    Preferences prefs;
    prefs.begin("DiveIno");
    prefs.putBool("sound", value);
    prefs.end();
}

void SettingsUtils::storeImperialUnitsSetting(bool value)
{
    //Save the modified imperial units setting
    Preferences prefs;
    prefs.begin("DiveIno");
    prefs.putBool("imperial", value);
    prefs.end();
}

DiveInoSettings SettingsUtils::resetSettings()
{
    struct DiveInoSettings diveInoSettings;
    diveInoSettings.seaLevelPressureSetting = 1013;
    diveInoSettings.oxygenRateSetting = 0.21;
    diveInoSettings.soundSetting = false;
    diveInoSettings.imperialUnitsSetting = false;

    Preferences prefs;
    prefs.begin("DiveIno");
    prefs.putInt("pressure", diveInoSettings.seaLevelPressureSetting);
    prefs.putFloat("oxygen", diveInoSettings.oxygenRateSetting);
    prefs.putBool("sound", diveInoSettings.soundSetting);
    prefs.putBool("imperial", diveInoSettings.imperialUnitsSetting);
    prefs.end();

    _defaultSeaLevelPressure = diveInoSettings.seaLevelPressureSetting;

    return diveInoSettings;
}