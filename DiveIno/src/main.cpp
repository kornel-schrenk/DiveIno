#include "Arduino.h"
#include "M5Stack.h"
#include "Wire.h"

#include "SparkFun_MS5803_I2C.h"

#include "ezTime.h"
#include "M5ez.h"

const String VERSION_NUMBER = "2.0.0";

// Pressure Sensor initialization
MS5803 _sensor(ADDRESS_HIGH);

float _pressureInMillibar = 1050.0;
float _temperatureInCelsius = 25.0;

/////////////////
// HOME SCREEN //
/////////////////

void initHomeScreen() 
{
  ez.screen.clear();
  ez.header.show("Home");
  ez.buttons.show("Dive # Menu # Power off");
}

///////////////
// MAIN MENU //
///////////////

void openDiveScreen() 
{
  ez.msgBox("Dive", "Dive screen");
}

void openLogbookScreen() 
{
  ez.msgBox("Logbook", "Logbook screen");
}

void openSurfaceScreen() 
{
  ez.msgBox("Surface", "Surface screen");
}

void openGaugeScreen()
{
  ez.msgBox("Gauge", "Gauge screen");
}

void openAboutScreen()
{
  ez.msgBox("About", "About screen");
}

ezMenu initMainMenu()
{
  ezMenu mainMenu("Menu");
  mainMenu.addItem("Dive", openDiveScreen);
  mainMenu.addItem("Logbook", openLogbookScreen);
  mainMenu.addItem("Surface", openSurfaceScreen);
  mainMenu.addItem("Gauge", openGaugeScreen);
  mainMenu.addItem("Settings", ez.settings.menu);
  mainMenu.addItem("About", openAboutScreen);
  mainMenu.addItem("Back");
  return mainMenu;
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  M5.Power.begin();

  #include <themes/default.h>
  #include <themes/dark.h>
  ezt::setDebug(INFO);
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

  _pressureInMillibar = _sensor.getPressure(ADC_4096);
  _temperatureInCelsius = _sensor.getTemperature(CELSIUS, ADC_512);  
  Serial.print(_temperatureInCelsius);
  Serial.print(" C ");
  Serial.print(_pressureInMillibar);
  Serial.println(" mb");

  //Clock settings
  //Note: Timezone name can be the default 2 letter country code - e.g. HU = Hungary

  // if (!waitForSync(120)) {  // In seconds
  //   Serial.println("No Wifi is available for time sync!");
  // }

  initHomeScreen();
}

void loop()
{
  String buttonPressed = ez.buttons.poll();
  if (buttonPressed  == "Dive") {
    //Blocks until the OK button is pressed
    openDiveScreen();    
    initHomeScreen();
  } else if (buttonPressed  == "Menu") {
    initMainMenu().run();
    initHomeScreen();
  } else if (buttonPressed  == "Power off") {
    M5.Power.powerOFF();    
  }
}