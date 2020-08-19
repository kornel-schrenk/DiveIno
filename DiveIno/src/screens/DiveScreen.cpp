#include "screens/DiveScreen.h"

#include "screens/GaugeScreen.h"

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

void DiveScreen::refreshSensorData(PressureSensorData sensorData)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);

    ez.canvas.pos(45, 40);
    ez.canvas.print(sensorData.temperatureInCelsius, 0);

    ez.canvas.pos(140, 40);
    ez.canvas.print(sensorData.pressureInMillibar, 0);

    ez.canvas.pos(260, 40);
    ez.canvas.print("0.0"); //TODO Print out the real max depth here
}

void DiveScreen::init(DiveInoSettings diveInoSettings, PressureSensorData sensorData)
{
    _currentMode = SAFE_MODE;

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

        M5.Lcd.drawJpg((uint8_t *)ok_small_jpg, (sizeof(ok_small_jpg) / sizeof(ok_small_jpg[0])), 5, 90, 32, 32);
    }
    if (ez.theme->name == "Dark") {
        if (diveInoSettings.imperialUnitsSetting) {      
            M5.Lcd.drawJpg((uint8_t *)fahrenheit_small_jpg_dark, (sizeof(fahrenheit_small_jpg_dark) / sizeof(fahrenheit_small_jpg_dark[0])), 5, 35, 32, 32);  
        } else {      
            M5.Lcd.drawJpg((uint8_t *)celsius_small_jpg_dark, (sizeof(celsius_small_jpg_dark) / sizeof(celsius_small_jpg_dark[0])), 5, 35, 32, 32);  
        }
        M5.Lcd.drawJpg((uint8_t *)pressure_small_jpg_dark, (sizeof(pressure_small_jpg_dark) / sizeof(pressure_small_jpg_dark[0])), 100, 35, 32, 32);  
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg_dark, (sizeof(max_depth_small_jpg_dark) / sizeof(max_depth_small_jpg_dark[0])), 220, 35, 32, 32);

        M5.Lcd.drawJpg((uint8_t *)ok_small_jpg_dark, (sizeof(ok_small_jpg_dark) / sizeof(ok_small_jpg_dark[0])), 5, 90, 32, 32);
    }

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);

    ez.buttons.show("TTS # Profile # Menu");
    this->displayZeroTime();

    //Dive is safe at the beginning of the dive
    ez.canvas.pos(10, 150);
    ez.canvas.print("000");

    //The current depth of the beginning of the dive is 0.0
    ez.canvas.pos(210, 150);
    ez.canvas.print("0.0");

    refreshSensorData(sensorData);
}

void DiveScreen::startStopwatch()
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

void DiveScreen::display(PressureSensorData sensorData)
{
    if (minuteChanged())
    {
        this->refreshClockWidget();
        this->displayActualTime();
    }

    if (secondChanged() && _isStopWatchRunning)
    {
        this->displayActualTime();
    }

    refreshSensorData(sensorData);
}

void DiveScreen::handleButtonPress(String buttonName)
{
    if (buttonName == "TTS")
    {
        //TODO Display TTS = Time To Surface screen
    }
    else if (buttonName == "Profile")
    {
        //TODO Display Dive Profile screen
    }
}