#include "Arduino.h"
#include "M5Stack.h"
#include "ezTime.h"
#include "M5ez.h"
#include "Preferences.h"

#include "RTClib.h"
#include "SparkFun_MS5803_I2C.h"

#include "images/jpgs.h"
#include "images/jpgsdark.h"

#include "screens/MainMenu.h"
#include "screens/DiveInoScreen.h"
#include "screens/HomeScreen.h"
#include "screens/DiveScreen.h"
#include "screens/GaugeScreen.h"
#include "screens/LogbookScreen.h"
#include "screens/SurfaceScreen.h"

#include "pickers/SettingsPicker.h"

const String VERSION_NUMBER = "2.0.2";

bool _emulatorEnabled = false;

// Pressure Sensor initialization
MS5803 _sensor(ADDRESS_HIGH);
struct PressureSensorData _sensorData;

//TODO Use sea level pressure setting here
float _previousPressureInMillibar = 1013.2;

// Screen properties
int _currentScreen = SCREEN_HOME;
int16_t _lastPickedMainMenuIndex = 1;
bool _backToMenu = false;

/////////////
// SCREENS //
/////////////

HomeScreen homeScreen = HomeScreen();
DiveScreen diveScreen = DiveScreen();
GaugeScreen gaugeScreen = GaugeScreen();
LogbookScreen logbookScreen = LogbookScreen();
SurfaceScreen surfaceScreen = SurfaceScreen();
MainMenu mainMenuScreen = MainMenu();

SettingsPicker settingsPicker;

/////////////////////
// Utility methods //
/////////////////////

bool setTimeFromRtc()
{
  if (timeStatus() == timeNotSet || timeStatus() == timeNeedsSync)
  {
    Serial.println("Network time is unavailable");

    RTC_DS1307 rtc;
    //Real Time Clock (RTC) initialization
    if (rtc.begin())
    {
      unsigned long timerTimestamp = rtc.now().unixtime();
      Serial.println(F("RTC time available: "));

      DateTime now = rtc.now();
      Serial.print(now.year(), DEC);
      Serial.print('-');
      Serial.print(now.month(), DEC);
      Serial.print('-');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();

      setTime(timerTimestamp);
      Serial.println("RTC based time was set.");
      return true;
    }
    else
    {
      Serial.println(F("RTC is NOT available."));
      // No clock will be displayed - update has to be done manually with the Update button
    }
  }
  return false;
}

PressureSensorData readPressureSensorData(float seaLevelPressureSetting, bool emulatorEnabled)
{
  struct PressureSensorData sensorData;

  sensorData.pressureInMillibar = seaLevelPressureSetting; 
  sensorData.temperatureInCelsius = 99;

  if (emulatorEnabled) {
    Serial.println(F("@GET#"));
    if (Serial.available() > 0) {
      sensorData.pressureInMillibar = Serial.parseFloat();
      sensorData.temperatureInCelsius = Serial.parseFloat();
    }
  } else {			
    sensorData.pressureInMillibar = _sensor.getPressure(ADC_4096);
    sensorData.temperatureInCelsius = _sensor.getTemperature(CELSIUS, ADC_512);    
  }
  return sensorData;  
}

///////////////////////
// Lifecycle methods //
///////////////////////

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  M5.Power.begin();

#include <themes/default.h>
#include <themes/dark.h>
  ez.begin();

  Serial.println("\n");
  Serial.println(F(" ____   _              ___"));
  Serial.println(F("|  _ \\ (_)__   __ ___ |_ _| _ __    ___"));
  Serial.println(F("| | | || |\\ \\ / // _ \\ | | | '_ \\  / _ \\"));
  Serial.println(F("| |_| || | \\ V /|  __/ | | | | | || (_) |"));
  Serial.print(F("|____/ |_|  \\_/  \\___||___||_| |_| \\___/  "));

  Serial.print(F("v"));
  Serial.print(VERSION_NUMBER);
  Serial.println("\n");

  //Retrieve calibration constants for conversion math.
  _sensor.reset();
  _sensor.begin();

  //Disable automatic updates on the NTP server.
  //The update needs several seconds to execute, which makes the seconds counter freeze until the update
  setInterval(0);

  setTimeFromRtc();

  homeScreen.initHomeScreen(settingsPicker.getDiveInoSettings());
}

void loop()
{
  // Read temperature and pressure information in every second
  if (secondChanged()) {
    //TODO - Use the sea level pressure setting here
    _sensorData = readPressureSensorData(1013.2, false);

    //Check for sensor error - difference has to be less than 20 meters
		if (abs(_sensorData.pressureInMillibar-_previousPressureInMillibar) > 2000) {
			//This is a sensor error - skip it!!!
			_sensorData.pressureInMillibar = _previousPressureInMillibar;
		} else {
      _previousPressureInMillibar = _sensorData.pressureInMillibar;
    }		
  }

  String buttonPressed = "";
  if (!_backToMenu)
  {
    buttonPressed = ez.buttons.poll();
  }
  if (_backToMenu || buttonPressed == "Menu")
  {
    ezMenu mainMenu = mainMenuScreen.initMainMenu();
    // Set the menu selection based on the last visited menu item
    mainMenu.pickItem(_lastPickedMainMenuIndex - 1);
    // Run the stuff behind the menu item and return with its index + 1
    _lastPickedMainMenuIndex = mainMenu.runOnce();

    switch (_lastPickedMainMenuIndex)
    {
    case 0:
      _backToMenu = false;
      _currentScreen = SCREEN_HOME;
      homeScreen.initHomeScreen(settingsPicker.getDiveInoSettings());
      break;
    case 1:
      _backToMenu = false;
      _currentScreen = SCREEN_DIVE;      
      break;
    case 2:
      _backToMenu = false;
      _currentScreen = SCREEN_GAUGE;
      gaugeScreen.init(_sensorData);
      break;
    case 3:
      _backToMenu = false;
      _currentScreen = SCREEN_LOGBOOK;
      break;
    case 4:
      _backToMenu = false;
      _currentScreen = SCREEN_SURFACE;
      break;
    case 5:
      settingsPicker.runOnce("Settings");
      _backToMenu = true;
      _currentScreen = SCREEN_SETTINGS;
      break;
    case 6:
      _currentScreen = SCREEN_ABOUT;
      _backToMenu = true;
      ez.msgBox("About", "www.diveino.hu | Version: " + VERSION_NUMBER + "| Author: kornel@schrenk.hu", "Menu");
      break;
    }
  }
  else if (buttonPressed != "")
  {
    //Handle button press on the current screen
    switch (_currentScreen)
    {
    case SCREEN_HOME:
      homeScreen.handleButtonPress(buttonPressed);
      break;
    case SCREEN_DIVE:
      diveScreen.handleButtonPress(buttonPressed);
      break;
    case SCREEN_GAUGE:
      gaugeScreen.handleButtonPress(buttonPressed);
      break;
    case SCREEN_LOGBOOK:
      logbookScreen.handleButtonPress(buttonPressed);
      break;
    }
  }
  else
  {
    //NO Button was pressed - Normal operation
    switch (_currentScreen)
    {
    case SCREEN_HOME:
      homeScreen.displayHomeClock();
      break;
    case SCREEN_DIVE:
      if (minuteChanged())
      {
        diveScreen.refreshClockWidget();
      }
      break;
    case SCREEN_GAUGE:
      if (minuteChanged())
      {
        gaugeScreen.refreshClockWidget();
        gaugeScreen.display(_sensorData);
      }
      if (secondChanged()) {
        gaugeScreen.display(_sensorData);
      }
      break;
    case SCREEN_LOGBOOK:
      if (minuteChanged())
      {
        logbookScreen.refreshClockWidget();
      }
      break;
    case SCREEN_SURFACE:
      if (minuteChanged())
      {
        surfaceScreen.refreshClockWidget();
      }
      break;
    case SCREEN_SETTINGS:
      if (minuteChanged())
      {
        settingsPicker.refreshClockWidget();
      }
      break;      
    }
  }
}