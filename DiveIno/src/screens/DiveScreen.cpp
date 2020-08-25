#include "screens/DiveScreen.h"

DiveScreen::DiveScreen(Buhlmann* buhlmann)
{
    _buhlmann = buhlmann;
}

void DiveScreen::displayZeroTime()
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(50, 80);

    ez.canvas.print(zeropad(0, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(0, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(0, 2));
}

void DiveScreen::displayActualTime()
{
    unsigned long elapsedTimeInSeconds = now() - _stopwatchStartTimestamp;
    _displayActualTime(elapsedTimeInSeconds);
}

void DiveScreen::_displayActualTime(unsigned long elapsedTimeInSeconds)
{
    int hours = elapsedTimeInSeconds / 3600;
    elapsedTimeInSeconds = elapsedTimeInSeconds % 3600;
    int minutes = elapsedTimeInSeconds / 60;
    int seconds = elapsedTimeInSeconds % 60;

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(50, 80);

    ez.canvas.print(zeropad(hours, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(minutes, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(seconds, 2));
}

void DiveScreen::_drawSensorData(PressureSensorData sensorData)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);

    ez.canvas.pos(45, 40);
    ez.canvas.print(sensorData.temperatureInCelsius, 0);

    ez.canvas.pos(140, 40);
    ez.canvas.print(zeropad((int)sensorData.pressureInMillibar, 4));
}

void DiveScreen::_drawCurrentDepth(float depthInMeter)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);

    ez.canvas.pos(210, 150);
    
    if (depthInMeter < 10) {
        ez.canvas.color(ez.theme->background);
        ez.canvas.print("0");    
        ez.canvas.color(ez.theme->foreground);
        ez.canvas.print(zeropad((int)depthInMeter, 1));
    } else {
        ez.canvas.print(zeropad((int)depthInMeter, 2));
    }

    ez.canvas.print(".");
    ez.canvas.print(zeropad((int)(depthInMeter*10) % 10, 1));
}

void DiveScreen::_drawMaximumDepth(float depthInMeter)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);
    
    ez.canvas.pos(260, 40);
    ez.canvas.print(depthInMeter, 1); 
}

void DiveScreen::_drawDiveIndicator()
{
    if (_currentMode == SAFE_MODE) {
        if (ez.theme->name == "Default") { 
            M5.Lcd.drawJpg((uint8_t *)ok_small_jpg, (sizeof(ok_small_jpg) / sizeof(ok_small_jpg[0])), 5, 90, 32, 32);
        }
        if (ez.theme->name == "Dark") {
            M5.Lcd.drawJpg((uint8_t *)ok_small_jpg_dark, (sizeof(ok_small_jpg_dark) / sizeof(ok_small_jpg_dark[0])), 5, 90, 32, 32);
        }
    } else if (_currentMode == SAFETY_STOP_MODE) {
        if (ez.theme->name == "Default") { 
            M5.Lcd.drawJpg((uint8_t *)safety_stop_small_jpg, (sizeof(safety_stop_small_jpg) / sizeof(safety_stop_small_jpg[0])), 5, 90, 32, 32);
        }
        if (ez.theme->name == "Dark") {
            M5.Lcd.drawJpg((uint8_t *)safety_stop_small_jpg_dark, (sizeof(safety_stop_small_jpg_dark) / sizeof(safety_stop_small_jpg_dark[0])), 5, 90, 32, 32);
        }
    } else if (_currentMode == DECO_MODE) {
        if (ez.theme->name == "Default") { 
            M5.Lcd.drawJpg((uint8_t *)deco_stop_small_jpg, (sizeof(deco_stop_small_jpg) / sizeof(deco_stop_small_jpg[0])), 5, 90, 32, 32);
        }
        if (ez.theme->name == "Dark") {
            M5.Lcd.drawJpg((uint8_t *)deco_stop_small_jpg_dark, (sizeof(deco_stop_small_jpg_dark) / sizeof(deco_stop_small_jpg_dark[0])), 5, 90, 32, 32);
        }
    } else if (_currentMode == TILT_MODE) {
        if (ez.theme->name == "Default") { 
            M5.Lcd.drawJpg((uint8_t *)tilt_small_jpg, (sizeof(tilt_small_jpg) / sizeof(tilt_small_jpg[0])), 5, 90, 32, 32);
        }
        if (ez.theme->name == "Dark") {
            M5.Lcd.drawJpg((uint8_t *)tilt_small_jpg_dark, (sizeof(tilt_small_jpg_dark) / sizeof(tilt_small_jpg_dark[0])), 5, 90, 32, 32);
        }
    }

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(10, 150);

    if (_currentMode == SAFE_MODE) {       
        if (_currentDiveInfo.minutesToDeco < 10) {
            ez.canvas.color(ez.theme->background);
            ez.canvas.print("0");    
            ez.canvas.color(ez.theme->foreground);
        } 
        ez.canvas.print(_currentDiveInfo.minutesToDeco);               
    } else if (_currentMode == SAFETY_STOP_MODE) {   
        int minutes = 0;
        int seconds = 0;
        if (_safetyStopState == SAFETY_STOP_IN_PROGRESS) {
            unsigned int duration = 0;
            if (_safetyStopDurationInSeconds <= 180) {
                duration = 180 - _safetyStopDurationInSeconds; // Counting backwards
            }
            minutes = duration / 60;
            seconds = duration % 60;  
        }       
        ez.canvas.print(zeropad(minutes, 1));
        ez.canvas.print(":");
        ez.canvas.print(zeropad(seconds, 2));
    } else if (_currentMode == DECO_MODE) {     
        if (_currentDiveInfo.decoStopInMeters < 10) {
            ez.canvas.color(ez.theme->background);
            ez.canvas.print("0");    
            ez.canvas.color(ez.theme->foreground);
        }   
        ez.canvas.print(_currentDiveInfo.decoStopInMeters);
        ez.canvas.print("-");
        ez.canvas.print(zeropad(_currentDiveInfo.decoStopDurationInMinutes, 2));            
    } else if (_currentMode == TILT_MODE) {                      
        ez.canvas.print("00");        
    }
}

void DiveScreen::init(DiveInoSettings diveInoSettings, PressureSensorData sensorData, bool replayEnabled, bool emulatorEnabled)
{
    _diveInoSettings = diveInoSettings;
    _diveStarted = false;
    _currentMode = SAFE_MODE;
    _safetyStopState = SAFETY_STOP_NOT_STARTED;

    _currentDiveInfo = DiveInfo();
    _currentDiveInfo.minutesToDeco = 99;
    _currentDiveInfo.decoNeeded = false;

    // Initialize the DiveDeco library based on the settings
    _buhlmann->setSeaLevelAtmosphericPressure(diveInoSettings.seaLevelPressureSetting);    
    _buhlmann->setNitrogenRateInGas(1 - (diveInoSettings.oxygenRateSetting + 0.01));

    _maxDepthInMeter = 0.0;

    _replayEnabled = replayEnabled;
    _emulatorEnabled = emulatorEnabled;

    ez.screen.clear();
    ez.header.show("Dive");

    if (ez.theme->name == "Default") {        
        if (diveInoSettings.imperialUnitsSetting) {      
            M5.Lcd.drawJpg((uint8_t *)fahrenheit_small_jpg, (sizeof(fahrenheit_small_jpg) / sizeof(fahrenheit_small_jpg[0])), 5, 35, 32, 32);  
        } else {      
            M5.Lcd.drawJpg((uint8_t *)celsius_small_jpg, (sizeof(celsius_small_jpg) / sizeof(celsius_small_jpg[0])), 5, 35, 32, 32);  
        }
        M5.Lcd.drawJpg((uint8_t *)pressure_small_jpg, (sizeof(pressure_small_jpg) / sizeof(pressure_small_jpg[0])), 100, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg, (sizeof(max_depth_small_jpg) / sizeof(max_depth_small_jpg[0])), 220, 35, 32, 32);        
    }
    if (ez.theme->name == "Dark") {
        if (diveInoSettings.imperialUnitsSetting) {      
            M5.Lcd.drawJpg((uint8_t *)fahrenheit_small_jpg_dark, (sizeof(fahrenheit_small_jpg_dark) / sizeof(fahrenheit_small_jpg_dark[0])), 5, 35, 32, 32);  
        } else {      
            M5.Lcd.drawJpg((uint8_t *)celsius_small_jpg_dark, (sizeof(celsius_small_jpg_dark) / sizeof(celsius_small_jpg_dark[0])), 5, 35, 32, 32);  
        }
        M5.Lcd.drawJpg((uint8_t *)pressure_small_jpg_dark, (sizeof(pressure_small_jpg_dark) / sizeof(pressure_small_jpg_dark[0])), 100, 35, 32, 32);  
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg_dark, (sizeof(max_depth_small_jpg_dark) / sizeof(max_depth_small_jpg_dark[0])), 220, 35, 32, 32);        
    }

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);

    if (_replayEnabled) {
        _isStopWatchRunning = false;
        ez.buttons.show("Menu");
    } else {
        ez.buttons.show("More # Profile # Menu");
        this->displayZeroTime();
    }

    display(sensorData);
}

///////////
// Timer //
///////////

void DiveScreen::startStopwatch()
{
    if (_stopwatchElapsedTime == 0) {
        _stopwatchStartTimestamp = now();
    }
    else {
        _stopwatchStartTimestamp = now() - _stopwatchElapsedTime;
    }
    _isStopWatchRunning = true;
}

void DiveScreen::stopStopwatch()
{
    _stopwatchElapsedTime = now() - _stopwatchStartTimestamp;
    _isStopWatchRunning = false;
}

void DiveScreen::resetStopwatch()
{
    _stopwatchElapsedTime = 0;
    _isStopWatchRunning = false;

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(50, 80);

    ez.canvas.print(zeropad(0, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(0, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(0, 2));
}

void DiveScreen::_refreshMaxDepth(float currentDepthInMeter)
{	
	if (_maxDepthInMeter <= currentDepthInMeter) {
		_maxDepthInMeter = currentDepthInMeter;

		_drawMaximumDepth(_maxDepthInMeter);
	}
}

//////////
// Dive //
//////////

void DiveScreen::_clearSafetyStopDisplay()
{
    _currentMode = SAFE_MODE;

    ez.canvas.color(ez.theme->background);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(10, 150);
     
    ez.canvas.print(zeropad(8, 1));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(88, 2));
}

void DiveScreen::_clearDecoDisplay()
{
    _currentMode = SAFE_MODE;

    ez.canvas.color(ez.theme->background);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(10, 150);
     
    ez.canvas.print(zeropad(88, 2));
    ez.canvas.print("-");
    ez.canvas.print(zeropad(88, 2));
}

void DiveScreen::_calculateSafetyStop(float maxDepthInMeter, float depthInMeter, unsigned int intervalDuration)
{
	//Safety stop calculation - only if the dive was deeper than 10 m
	if (maxDepthInMeter > 10) {

		//Between 3 and 6 meters the diver can do the safety stop
		if (3 < depthInMeter && depthInMeter < 6) {

			if (_safetyStopState == SAFETY_STOP_NOT_STARTED) {
				_safetyStopState = SAFETY_STOP_IN_PROGRESS;
                _safetyStopDurationInSeconds = 0;

				Serial.print(F(" Safety stop STARTED at "));
				Serial.println(depthInMeter);

			} else if (_safetyStopState == SAFETY_STOP_VIOLATED) {
				_safetyStopState = SAFETY_STOP_IN_PROGRESS;
                _clearSafetyStopDisplay();
			}
			if (_safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				_safetyStopDurationInSeconds += intervalDuration;

                _currentMode = SAFETY_STOP_MODE;				

				Serial.print(F(" Safety stop IN PROGRESS at "));
				Serial.println(depthInMeter);
				Serial.print(F(" Safety stop duration: "));
				Serial.println(_safetyStopDurationInSeconds);

				if (180 <= _safetyStopDurationInSeconds) {
					_safetyStopState = SAFETY_STOP_DONE;
                    _clearSafetyStopDisplay();

					Serial.print(F(" Safety stop DONE at "));
					Serial.println(depthInMeter);
				}
			}
		} else if (10 < depthInMeter) {
			//Reset the safety stop mode, if the diver descends back down to 10 m
			if (_safetyStopState != SAFETY_STOP_NOT_STARTED) {
				_safetyStopState = SAFETY_STOP_NOT_STARTED;
                
                _clearSafetyStopDisplay();
                _safetyStopDurationInSeconds = 0;

				Serial.print(F(" Safety stop RESET after DONE at "));
				Serial.println(depthInMeter);
			}
		} else {
			//Between 10-6 meters and above 3 meters the safety stop is violated - the counter will stop
			if (_safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				_safetyStopState = SAFETY_STOP_VIOLATED;
                _clearSafetyStopDisplay();

				Serial.print(F(" Safety stop VIOLATED at "));
				Serial.println(depthInMeter);
			}
		}
	}
}

void DiveScreen::startDive()
{
	if (_emulatorEnabled) {
		Serial.println(F("@START#"));
	}

	//Set default values before the dive
	_maxDepthInMeter = 0;

	_safetyStopState = SAFETY_STOP_NOT_STARTED;
	_safetyStopDurationInSeconds = 0;

	// Initialize the DiveDeco library based on the settings
    _buhlmann->setSeaLevelAtmosphericPressure(_diveInoSettings.seaLevelPressureSetting);    
    _buhlmann->setNitrogenRateInGas(1 - (_diveInoSettings.oxygenRateSetting + 0.01));

    Serial.println(F("DIVE - Started"));

    DiveResult* diveResult = new DiveResult;
    diveResult = _buhlmann->initializeCompartments();
 
    _buhlmann->startDive(diveResult, ez.clock.tz.now());
}

void DiveScreen::stopDive()
{
	if (_emulatorEnabled) {
		Serial.println(F("@STOP#"));
	}

    /////////////////////
    // Finish the dive //

    Serial.println(F("DIVE - Finished"));

    DiveResult* diveResult = _buhlmann->stopDive(ez.clock.tz.now());

    //TODO Navigate to the surface display screen
}

void DiveScreen::_diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) {
	_currentDiveInfo = _buhlmann->progressDive(pressureInMillibar, durationInSeconds);
}

/////////////
// Display //
/////////////

void DiveScreen::display(PressureSensorData sensorData)
{
    if (_replayEnabled) {
        _replayDive();
    } else {
        if (minuteChanged())
        {
            this->refreshClockWidget();
            this->displayActualTime();
        }

        if (secondChanged() && _isStopWatchRunning)
        {
            this->displayActualTime();
        }

        float currentDepthInMeter = _buhlmann->calculateDepthFromPressure(sensorData.pressureInMillibar);

        _drawSensorData(sensorData);        
        _refreshMaxDepth(currentDepthInMeter);
        _drawCurrentDepth(currentDepthInMeter);
        _drawDiveIndicator();
    }
}

void DiveScreen::_replayDive()
{
	// Check if test data is available, which comes through the serial interface
	if (Serial.available() > 0) {
		float pressureInMillibar = Serial.readStringUntil(',').toFloat();
		float depthInMeter = Serial.readStringUntil(',').toFloat();
		unsigned long diveDurationInSeconds = Serial.readStringUntil(',').toInt();
		float temperatureInCelsius = Serial.readStringUntil(',').toFloat();

		Serial.print(F("REPLAY: "));
		Serial.print(pressureInMillibar, 1);
		Serial.print(F(" mbar, "));
		Serial.print(depthInMeter, 1);
		Serial.print(F(" m, "));
		Serial.print(diveDurationInSeconds);
		Serial.print(F(" sec, "));
		Serial.print(temperatureInCelsius, 1);
		Serial.print(F(" cel\n"));

        PressureSensorData replaySensorData = PressureSensorData();
        replaySensorData.pressureInMillibar = pressureInMillibar;
        replaySensorData.temperatureInCelsius = temperatureInCelsius;

        _drawSensorData(replaySensorData);
        _drawCurrentDepth(depthInMeter);
		_refreshMaxDepth(depthInMeter);
        _displayActualTime(diveDurationInSeconds);

        if (!_diveStarted && 
                (pressureInMillibar > (_diveInoSettings.seaLevelPressureSetting + 120) || diveDurationInSeconds > 60)) {			
            this->startDive();
            _diveStarted = true;                    
		}

        if (_diveStarted) {
            if (_currentMode == SAFE_MODE || _currentMode == SAFETY_STOP_MODE) {
                _calculateSafetyStop(_maxDepthInMeter, depthInMeter, diveDurationInSeconds - _previousDiveDurationInSeconds);
                _previousDiveDurationInSeconds = diveDurationInSeconds;
            }

            //Progress with the algorithm in every 5 seconds -- in ALL modes
            if (diveDurationInSeconds % 5 == 0) {
                _diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
            }

            if (_currentDiveInfo.decoNeeded) {
                _currentMode = DECO_MODE;
            }

            if (_currentMode == DECO_MODE && !_currentDiveInfo.decoNeeded) {
                _clearDecoDisplay();
            }

            _drawDiveIndicator();            
        }        

        //Finish the dive on the surface
        if (depthInMeter == 0.0) {
            _diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);            
            this->stopDive();
            _replayEnabled = false;
        }
    }
}

/////////////
// Buttons //
/////////////

void DiveScreen::handleButtonPress(String buttonName)
{
    if (buttonName == "More")
    {
        //TODO Display more information about the current dive 
        //e.g. Time To Surface, min max temperature values etc.
    }
    else if (buttonName == "Profile")
    {
        //TODO Display Dive Profile screen
    }
}