#include "screens/SurfaceScreen.h"

long SurfaceScreen::_calculateNoFlyTime(LastDiveData* lastDiveData)
{
    if (lastDiveData != NULL) {
        long elapsedMinutes = (ez.clock.tz.now() - lastDiveData->diveDateTimestamp) / 60;
        //No Fly Time is valid in 2 days only - after it becomes 0
        if (elapsedMinutes < 2880 && (lastDiveData->noFlyTimeInMinutes - elapsedMinutes) > 0) {
            return lastDiveData->noFlyTimeInMinutes - elapsedMinutes;
        }
    }
    return 0L;
}

long SurfaceScreen::_calculateSurfaceInterval(LastDiveData* lastDiveData)
{
    if (lastDiveData != NULL) {
        long elapsedSeconds = ez.clock.tz.now() - lastDiveData->diveDateTimestamp;
        // The dive is non repetitive after 24 hours - no surface interval calculation is needed
        if (elapsedSeconds < 86400) {
            return elapsedSeconds;
        }        
    }
    return 0L;
}

void SurfaceScreen::init(DiveInoSettings diveInoSettings, LastDiveData* lastDiveData)
{
    this->_diveInoSettings = diveInoSettings;   
    this->_lastDiveData = lastDiveData; 

    Serial.print("Surface interval: ");
    Serial.println(this->_calculateSurfaceInterval(this->_lastDiveData));
    Serial.print(ez.clock.tz.now());
    Serial.print(" - ");
    Serial.println(lastDiveData->diveDateTimestamp);

    ez.screen.clear();
    ez.header.show("Surface");

    ez.buttons.show("Menu");    

    display();
}

void SurfaceScreen::_displaySurfaceInterval(unsigned long elapsedTimeInSeconds)
{
    int hours = elapsedTimeInSeconds / 3600;
    elapsedTimeInSeconds = elapsedTimeInSeconds % 3600;
    int minutes = elapsedTimeInSeconds / 60;
    int seconds = elapsedTimeInSeconds % 60;

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(50, 140);

    ez.canvas.print(zeropad(hours, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(minutes, 2));
    ez.canvas.print(":");
    ez.canvas.print(zeropad(seconds, 2));
}

void SurfaceScreen::display()
{    
    if (ez.theme->name == "Default") { 
        M5.Lcd.drawJpg((uint8_t *)ok_small_jpg, (sizeof(ok_small_jpg) / sizeof(ok_small_jpg[0])), 35, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)airplane_small_jpg, (sizeof(airplane_small_jpg) / sizeof(airplane_small_jpg[0])), 35, 75, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg, (sizeof(max_depth_small_jpg) / sizeof(max_depth_small_jpg[0])), 175, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)deco_stop_small_jpg, (sizeof(deco_stop_small_jpg) / sizeof(deco_stop_small_jpg[0])), 175, 75, 32, 32);
    }
    if (ez.theme->name == "Dark") {
        M5.Lcd.drawJpg((uint8_t *)ok_small_jpg_dark, (sizeof(ok_small_jpg_dark) / sizeof(ok_small_jpg_dark[0])), 35, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)airplane_small_jpg, (sizeof(airplane_small_jpg) / sizeof(airplane_small_jpg[0])), 35, 75, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg, (sizeof(max_depth_small_jpg) / sizeof(max_depth_small_jpg[0])), 175, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)deco_stop_small_jpg_dark, (sizeof(deco_stop_small_jpg_dark) / sizeof(deco_stop_small_jpg_dark[0])), 175, 75, 32, 32);
    }

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);

    if (this->_lastDiveData != NULL) {
        ez.canvas.pos(80, 40);
        ez.canvas.print(zeropad(this->_lastDiveData->durationInSeconds/60, 2)); //minutes
        ez.canvas.print(":");
        ez.canvas.print(zeropad(this->_lastDiveData->durationInSeconds%60, 2)); //seconds

        long noFlyTimeInMinutes = this->_calculateNoFlyTime(this->_lastDiveData);
        ez.canvas.pos(80, 80);        
        ez.canvas.print(zeropad(noFlyTimeInMinutes/60, 2)); //hours
        ez.canvas.print(":");
        ez.canvas.print(zeropad(noFlyTimeInMinutes%60, 2)); //minutes        

        ez.canvas.pos(220, 40);
        if (_diveInoSettings.imperialUnitsSetting) {
            ez.canvas.print(this->_lastDiveData->maxDepthInMeters * 3.28, 0);
            ez.canvas.print(" ft");
        } else {
            ez.canvas.print(this->_lastDiveData->maxDepthInMeters);
            ez.canvas.print(" m");
        }        

        ez.canvas.pos(220, 80);
        if (this->_lastDiveData->wasDecoDive) {
            ez.canvas.print("yes");
        } else {
            ez.canvas.print("no");
        }        
    } else {
        ez.canvas.pos(80, 40);
        ez.canvas.print("00:00");

        ez.canvas.pos(80, 80);
        ez.canvas.print("00:00");

        ez.canvas.pos(220, 40);
        ez.canvas.print("n/a");

        ez.canvas.pos(220, 80);
        ez.canvas.print("n/a");
    }
        
    this->_displaySurfaceInterval(_calculateSurfaceInterval(this->_lastDiveData));
}

void SurfaceScreen::handleButtonPress(String buttonName)
{
}