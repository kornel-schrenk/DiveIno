#include "Preferences.h"
#include "RTClib.h"

#include "screens/HomeScreen.h"

HomeScreen::HomeScreen(TimeUtils timeUtils)
{
  _timeUtils = timeUtils;
}

void HomeScreen::updateTime()
{    
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(numonly7seg48);
    ez.canvas.pos(50, 80);

    if (ez.clock.isClockFormat12()) {      
      ez.canvas.print(ez.clock.tz.dateTime("h:i:s"));
    } else {
      ez.canvas.print(ez.clock.tz.dateTime("H:i:s"));  
    }    
}

void HomeScreen::updateDate()
{
    String currentDate = ez.clock.tz.dateTime("Y-m-d");

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);
    ez.canvas.pos(100, 40);
    ez.canvas.print(currentDate);
}

void HomeScreen::updateAmPm()
{
  if (ez.clock.isClockFormat12() && ez.clock.isAmPmIndicatorDisplayed()) {
    ez.canvas.pos(272, 108);
    ez.canvas.print(ez.clock.tz.dateTime("A"));
  }
}

void HomeScreen::initHomeScreen(DiveInoSettings diveInoSettings) 
{ 
  _diveInoSettings = diveInoSettings;

  ez.screen.clear();
  ez.header.show("DiveIno");
  ez.buttons.show("Update # Menu # Dive");  

  if (timeStatus() == timeSet) {     
    this->updateTime();
    this->updateDate();
    this->updateAmPm();
  }

  //Draw notification icons
  if (ez.theme->name == "Default") {
    M5.Lcd.drawJpg((uint8_t *)ok_small_jpg, (sizeof(ok_small_jpg) / sizeof(ok_small_jpg[0])), 5, 160, 32, 32);
    if (_diveInoSettings.soundSetting) {
      M5.Lcd.drawJpg((uint8_t *)sound_on_small_jpg, (sizeof(sound_on_small_jpg) / sizeof(sound_on_small_jpg[0])), 40, 160, 32, 32);    
    } else {
      M5.Lcd.drawJpg((uint8_t *)sound_off_small_jpg, (sizeof(sound_off_small_jpg) / sizeof(sound_off_small_jpg[0])), 40, 160, 32, 32);    
    }    
    if (_diveInoSettings.imperialUnitsSetting) {      
      M5.Lcd.drawJpg((uint8_t *)fahrenheit_small_jpg, (sizeof(fahrenheit_small_jpg) / sizeof(fahrenheit_small_jpg[0])), 75, 160, 32, 32);  
    } else {      
      M5.Lcd.drawJpg((uint8_t *)celsius_small_jpg, (sizeof(celsius_small_jpg) / sizeof(celsius_small_jpg[0])), 75, 160, 32, 32);  
    }    
    M5.Lcd.drawJpg((uint8_t *)airplane_small_jpg, (sizeof(airplane_small_jpg) / sizeof(airplane_small_jpg[0])), 110, 160, 32, 32);
  } else if (ez.theme->name == "Dark") {
    M5.Lcd.drawJpg((uint8_t *)ok_small_jpg_dark, (sizeof(ok_small_jpg) / sizeof(ok_small_jpg_dark[0])), 5, 160, 32, 32);
    if (_diveInoSettings.soundSetting) {
      M5.Lcd.drawJpg((uint8_t *)sound_on_small_jpg_dark, (sizeof(sound_on_small_jpg_dark) / sizeof(sound_on_small_jpg_dark[0])), 40, 160, 32, 32);    
    } else {
      M5.Lcd.drawJpg((uint8_t *)sound_off_small_jpg_dark, (sizeof(sound_off_small_jpg_dark) / sizeof(sound_off_small_jpg_dark[0])), 40, 160, 32, 32);    
    }
    if (_diveInoSettings.imperialUnitsSetting) {      
      M5.Lcd.drawJpg((uint8_t *)fahrenheit_small_jpg_dark, (sizeof(fahrenheit_small_jpg_dark) / sizeof(fahrenheit_small_jpg_dark[0])), 75, 160, 32, 32);  
    } else {      
      M5.Lcd.drawJpg((uint8_t *)celsius_small_jpg_dark, (sizeof(celsius_small_jpg_dark) / sizeof(celsius_small_jpg_dark[0])), 75, 160, 32, 32);  
    }  
    M5.Lcd.drawJpg((uint8_t *)airplane_small_jpg_dark, (sizeof(airplane_small_jpg_dark) / sizeof(airplane_small_jpg_dark[0])), 110, 160, 32, 32);
  }

  ez.canvas.color(ez.theme->foreground);
  ez.canvas.font(sans26);

  //Print oxygen rate 
  ez.canvas.pos(175, 168);
  ez.canvas.print(diveInoSettings.oxygenRateSetting * 100, 0);
  ez.canvas.pos(202, 168);
  ez.canvas.print("%");  

  //Print number of dives
  ez.canvas.pos(247, 168);
  ez.canvas.print("2|328");
}

void HomeScreen::displayHomeClock()
{
  if (timeStatus() == timeSet) {
    if (minuteChanged()) {                
      this->updateTime();
      this->updateDate();
      this->updateAmPm();
      this->refreshClockWidget();             
    }

    if (secondChanged()) {    
      this->updateTime();      
    } 
  }
}

void HomeScreen::handleButtonPress(String buttonName)
{
  if (buttonName == "Update")
  {
    ez.buttons.show("$Update # Menu # Minimal");

    if (_timeUtils.updateTimeFromNTPServer()) {
      ez.msgBox("Updated", dateTime(ez.clock.tz.now(), "Y-m-d H:i:s") + "||was set.", "Ok");
    } else {
      ez.msgBox("Error", "Time update failed.", "Ok");
    }

    initHomeScreen(_diveInoSettings);
  }
  else if (buttonName == "Dive")
  {
    //TODO Show Dive Screen
  }
}