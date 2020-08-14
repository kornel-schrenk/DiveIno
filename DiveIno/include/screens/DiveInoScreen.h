#ifndef DiveInoScreen_h
#define DiveInoScreen_h

#define SCREEN_HOME       310
#define SCREEN_DIVE       320
#define SCREEN_GAUGE      330
#define SCREEN_LOGBOOK    340
#define SCREEN_SURFACE    350
#define SCREEN_SETTINGS   360
#define SCREEN_ABOUT      370

#include "images/jpgs.h"
#include "images/jpgsdark.h"

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

class DiveInoScreen {

    public: 
        void refreshClockWidget();
        bool isClockWidgetDisplayed();
    
    private:
};

#endif