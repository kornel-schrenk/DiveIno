#ifndef DiveScreen_h
#define DiveScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"
#include "deco/Buhlmann.h"

#define SAFE_MODE           510
#define SAFETY_STOP_MODE    520
#define DECO_MODE           530
#define TILT_MODE           540

#define SAFETY_STOP_NOT_STARTED 0
#define SAFETY_STOP_IN_PROGRESS 1
#define SAFETY_STOP_DONE 		2
#define SAFETY_STOP_VIOLATED	3

class DiveScreen : public DiveInoScreen {

    public:
        DiveScreen(Buhlmann* buhlmann);
        
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
        DiveInoSettings _diveInoSettings;
        int _currentMode = SAFE_MODE;
        bool _diveStarted = false;

        bool _emulatorEnabled = false;
        bool _replayEnabled = false;

        unsigned long _previousDiveDurationInSeconds = 0;
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
        
        void _drawDiveIndicator();   

        byte _safetyStopState = SAFETY_STOP_NOT_STARTED;
        unsigned long _safetyStopDurationInSeconds;
        void _calculateSafetyStop(float maxDepthInMeter, float depthInMeter, unsigned int intervalDuration);
        void _clearSafetyStopDisplay();

        void _clearDecoDisplay();
        DiveInfo _currentDiveInfo;
        void startDive();
        void _diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds);
        void stopDive();

};

#endif