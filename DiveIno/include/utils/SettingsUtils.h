#ifndef SettingsUtils_h
#define SettingsUtils_h

#include "Arduino.h"
#include "Preferences.h"
#include "M5ez.h"
#include "ezTime.h"

#include "DiveInoBase.h"

class SettingsUtils : public DiveInoBase {
    
    public:
        DiveInoSettings getDiveInoSettings();
        DiveInoSettings resetSettings();

        float getDefaultSeaLevelPressure();

        void storeSeaLevelPressureSetting(int value);
        void storeOxygenRateSetting(float value);
        void storeSoundSetting(bool value);
        void storeImperialUnitsSetting(bool value);
    private:  
        int _defaultSeaLevelPressure = 1013;
};

#endif