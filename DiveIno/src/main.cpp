#include "Arduino.h"
#include "M5Stack.h"
#include "ezTime.h"
#include "M5ez.h"

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

#include "api/SerialApi.h"

#include "utils/PressureSensorUtils.h"
#include "utils/SettingsUtils.h"
#include "utils/TimeUtils.h"

#include "deco/Buhlmann.h"

const String VERSION_NUMBER = "2.0.8";

struct PressureSensorData _sensorData;

//////////
// Deco //
//////////

Buhlmann buhlmann = Buhlmann(56.7);

///////////
// Utils //
///////////

SettingsUtils settingsUtils = SettingsUtils();
TimeUtils timeUtils = TimeUtils();
PressureSensorUtils pressureSensorUtils = PressureSensorUtils();

/////////
// API //
/////////

SerialApi serialApi = SerialApi(VERSION_NUMBER, settingsUtils, timeUtils);

/////////////
// SCREENS //
/////////////

// Screen properties
int _currentScreen = SCREEN_HOME;
int16_t _lastPickedMainMenuIndex = 1;
bool _backToMenu = false;

HomeScreen homeScreen = HomeScreen(timeUtils);
DiveScreen diveScreen = DiveScreen(&buhlmann);
GaugeScreen gaugeScreen = GaugeScreen(&buhlmann);
LogbookScreen logbookScreen = LogbookScreen();
SurfaceScreen surfaceScreen = SurfaceScreen();
MainMenu mainMenuScreen = MainMenu();

SettingsPicker settingsPicker;

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

  timeUtils.setTimeFromRtc();
  pressureSensorUtils.initPressureSensor();

  Serial.println("\n");
  Serial.println(F(" ____   _              ___"));
  Serial.println(F("|  _ \\ (_)__   __ ___ |_ _| _ __    ___"));
  Serial.println(F("| | | || |\\ \\ / // _ \\ | | | '_ \\  / _ \\"));
  Serial.println(F("| |_| || | \\ V /|  __/ | | | | | || (_) |"));
  Serial.print(F("|____/ |_|  \\_/  \\___||___||_| |_| \\___/  "));

  Serial.print(F("v"));
  Serial.print(VERSION_NUMBER);
  Serial.println("\n");

  homeScreen.initHomeScreen(settingsUtils.getDiveInoSettings());
}

void loop()
{
  if (secondChanged()) {
    _sensorData = pressureSensorUtils.readPressureSensorData(settingsUtils.getDefaultSeaLevelPressure(), serialApi.isEmulatorEnabled());	

    serialApi.updatePressureSensorData(_sensorData);
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
      homeScreen.initHomeScreen(settingsUtils.getDiveInoSettings());
      break;
    case 1:
      _backToMenu = false;
      _currentScreen = SCREEN_DIVE; 
      diveScreen.init(settingsUtils.getDiveInoSettings(), _sensorData, serialApi.isReplayEnabled(), serialApi.isEmulatorEnabled());    
      serialApi.reset();
      break;
    case 2:
      _backToMenu = false;
      _currentScreen = SCREEN_GAUGE;
      gaugeScreen.init(settingsUtils.getDiveInoSettings(), _sensorData, serialApi.isReplayEnabled(), serialApi.isEmulatorEnabled());
      serialApi.reset();
      break;
    case 3:
      _backToMenu = false;
      _currentScreen = SCREEN_LOGBOOK;
      logbookScreen.init(settingsUtils.getDiveInoSettings());
      break;
    case 4:
      _backToMenu = false;
      _currentScreen = SCREEN_SURFACE;
      surfaceScreen.init(settingsUtils.getDiveInoSettings());
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
    case SCREEN_SURFACE:
      surfaceScreen.handleButtonPress(buttonPressed);
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

      //Serial API - Read messages from the standard Serial interface
		  if (Serial.available() > 0) {
			  serialApi.readMessageFromSerial(Serial.read());
		  }
      break;
    case SCREEN_DIVE:
      if (minuteChanged())
      {
        diveScreen.refreshClockWidget();
        diveScreen.display(_sensorData);
      }
      if (secondChanged()) {
        diveScreen.display(_sensorData);
      }
      break;
    case SCREEN_GAUGE:
      if (minuteChanged()) {
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