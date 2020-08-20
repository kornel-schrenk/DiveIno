#include "utils/PressureSensorUtils.h"

void PressureSensorUtils::initPressureSensor()
{
    //Retrieve calibration constants for conversion math
    _sensor.reset();
    _sensor.begin();
}

PressureSensorData PressureSensorUtils::readPressureSensorData(float seaLevelPressureSetting, bool emulatorEnabled)
{
    struct PressureSensorData sensorData;

    sensorData.pressureInMillibar = seaLevelPressureSetting;
    sensorData.temperatureInCelsius = 99;

    if (emulatorEnabled)
    {
        Serial.println(F("@GET#"));
        if (Serial.available() > 0)
        {
            sensorData.pressureInMillibar = Serial.parseFloat();
            sensorData.temperatureInCelsius = Serial.parseFloat();
        }
    }
    else
    {
        sensorData.pressureInMillibar = _sensor.getPressure(ADC_4096);
        sensorData.temperatureInCelsius = _sensor.getTemperature(CELSIUS, ADC_512);
    }

    //Check for sensor error - difference has to be less than 20 meters
	if (abs(sensorData.pressureInMillibar-_previousPressureInMillibar) > 2000) {
		//This is a sensor error - skip it!!!
		sensorData.pressureInMillibar = _previousPressureInMillibar;
	} else {
      _previousPressureInMillibar = sensorData.pressureInMillibar;
    }	

    return sensorData;
}