#include "Preferences.h"
#include "RTClib.h"

#include "screens/HomeScreen.h"

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
    ez.canvas.pos(100, 150);
    ez.canvas.print(currentDate);
}

void HomeScreen::updateAmPm()
{
  if (ez.clock.isClockFormat12() && ez.clock.isAmPmIndicatorDisplayed()) {
    ez.canvas.pos(272, 108);
    ez.canvas.print(ez.clock.tz.dateTime("A"));
  }
}

void HomeScreen::initHomeScreen() 
{  
  ez.screen.clear();
  ez.header.show("DiveIno");
  ez.buttons.show("Update # Menu # Dive");  

  if (timeStatus() == timeSet) {     
    this->updateTime();
    this->updateDate();
    this->updateAmPm();
  }
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
    ez.buttons.show("$Update # Menu # Dive");
    updateNTP();
    if (timeStatus() == timeSet)
    {
      //Update timezone based on Preferences
      String storedTimezone = getTimezoneLocation();
      Serial.println("Stored timezone: " + storedTimezone);
      ez.clock.tz.setLocation(storedTimezone);
      Serial.println("New timezone was set to " + storedTimezone);

      this->storeTimeInRtc();

      ez.msgBox("Updated", dateTime(ez.clock.tz.now(), "Y-m-d H:i:s") + "||was set.", "Ok");
    }
    else
    {
      ez.msgBox("Error", "Time update failed.", "Ok");
    }
    initHomeScreen();
  }
  else if (buttonName == "Dive")
  {
    //TODO Show Dive Screen
  }
}

String HomeScreen::getTimezoneLocation()
{
	Preferences prefs;
	prefs.begin("M5ez", true);	// read-only
	String savedTimezone = prefs.getString("timezone", "GeoIP");
  prefs.end();
  return savedTimezone;
}

void HomeScreen::storeTimeInRtc()
{
  //Update the RTC based time
  DateTime rtcDateTime = DateTime(ez.clock.tz.now());
 
  RTC_DS1307 rtc;
  rtc.adjust(rtcDateTime);

  Serial.println(F("RTC time was set as: "));
  Serial.print(rtcDateTime.year(), DEC);
  Serial.print('-');
  Serial.print(rtcDateTime.month(), DEC);
  Serial.print('-');
  Serial.print(rtcDateTime.day(), DEC);
  Serial.print(' ');
  Serial.print(rtcDateTime.hour(), DEC);
  Serial.print(':');
  Serial.print(rtcDateTime.minute(), DEC);
  Serial.print(':');
  Serial.print(rtcDateTime.second(), DEC);
  Serial.println();
}