#ifndef GaugeScreen_h
#define GaugeScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"
#include "deco/Buhlmann.h"

class GaugeScreen : public DiveInoScreen {

    public:
        GaugeScreen(Buhlmann* buhlmann);

        void init(DiveInoSettings diveInoSettings, PressureSensorData sensorData, bool replayEnabled, bool emulatorEnabled);
        
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
        Buhlmann* _buhlmann;

        bool _emulatorEnabled = false;
        bool _replayEnabled = false;
        void _replayDive();

        bool _isStopWatchRunning = false;
        unsigned long _stopwatchStartTimestamp;
        unsigned long _stopwatchElapsedTime = 0;    

        void _displayActualTime(unsigned long elapsedTimeInSeconds);

        float _maxDepthInMeter = 0.0;

        void _drawCurrentDepth(float depthInMeter);
        void _drawMaximumDepth(float depthInMeter);
        void _drawSensorData(PressureSensorData sensorData);
        void _refreshMaxDepth(float currentDepthInMeter);        
};

#endif