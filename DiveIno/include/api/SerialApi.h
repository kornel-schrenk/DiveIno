#ifndef SerialApi_h
#define SerialApi_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "DiveInoBase.h"
#include "utils/SettingsUtils.h"
#include "utils/TimeUtils.h"

class SerialApi : public DiveInoBase {

    public: 
        SerialApi(String versionNumber, SettingsUtils settingsUtils, TimeUtils timeUtils);
        
        bool isEmulatorEnabled();
        bool isReplayEnabled();

        void updatePressureSensorData(PressureSensorData sensorData);

        void readMessageFromSerial(char data, bool fromSerial);
        void handleMessage(String message, bool fromSerial);
    private:    
        String _messageBuffer = "";
        bool _recordMessage = false;

        bool _emulatorEnabled = false;
        bool _replayEnabled = false;

        PressureSensorData _sensorData;
        String _versionNumber;

        SettingsUtils _settingsUtils;
        TimeUtils _timeUtils;
};

#endif