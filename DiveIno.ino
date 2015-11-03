#include "SparkFun_MS5803_I2C.h"
#include "SPI.h"
#include "Wire.h"
#include "DiveDeco.h"
#include "UTFT.h"
#include "DS3231.h"
#include "IRremote.h"

//DS3231 Real Time Clock initialization
DS3231 Clock;
bool Century=false;
bool h12 = false;
bool PM = false;

//Infrared Receiver initialization
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

//Pressure Sensor initialization
MS5803 sensor(ADDRESS_HIGH);
//Create variables to store results
float temperatureCelsius;
double pressureAbsolute, pressureSeaLevel, currentAltitude;
// Create Variable to store altitude in (m) for calculations;
double baseAltitude = 120.0; // Altitude of Ferencváros, Budapest in (m)

// Pin number for the piezo buzzer
int speakerPin = 8;

// TFT setup - 480x320 pixel

// Declare which fonts we will be using
extern uint8_t SmallFont[];       //8x12 pixel
extern uint8_t BigFont[];         //16x16 pixel
extern uint8_t SevenSegNumFont[]; //32x50 pixel

UTFT tft(HX8357B,38,39,40,41);

const char* MAX = "Max";
const char* MIN = "min";
const char* MILLIBAR_SHORT = "mb";
const char* METER_SHORT = "m";
const char* METER = "meter";
const char* CEL_SHORT = "cel";

char* mainMenu[] = {" DiveIno ",
                " Dive",
                " Logbook",
				" Surface Time",
				" Gauge",
                " Settings"};

#define MENU_SIZE (sizeof(mainMenu)/sizeof(char *))-1

// Position of first menu item from the top of the screen
#define MENU_TOP 20
// Currently elected menu item
byte selectedMenuItemIndex;

#define SURFACE_MODE 		0  //Menu, Logbook, Surface Time, Settings are available
#define DIVE_START_MODE 	1  //When the driver pressed the dive button but not below 2 meters
#define DIVE_PROGRESS_MODE 	2  //During the dive
#define DIVE_STOP_MODE 		3  //Once the diver ascended to the surface - for X minutes - the diver can go under water
#define GAUGE_MODE          4

byte currentMode = SURFACE_MODE;

#define MENU_SCREEN 		0
#define DIVE_SCREEN 	    1
#define LOGBOOK_SCREEN      2
#define SURFACE_TIME_SCREEN 3
#define GAUGE_SCREEN     	4
#define SETTINGS_SCREEN     5

byte currentScreen = MENU_SCREEN;

const float seaLevelAtmosphericPressure = 1013.25;
//Create a new instance from the library - the default atmospheric pressure on sea level was passed in milliBar
DiveDeco diveDeco = DiveDeco(seaLevelAtmosphericPressure, 0.79, 400, 56.7);

//Preferences
const bool testMode = true;

//Utility variables
DiveResult* previousDiveResult;

float depthInMeter;

float maxDepthInMeter;
float maxTemperatureInCelsius;
float minTemperatureInCelsius;

bool isDecoNeeded = true;

#define SAFETY_STOP_NOT_STARTED 0
#define SAFETY_STOP_IN_PROGRESS 1
#define SAFETY_STOP_DONE 		2
#define SAFETY_STOP_VIOLATED	3

byte safetyStopState = SAFETY_STOP_NOT_STARTED;

unsigned long safetyStopDurationTimeStamp;
unsigned int safetyStopDurationInSeconds;

unsigned long diveDurationTimeStamp;
unsigned int diveDurationInSeconds;

//The setup function is called once at startup of the sketch
void setup() {
	if (testMode) {
		Serial.begin(19200);
		Serial.println("START");
	}

	Wire.begin();         //Start the I2C interface
	irrecv.enableIRIn();  //Start the Infrared Receiver

	sensor.reset();
	sensor.begin();

	tft.InitLCD();
	tft.setFont(BigFont);
	tft.setBackColor(VGA_BLACK);
	tft.setColor(VGA_LIME);
	tft.clrScr();

	displayScreen(MENU_SCREEN);

	diveDurationTimeStamp = millis();
}

// The loop function is called in an endless loop
void loop() {

	/////////////////////
	// Button Handling //
	/////////////////////



	/////////////////////
	// Sensor handling //
	/////////////////////

	switch (currentScreen) {
		case DIVE_START_SCREEN: {
			// Read pressure from the sensor in mbar.
			float pressureInMillibar = sensor.getPressure(ADC_4096);

			//Switch to DIVE PROGRESS mode, if the diver has already descended below 2 meters
			if (pressureInMillibar > 1200) {
				diveStartButtonPressed();
			} else {
				if (pressureInMillibar > seaLevelAtmosphericPressure) {
					depthInMeter = diveDeco.calculateDepthFromPressure(pressureInMillibar);
				}
				float temperatureInCelsius = sensor.getTemperature(CELSIUS, ADC_512);

				drawCurrentTemperature(temperatureInCelsius);
				drawCurrentPressure(pressureInMillibar);
				drawDepth(depthInMeter);
			}
		}
		break;
		case DIVE_SCREEN: {
			float pressureInMillibar;
			float temperatureInCelsius;

			if (!testMode) {
				pressureInMillibar = sensor.getPressure(ADC_4096);
				depthInMeter = diveDeco.calculateDepthFromPressure(pressureInMillibar);
				temperatureInCelsius = sensor.getTemperature(CELSIUS, ADC_512);

				//Calculate dive duration
				if ((diveDurationTimeStamp / 1000) < (millis() / 1000)) {
					diveDurationInSeconds++;
					diveDurationTimeStamp = millis();
					//Progress with the algorithm in every second
					diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
				}
			} else {
				//We are in test - simulation - mode, where the data comes from the serial interface

				if (Serial.available() > 0) {
					pressureInMillibar = Serial.parseFloat();
					depthInMeter = Serial.parseFloat();
					unsigned int durationInSeconds = Serial.parseInt();
					temperatureInCelsius = Serial.parseFloat();

					diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, durationInSeconds);
				}
			}

			//Safety stop calculation - only if the dive was deeper than 10 m
			if (maxDepthInMeter > 10) {

				//Between 3 and 6 meters the diver can do the safety stop
				if (3 < depthInMeter && depthInMeter < 6) {
					if (safetyStopState == SAFETY_STOP_NOT_STARTED) {
						//Start the safety stop
						safetyStopState = SAFETY_STOP_IN_PROGRESS;
						safetyStopDurationTimeStamp = millis();
					}
					if (safetyStopState == SAFETY_STOP_VIOLATED) {
						safetyStopState = SAFETY_STOP_IN_PROGRESS;
					}
					if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
						//Calculate safety stop duration
						if ((safetyStopDurationTimeStamp / 1000) < (millis() / 1000)) {
							safetyStopDurationInSeconds--;
							safetyStopDurationTimeStamp = millis();

							//Display safety stop duration
							drawSafetyStop(safetyStopDurationInSeconds);
						}
						if (safetyStopDurationInSeconds <= 0) {
							safetyStopState = SAFETY_STOP_DONE;
							drawStaticDecoArea(isDecoNeeded);
						}
					}
				} else if (10 < depthInMeter) {
					//Reset the safety stop mode, if the diver descends back down to 10 m
					if (safetyStopState != SAFETY_STOP_NOT_STARTED) {
						safetyStopState = SAFETY_STOP_NOT_STARTED;
						safetyStopDurationInSeconds = 180;
					}
				} else {
					//Between 10-6 meters and above 3 meters the safety stop is violated - the counter will stop
					if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
						safetyStopState = SAFETY_STOP_VIOLATED;
						drawStaticDecoArea(isDecoNeeded);
					}
				}
			}
		}
		break;
	}
}

void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) {

	//Draw the data to the screen
	drawCurrentTemperature(temperatureInCelsius);
	drawCurrentPressure(pressureInMillibar);
	drawDepth(depthInMeter);
	drawDiveDuration(durationInSeconds / 60);

	//Calculate minimum and maximum values during the dive
	if (maxDepthInMeter < depthInMeter) {
		maxDepthInMeter = depthInMeter;
		drawMaximumDepth(maxDepthInMeter);
	}
	if (maxTemperatureInCelsius < temperatureInCelsius) {
		maxTemperatureInCelsius = temperatureInCelsius;
	}
	if (minTemperatureInCelsius > temperatureInCelsius) {
		minTemperatureInCelsius = temperatureInCelsius;
	}

	DiveData diveData = {pressureInMillibar, durationInSeconds};
	DiveInfo diveInfo = diveDeco.progressDive(&diveData);
	drawAscend(diveInfo.ascendRate);

	if (safetyStopState != SAFETY_STOP_IN_PROGRESS) {
		drawDecoArea(diveInfo);
	}

	if (seaLevelAtmosphericPressure >= pressureInMillibar) {
		if (testMode) {
//			Serial.println("STOP");
		}

		previousDiveResult = diveDeco.stopDive();

		//Switch to DIVE_STOP mode
		currentMode = DIVE_STOP_MODE;
		displayScreen(DIVE_STOP_SCREEN);

		//TODO Start surface time counter
	}
}

void selectButtonPressed() {
	if (currentMode == SURFACE_MODE) {
		switch (selectedMenuItemIndex) {
			case 1: {
				currentMode = DIVE_START_MODE;
				displayScreen(DIVE_START_SCREEN);
			}
			break;
			case 2 : { //Last dive
				currentMode = DIVE_STOP_MODE;
				displayScreen(DIVE_STOP_SCREEN);
			}
			break;
		}
	} else if (currentMode == DIVE_STOP_MODE || currentMode == DIVE_START_MODE) {
		//Go back to the main menu
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	}
}

void navigationButtonPressed() {
	if (currentMode == SURFACE_MODE) {
		if (selectedMenuItemIndex < MENU_SIZE) {
			// Move down the selection
			menuSelect(selectedMenuItemIndex + 1);
		} else {
			// Position to the first menu item
			menuSelect(1);
		}
	}
}

void diveStartButtonPressed() {
	if (currentMode == SURFACE_MODE) {
		currentMode = DIVE_START_MODE;
		displayScreen(DIVE_START_SCREEN);
	} else if (currentMode == DIVE_START_MODE) {
		//Start to count the duration of the dive
		diveDurationInSeconds = 0;

		//Set default values before the dive
		maxDepthInMeter = 0;
		depthInMeter = 0;
		minTemperatureInCelsius = 60;
		maxTemperatureInCelsius = 0;

		safetyStopState = SAFETY_STOP_NOT_STARTED;
		safetyStopDurationInSeconds = 180;

		if (previousDiveResult == NULL) {
			//No previous dive result is available, so do the default compartment initialization
			diveDeco.startDive(diveDeco.initializeCompartments());
		} else {
			diveDeco.startDive(previousDiveResult);
		}

		currentMode = DIVE_PROGRESS_MODE;
		displayScreen(DIVE_SCREEN);
	}
}

void displayScreen(byte screen) {
	currentScreen = screen;

	//Clear the screen
	tft.clrScr();

	switch (screen) {
		case MENU_SCREEN:
			// Draw the menu
			displayMenuScreen();
			// Select 1st menu item
			selectedMenuItemIndex = 1;
			menuSelect(selectedMenuItemIndex);
			break;
		case DIVE_SCREEN:
			displayDiveScreen();

			drawDepth(0);
			drawMaximumDepth(0);
			drawDiveDuration(0);
			drawCurrentTemperature(0);
			drawCurrentPressure(0);
			drawOxigenPercentage(21);
			break;
		case DIVE_STOP_SCREEN:
			displayDiveStopScreen();
			break;
	}
}

void displayMenuScreen() {

	// Clear the screen
	tft.clrScr();

	// Display the header of the menu - the header is the first item
	tft.setColor(VGA_LIME);
	tft.print(mainMenu[0], 0, 0);

	// Draw separation line
	tft.drawLine(0, 0, tft.getDisplayXSize(), 0);

	// Display other menu items
	tft.setColor(VGA_YELLOW);
	for (int i = 1; i <= MENU_SIZE; i++) {
		tft.print(mainMenu[i], 0, ((i - 1) * 16) + MENU_TOP);
	}
}

void menuSelect(byte menuItemIndex) {

	tft.setColor(VGA_YELLOW);

	// Remove the highlight from the currently selected menu item
	tft.setBackColor(VGA_BLACK);
	tft.print(mainMenu[selectedMenuItemIndex], 0, ((selectedMenuItemIndex - 1) * 16) + MENU_TOP);

	// Highlight the new selected menu item
	tft.setBackColor(VGA_BLUE);
	tft.print(mainMenu[menuItemIndex], 0, ((menuItemIndex - 1) * 16) + MENU_TOP);

	// Save the selection
	selectedMenuItemIndex = menuItemIndex;
}

void displayDiveScreen() {
	//TODO
}

void displayDiveStopScreen() {
	//TODO
}

void drawDepth(float depth) {
	//TODO
}

void drawMaximumDepth(float maximumDepth) {
	//TODO
}

void drawDiveDuration(int duration) {
	//TODO
}

void drawCurrentTemperature(float currentTemperature) {
	//TODO
}

void drawCurrentPressure(int currentPressure) {
	//TODO
}

void drawOxigenPercentage(int oxigenPercentage) {
	//TODO
}

void drawStaticDecoArea(bool decoNeeded) {
	//Clear deco area

	//Draw the static text
	if (!decoNeeded) {
		//tft.print("Safe");
	} else {
		//tft.print("Deco");
	}
}

void drawDecoArea(DiveInfo diveInfo) {

	if (diveInfo.decoNeeded != isDecoNeeded) {
		drawStaticDecoArea(diveInfo.decoNeeded);
	}
	isDecoNeeded = diveInfo.decoNeeded;

	//Draw the numbers, which are changing
	if (!isDecoNeeded) {
		if (diveInfo.minutesToDeco < 10) {
			//tft.print(SPACE);
		}
		//diveInfo.minutesToDeco;
	} else {
		if (diveInfo.decoStopInMeters < 10) {
			//tft.print(SPACE);
		}
		//diveInfo.decoStopInMeters;

		if (diveInfo.decoStopDurationInMinutes < 10) {
			//tft.print(SPACE);
		}
		//diveInfo.decoStopDurationInMinutes;
	}
}

void drawSafetyStop(unsigned int safetyStopDurationInSeconds) {

	//tft.print("Stay");

	int minutes = safetyStopDurationInSeconds / 60;
	int seconds = safetyStopDurationInSeconds % 60;

	if (minutes < 10) {
		//tft.print("0");
	}
	//tft.print(minutes);
	//tft.print(":");
	if (seconds < 10) {
		//tft.print("0");
	}
	//tft.print(seconds);
}

void drawAscend(int ascendRate) {

	switch (ascendRate) {
	case ASCEND_OK:
		tft.setColor(VGA_GREEN);
		//tft.print("----");
		break;
	case ASCEND_SLOW:
		tft.setColor(VGA_WHITE);
		//tft.print("---|");
		break;
	case ASCEND_NORMAL:
		tft.setColor(VGA_WHITE);
		//tft.print("--||");
		break;
	case ASCEND_ATTENTION:
		tft.setColor(VGA_YELLOW);
		//tft.print("-|||");
		break;
	case ASCEND_CRITICAL:
		tft.setColor(VGA_YELLOW);
		//tft.print("||||");
		break;
	case ASCEND_DANGER:
		tft.setColor(VGA_RED);
		//tft.print("SLOW");
		break;
	default:
		tft.setColor(VGA_GREEN);
		//tft.print("<<--");
	}
}
