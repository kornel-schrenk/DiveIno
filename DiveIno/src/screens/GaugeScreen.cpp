#include "screens/GaugeScreen.h"

GaugeScreen::GaugeScreen(Buhlmann* buhlmann)
{
    _buhlmann = buhlmann;
}

void GaugeScreen::displayZeroTime()
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

void GaugeScreen::displayActualTime()
{
    unsigned long elapsedTimeInSeconds = now() - _stopwatchStartTimestamp;
    _displayActualTime(elapsedTimeInSeconds);
}

void GaugeScreen::_displayActualTime(unsigned long elapsedTimeInSeconds)
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

void GaugeScreen::_drawSensorData(PressureSensorData sensorData)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);

    ez.canvas.pos(45, 40);
    ez.canvas.print(sensorData.temperatureInCelsius, 0);

    ez.canvas.pos(140, 40);
    ez.canvas.print(sensorData.pressureInMillibar, 0);
}

void GaugeScreen::_drawCurrentDepth(float depthInMeter)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);

    ez.canvas.pos(210, 150);
    ez.canvas.print(depthInMeter, 1);
}

void GaugeScreen::_drawMaximumDepth(float depthInMeter)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);
    
    ez.canvas.pos(260, 40);
    ez.canvas.print(depthInMeter, 1); 
}

void GaugeScreen::init(DiveInoSettings diveInoSettings, PressureSensorData sensorData, bool replayEnabled, bool emulatorEnabled)
{
    // Initialize the DiveDeco library based on the settings
    _buhlmann->setSeaLevelAtmosphericPressure(diveInoSettings.seaLevelPressureSetting);    

    _maxDepthInMeter = 0.0;

    _replayEnabled = replayEnabled;
    _emulatorEnabled = emulatorEnabled;

    ez.screen.clear();
    ez.header.show("Gauge");

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
        if (_isStopWatchRunning)
        {
            ez.buttons.show("Stop # Reset # Menu");
            this->displayActualTime();
        }
        else
        {
            ez.buttons.show("Start # Reset # Menu");
            this->displayZeroTime();
        }
    }

    display(sensorData);
}

void GaugeScreen::startStopwatch()
{
    if (_stopwatchElapsedTime == 0)
    {
        _stopwatchStartTimestamp = now();
    }
    else
    {
        _stopwatchStartTimestamp = now() - _stopwatchElapsedTime;
    }
    _isStopWatchRunning = true;
}

void GaugeScreen::stopStopwatch()
{
    _stopwatchElapsedTime = now() - _stopwatchStartTimestamp;
    _isStopWatchRunning = false;
}

void GaugeScreen::resetStopwatch()
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

void GaugeScreen::_refreshMaxDepth(float currentDepthInMeter)
{	
	if (_maxDepthInMeter <= currentDepthInMeter) {
		_maxDepthInMeter = currentDepthInMeter;

		_drawMaximumDepth(_maxDepthInMeter);
	}
}

void GaugeScreen::display(PressureSensorData sensorData)
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
    }
}

void GaugeScreen::_replayDive()
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
		Serial.println(F(" cel\n"));

        PressureSensorData replaySensorData = PressureSensorData();
        replaySensorData.pressureInMillibar = pressureInMillibar;
        replaySensorData.temperatureInCelsius = temperatureInCelsius;

        _drawSensorData(replaySensorData);
        _drawCurrentDepth(depthInMeter);
		_refreshMaxDepth(depthInMeter);
        _displayActualTime(diveDurationInSeconds);
    }
}

void GaugeScreen::handleButtonPress(String buttonName)
{
    if (buttonName == "Start")
    {
        ez.buttons.show("$Stop # Reset # Menu");
        this->startStopwatch();
        delay(300);
        ez.buttons.show("Stop # Reset # Menu");
    }
    else if (buttonName == "Stop")
    {
        ez.buttons.show("$Start # Reset # Menu");
        this->stopStopwatch();
        delay(300);
        ez.buttons.show("Start # Reset # Menu");
    }
    else if (buttonName == "Reset")
    {
        ez.buttons.show("Start # $Reset # Menu");
        this->resetStopwatch();
        delay(300);
        ez.buttons.show("Start # Reset # Menu");
    }
}