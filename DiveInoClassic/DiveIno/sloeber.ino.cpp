#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2019-12-25 08:49:16

#include "Arduino.h"
#include "Stream.h"
#include "SPI.h"
#include "Wire.h"
#include "UTFT.h"
#include "SdFat.h"
#include "IRremote2.h"
#include "MAX17043.h"
#include "MS5803_14.h"
#include "DS1307RTC.h"
#include "Buhlmann.h"
#include "View.h"
#include "Settings.h"
#include "Logbook.h"
#include "LastDive.h"
extern const String VERSION_NUMBER;
#define BLUETOOTH_SUPPORTED 1
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

void setup() ;
void loop() ;
void monitorWaterSwitch() ;
void activatedWaterSwitch() ;
void beep() ;
time_t nowTimestamp() ;
void readMessageFromSerial(char data, bool fromSerial) ;
void handleMessage(String message, bool fromSerial) ;
void calculateSafetyStop(float maxDepthInMeter, float depthInMeter, unsigned int intervalDuration) ;
void calculateMinMaxValues(float depthInMeter, float temperatureInCelsius) ;
void replayDive() ;
void dive() ;
void startDive() ;
void stopDive() ;
void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) ;
void processRemoteButtonPress(decode_results *results) ;
void numericButtonPressed(byte number) ;
void starButtonPressed() ;
void hashButtonPressed() ;
void selectButtonPressed() ;
void upButtonPressed() ;
void downButtonPressed() ;
void leftButtonPressed() ;
void rightButtonPressed() ;
void settingsSelect(byte settingIndex) ;
void dateTimeSettingsSelect(byte settingIndex) ;
void setSettingsToDefault() ;
void saveSettings() ;
void menuSelect(byte menuItemIndex) ;
void displayScreen(byte screen) ;

#include "DiveIno.ino"


#endif
