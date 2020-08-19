#ifndef DiveScreen_h
#define DiveScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

#define SAFE_MODE           510
#define SAFETY_STOP_MODE    520
#define DECO_MODE           530
#define TILT_MODE           540

class DiveScreen : public DiveInoScreen {

    public:
        void init(DiveInoSettings diveInoSettings, PressureSensorData sensorData);

        void display(PressureSensorData sensorData);

        void displayStopwatch();
        void displayZeroTime();
        void displayActualTime();
        
        void startStopwatch();
        void stopStopwatch();
        void resetStopwatch();
        
        bool isRunning();

        void handleButtonPress(String buttonName);

    private:
        int _currentMode = SAFE_MODE;

        bool _isStopWatchRunning = false;
        unsigned long _stopwatchStartTimestamp;
        unsigned long _stopwatchElapsedTime = 0;    

        void refreshSensorData(PressureSensorData sensorData);    
};

#endif