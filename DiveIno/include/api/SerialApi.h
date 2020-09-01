#ifndef SerialApi_h
#define SerialApi_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "DiveInoBase.h"
#include "utils/SettingsUtils.h"
#include "utils/TimeUtils.h"
#include "deco/LastDive.h"

class SerialApi : public DiveInoBase {

    public: 
        SerialApi(String versionNumber, SettingsUtils settingsUtils, TimeUtils timeUtils, LastDive* lastDive);
        
        bool isEmulatorEnabled();
        bool isReplayEnabled();

        void reset();

        void updatePressureSensorData(PressureSensorData sensorData);

        void readMessageFromSerial(char data);
        void handleMessage(String message);
    private:    
        String _messageBuffer = "";
        bool _recordMessage = false;

        bool _emulatorEnabled = false;
        bool _replayEnabled = false;

        PressureSensorData _sensorData;
        String _versionNumber;

        SettingsUtils _settingsUtils;
        TimeUtils _timeUtils;

        LastDive* _lastDive;
};

#endif