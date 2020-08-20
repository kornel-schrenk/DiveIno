#ifndef DiveInoBase_h
#define DiveInoBase_h

struct PressureSensorData {
    float pressureInMillibar = 1013.2; 
	float temperatureInCelsius = 99;
};

struct DiveInoSettings {
	int seaLevelPressureSetting = 1013;
	float oxygenRateSetting = 0.21;
	bool soundSetting = false;
	bool imperialUnitsSetting = false;
};

class DiveInoBase {

    public: 
    
    private:
};

#endif