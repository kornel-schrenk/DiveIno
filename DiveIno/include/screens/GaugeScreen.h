#ifndef GaugeScreen_h
#define GaugeScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

class GaugeScreen : public DiveInoScreen {

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
        bool _isStopWatchRunning = false;
        unsigned long _stopwatchStartTimestamp;
        unsigned long _stopwatchElapsedTime = 0;    

        void refreshSensorData(PressureSensorData sensorData);
};

#endif