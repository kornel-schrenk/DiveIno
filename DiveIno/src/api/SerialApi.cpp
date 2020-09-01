#include "api/SerialApi.h"

////////////////
// Serial API //
////////////////

SerialApi::SerialApi(String versionNumber, SettingsUtils settingsUtils, TimeUtils timeUtils, LastDive* lastDive)
{
    _versionNumber = versionNumber;
    _settingsUtils = settingsUtils;
	_timeUtils = timeUtils;
	_lastDive = lastDive;
}

void SerialApi::updatePressureSensorData(PressureSensorData sensorData)
{
   _sensorData = sensorData; 
}

bool SerialApi::isEmulatorEnabled()
{
    return _emulatorEnabled;
}

bool SerialApi::isReplayEnabled()
{
    return _replayEnabled;
}

void SerialApi::reset()
{
	_emulatorEnabled = false;
	_replayEnabled = false;
}

void SerialApi::readMessageFromSerial(char data)
{
	if (data == '@') {
		_messageBuffer = "";
		_recordMessage = true;
	} else if (data == '#') {
		handleMessage(_messageBuffer);
		_recordMessage = false;
	} else {
		if (_recordMessage) {
			_messageBuffer += data;
		}
	}
}

void SerialApi::handleMessage(String message) 
{
	Stream* outputStream = &Serial;

	String responseMessage = "@";
	if (message.startsWith(F("PROFILE")) || message.startsWith(F("profile"))) {
		int profileNumber = message.substring(7).toInt();
		if (profileNumber > 0) {
			outputStream->println("@");
            //TODO Implement Logbook based dive profile retrieval
			//logbook.printProfile(profileNumber, outputStream);
			outputStream->println("#");
		} else {
			responseMessage += F("ERROR: Invalid argument - Positive Integer value expected!#");
			outputStream->println(responseMessage);
		}
	} else if (message.startsWith(F("LOGBOOK")) || message.startsWith(F("logbook"))) {
		outputStream->println("@");
        //TODO Implement Logbook retreival
		//logbook.printLogbook(outputStream);
		outputStream->println("#");
	} else if (message.startsWith(F("EMULATOR")) || message.startsWith(F("emulator"))) {
		String onOffEmulator = message.substring(8);
		onOffEmulator.trim();
		if (onOffEmulator.startsWith(F("on")) || onOffEmulator.startsWith(F("ON"))) {
			_emulatorEnabled = true;
			responseMessage += F("EMULATOR - Enabled#");
		} else {
			_emulatorEnabled = false;
			responseMessage += F("EMULATOR - Disabled#");
		}
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("REPLAY")) || message.startsWith(F("replay"))) {
		String onOffReplay = message.substring(6);
		onOffReplay.trim();
		if (onOffReplay.startsWith(F("on")) || onOffReplay.startsWith(F("ON"))) {
			_replayEnabled = true;
			responseMessage += F("REPLAY - Enabled#");
		} else {
			_replayEnabled = false;
			responseMessage += F("REPLAY - Disabled#");
		}
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("VERSION")) || message.startsWith(F("version"))) {
		responseMessage += _versionNumber;
		responseMessage += F("#");
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("DIAG")) || message.startsWith(F("diag"))) {
		String what = message.substring(4);
		what.trim();
		if (what.startsWith(F("TEMP")) || what.startsWith(F("temp"))) {			
			responseMessage += _sensorData.temperatureInCelsius;
		} else if (what.startsWith(F("PRES")) || what.startsWith(F("pres"))) {			
			responseMessage += _sensorData.pressureInMillibar;
		} else if (what.startsWith(F("BAT")) || what.startsWith(F("bat"))) {
			responseMessage += ez.battery.getTransformedBatteryLevel();
		} else if (what.startsWith(F("DATE")) || what.startsWith(F("date"))) {
			responseMessage += ez.clock.tz.dateTime("Y-m-d");
		} else if (what.startsWith(F("TIMESTAMP")) || what.startsWith(F("timestamp"))) {
			responseMessage += ez.clock.tz.now();
		}
		responseMessage += F("#");
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("SETTINGS")) || message.startsWith(F("settings"))) {		
        DiveInoSettings diveInoSettings = _settingsUtils.getDiveInoSettings();
		outputStream->println("@");
        outputStream->println("{");
        outputStream->print("""seaLevelPressure"": " ); 
        outputStream->print(diveInoSettings.seaLevelPressureSetting);
        outputStream->println(",");
        outputStream->print("""oxygenPercentage"": " ); 
        outputStream->print(diveInoSettings.oxygenRateSetting);
        outputStream->println(",");
        outputStream->print("""soundEnabled"": " ); 
        outputStream->print(diveInoSettings.soundSetting ? "true" : "false");
        outputStream->println(",");
        outputStream->print("""isImperialUnits"": " ); 
        outputStream->print(diveInoSettings.imperialUnitsSetting ? "true" : "false");
        outputStream->println(",");
        outputStream->println("}");
        outputStream->println("#");
	} else if (message.startsWith(F("AUTO")) || message.startsWith(F("auto"))) {
		float pressureInMillibar = _sensorData.pressureInMillibar;
		if (990 <= pressureInMillibar && pressureInMillibar <= 1030) {			
			_settingsUtils.storeSeaLevelPressureSetting(pressureInMillibar);
            responseMessage += F("AUTO - ");
			responseMessage += pressureInMillibar;
			responseMessage += F("#");
		} else {
			responseMessage += F("ERROR: Invalid argument - Must be between 990 and 1030 millibar!#");
		}
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("SOUND")) || message.startsWith(F("sound"))) {
		String state = message.substring(5);
		state.trim();
		if (state.startsWith(F("ON")) || state.startsWith(F("on"))) {
			_settingsUtils.storeSoundSetting(true);            
			responseMessage += F("SOUND - ON#");
		} else {
			_settingsUtils.storeSoundSetting(false);
			responseMessage += F("SOUND - OFF#");
		}		
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("METRIC")) || message.startsWith(F("metric"))) {
		String state = message.substring(6);
		state.trim();
		if (state.startsWith(F("ON")) || state.startsWith(F("on"))) {
			_settingsUtils.storeImperialUnitsSetting(false);            
			responseMessage += F("METRIC - ON#");
		} else {
			_settingsUtils.storeImperialUnitsSetting(true);
			responseMessage += F("METRIC - OFF#");
		}		
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("PRESSURE")) || message.startsWith(F("pressure"))) {
		float seaLevelPressure = message.substring(8).toFloat();
		if (980 <= seaLevelPressure && seaLevelPressure <= 1100) {						
            _settingsUtils.storeSeaLevelPressureSetting((int)seaLevelPressure);
            responseMessage += F("PRESSURE - ");
			responseMessage += seaLevelPressure;
			responseMessage += F("#");			
		} else {
			responseMessage += F("ERROR: Invalid argument - Must be between 980 and 1100 millibar!#");
		}
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("OXYGEN")) || message.startsWith(F("oxygen"))) {
		float oxygenRate = message.substring(6).toFloat();
		if (oxygenRate >= 0.16 && oxygenRate <= 0.5) {
			_settingsUtils.storeOxygenRateSetting(oxygenRate);
			responseMessage += F("OXYGEN - ");
			responseMessage += String(oxygenRate, 3);
			responseMessage += F("#");			
		} else {
			responseMessage += F("ERROR: Invalid argument - Must be between 0.21 and 0.40!#");
		}
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("DEFAULT")) || message.startsWith(F("default"))) {
		_settingsUtils.resetSettings();
		responseMessage += F("DEFAULT settings restored.#");
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("DATETIME")) || message.startsWith(F("datetime"))) {
		String dateTime = message.substring(8);
		dateTime.trim();

		int year = 2016;
		int month = 0;
		int day = 0;
		int hour = 0;
		int minute = 0;

		year = dateTime.substring(0, 4).toInt();
		month = dateTime.substring(5, 7).toInt();
		day = dateTime.substring(8, 10).toInt();
		hour = dateTime.substring(11, 13).toInt();
		minute = dateTime.substring(14, 16).toInt();

		responseMessage += F("Year: ");
		responseMessage += year;
		responseMessage += F(" Month: ");
		responseMessage += month;
		responseMessage += F(" Day: ");
		responseMessage += day;

		responseMessage += F(" Hour: ");
		responseMessage += hour;
		responseMessage += F(" Minute: ");
		responseMessage += minute;
		responseMessage += "\n";

		if (year < 2017) {
			responseMessage += F("ERROR: Invalid argument - Year must be greater than 2017!");
		} else if (month < 1 || month > 12) {
			responseMessage += F("ERROR: Invalid argument - Month must be between 1 and 12!");
		} else if (day < 1 || day > 31) {
			responseMessage += F("ERROR: Invalid argument - Day must be between 1 and 31!");
		} else if (hour < 0 || hour > 23) {
			responseMessage += F("ERROR: Invalid argument - Hour must be between 0 and 23!");
		} else if (minute < 0 || minute > 59) {
			responseMessage += F("ERROR: Invalid argument - Minute must be between 0 and 59!");
		} else {
			responseMessage += F("DATETIME - ");
			responseMessage += _timeUtils.setDateTime(year, month, day, hour, minute);
		}
		responseMessage += F("#");
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("CLEAR")) || message.startsWith(F("clear"))) {
        if (_lastDive->clearLastDiveData()) {
			responseMessage += F("CLEAR - OK");
		} else {
			responseMessage += F("ERROR: Surface time clean operation failed!");
		}
        responseMessage += F("#");
		outputStream->println(responseMessage);
	} else if (message.startsWith(F("RESET")) || message.startsWith(F("reset"))) {
		//Revert settings to default
		responseMessage += F("SETTINGS - Default settings will be restored.\n");
		_settingsUtils.resetSettings();
		
		if (_lastDive->clearLastDiveData()) {
			responseMessage += F("LAST DIVE - Last dive information was deleted.\n");
		} else {
			responseMessage += F("ERROR: Failed to delete last dive information!\n");
		}

		//TODO Remove all dive profile files
		// if (logbook.clearProfiles()) {
		// 	responseMessage += F("PROFILE - All dive profiles were successfully deleted.\n");
		// } else {
		// 	responseMessage += F("ERROR: Failed to delete all dive profiles!\n");
		// }

		//TODO Reset logbook
		// if (logbook.clearLogbook()) {
		// 	responseMessage += F("LOGBOOK - Reset was successful.\n");
		// } else {
		// 	responseMessage += F("ERROR: Logbook reset failed!\n");
		// }
		responseMessage += F("#");
		outputStream->println(responseMessage);
	}
	outputStream->flush();
}