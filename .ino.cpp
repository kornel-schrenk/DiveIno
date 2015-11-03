//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2015-11-02 14:53:38

#include "Arduino.h"
#include "SparkFun_MS5803_I2C.h"
#include "SPI.h"
#include "Wire.h"
#include "DiveDeco.h"
#include "UTFT.h"
#include "DS3231.h"
#include "IRremote.h"
void setup() ;
void loop() ;
void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) ;
void selectButtonPressed() ;
void navigationButtonPressed() ;
void diveStartButtonPressed() ;
void displayScreen(byte screen) ;
void displayMenuScreen() ;
void menuSelect(byte menuItemIndex) ;
void displayDiveScreen() ;
void displayDiveStopScreen() ;
void drawDepth(float depth) ;
void drawMaximumDepth(float maximumDepth) ;
void drawDiveDuration(int duration) ;
void drawCurrentTemperature(float currentTemperature) ;
void drawCurrentPressure(int currentPressure) ;
void drawOxigenPercentage(int oxigenPercentage) ;
void drawStaticDecoArea(bool decoNeeded) ;
void drawDecoArea(DiveInfo diveInfo) ;
void drawSafetyStop(unsigned int safetyStopDurationInSeconds) ;
void drawAscend(int ascendRate) ;

#include "DiveIno.ino"

