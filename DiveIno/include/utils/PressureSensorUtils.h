#ifndef PressureSensorUtils_h
#define PressureSensorUtils_h

#include "Arduino.h"
#include "SparkFun_MS5803_I2C.h"

#include "DiveInoBase.h"

class PressureSensorUtils : public DiveInoBase {
    
    public:
        void initPressureSensor();

        PressureSensorData readPressureSensorData(float seaLevelPressureSetting, bool emulatorEnabled);

    private:  
        MS5803 _sensor = MS5803(ADDRESS_HIGH);;

        float _previousPressureInMillibar = 1013.0;
};

#endif