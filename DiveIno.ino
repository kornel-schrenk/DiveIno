#include "MS5803_I2C.h"
#include "SPI.h"
#include "Wire.h"
#include "DiveDeco.h"
#include "UTFT.h"
#include "DS3231.h"
#include "IRremote.h"
#include "SD.h"
#include "CountUpDownTimer.h"

#include "Settings.h"

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

// Pin number for the piezo buzzer
int speakerPin = 8;

// TFT setup - 480x320 pixel

// Declare which fonts we will be using
extern uint8_t BigFont[];                  //16x16 pixel
extern uint8_t SevenSegNumFontPlusPlus[];  //32x50 pixel
extern uint8_t SevenSeg_XXXL[];            //64x100 pixel
extern uint8_t Grotesk16x32[];

UTFT tft(HX8357B,38,39,40,41);

const char* MAX = "Max";
const char* MIN = "min";
const char* MILLIBAR_SHORT = "mb";
const char* METER_SHORT = "m";
const char* METER = "meter";
const char* CEL_SHORT = "cel";

char* mainMenu[] = {" DiveIno - Main Menu ",
                " Dive          ",
                " Logbook       ",
				" Surface Time  ",
				" Gauge         ",
                " Settings      ",
                " About         "};

#define MENU_SIZE (sizeof(mainMenu)/sizeof(char *))-1

// Position of first menu item from the top of the screen
#define MENU_TOP 60
// Currently elected menu item
byte selectedMenuItemIndex;

char* settingsList[] = {" Sea level",
						" Oxygen %",
						" Test mode",
						" Sound",
						" Units"};

float seaLevelPressureSetting = 1013.25;
float oxygenRateSetting = 0.21;
bool testModeSetting = true;
bool soundSetting = true;
bool imperialUnitsSetting = false;

#define SETTINGS_SIZE (sizeof(settingsList)/sizeof(char *))
#define SETTINGS_TOP 60

byte selectedSettingIndex = 0;
bool isSdCardPresent = false;
Settings settings = Settings();

#define SURFACE_MODE 		0  // Menu, Logbook, Surface Time, Settings and About are available
#define DIVE_START_MODE 	1  // When the driver pressed the dive button but not below 2 meters
#define DIVE_PROGRESS_MODE 	2  // During the dive
#define DIVE_STOP_MODE 		3  // Once the diver ascended to the surface - for X minutes - the diver can go under water
#define GAUGE_MODE          4

byte currentMode = SURFACE_MODE;

#define MENU_SCREEN 		0
#define DIVE_SCREEN 	    1
#define LOGBOOK_SCREEN      2
#define SURFACE_TIME_SCREEN 3
#define GAUGE_SCREEN     	4
#define SETTINGS_SCREEN     5
#define ABOUT_SCREEN        6

byte currentScreen = MENU_SCREEN;

DiveDeco diveDeco = DiveDeco(400, 56.7);

//Utility variables
DiveResult* previousDiveResult;

float maxDepthInMeter;
float maxTemperatureInCelsius;
float minTemperatureInCelsius;

bool isDecoNeeded = true;

#define SAFETY_STOP_NOT_STARTED 0
#define SAFETY_STOP_IN_PROGRESS 1
#define SAFETY_STOP_DONE 		2
#define SAFETY_STOP_VIOLATED	3

byte safetyStopState = SAFETY_STOP_NOT_STARTED;

unsigned int safetyStopDurationInSeconds;

CountUpDownTimer DIVE_DURATION_TIMER(UP);

//The setup function is called once at startup of the sketch
void setup() {

	// SD Card initialization
	pinMode(53, OUTPUT);
	if (SD.begin(53)) {
		isSdCardPresent = true;

		DiveInoSettings* diveInoSettings = settings.loadDiveInoSettings();
		seaLevelPressureSetting = diveInoSettings->seaLevelPressureSetting;
		oxygenRateSetting = diveInoSettings->oxygenRateSetting;
		testModeSetting = diveInoSettings->testModeSetting;
		soundSetting = diveInoSettings->soundSetting;
		imperialUnitsSetting = diveInoSettings->imperialUnitsSetting;
	}

	// Initialize the DiveDeco library based on the settings
	diveDeco.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
	diveDeco.setNitrogenRateInGas(1 - oxygenRateSetting);

	Serial.begin(19200);
	Serial.println("");
	Serial.println("DiveIno - START");
	Serial.println("");

	if (testModeSetting) {

		Serial.print("SD Card present: ");
		if (isSdCardPresent) {
			Serial.println("YES");
		} else {
			Serial.println("NO");
		}
		Serial.println("");

		Serial.println("Settings: ");
		Serial.println("");
		Serial.print("seaLevelPressure: ");
		Serial.println(seaLevelPressureSetting, 2);
		Serial.print("oxygenRate: ");
		Serial.println(oxygenRateSetting, 2);

		Serial.print("testMode: ");
		if (testModeSetting) {
			Serial.println("On");
		} else {
			Serial.println("Off");
		}

		Serial.print("sound: ");
		if (soundSetting) {
			Serial.println("On");
		} else {
			Serial.println("Off");
		}

		Serial.print("units: ");
		if (imperialUnitsSetting) {
			Serial.println("Imperial");
		} else {
			Serial.println("Metric");
		}
		Serial.println("");
	}

	Wire.begin();         //Start the I2C interface
	irrecv.enableIRIn();  //Start the Infrared Receiver

	sensor.reset();
	sensor.begin();

	tft.InitLCD();
	tft.clrScr();

	displayScreen(MENU_SCREEN);
}

void loop() {

	/////////////////////
	// Button Handling //
	/////////////////////

	if (irrecv.decode(&results)) {
		processRemoteButtonPress(&results);
		irrecv.resume(); // Receive the next value
	}

	/////////////////////
	// Sensor handling //
	/////////////////////

	float pressureInMillibar;
	float temperatureInCelsius;
	float depthInMeter;

	unsigned long diveDurationInSeconds;

	switch (currentScreen) {
		case GAUGE_SCREEN: {
			if (!testModeSetting) {
				if (DIVE_DURATION_TIMER.TimeHasChanged()) {

					diveDurationInSeconds = DIVE_DURATION_TIMER.ShowTotalSeconds();

					pressureInMillibar = sensor.getPressure(ADC_4096);
					if (pressureInMillibar > seaLevelPressureSetting) {
						depthInMeter = diveDeco.calculateDepthFromPressure(pressureInMillibar);
					} else {
						depthInMeter = 0;
					}
					temperatureInCelsius = sensor.getTemperature(CELSIUS, ADC_512);

					drawCurrentPressure(pressureInMillibar);
					drawDepth(depthInMeter);
					drawCurrentTemperature(temperatureInCelsius);

					drawDiveDuration(diveDurationInSeconds);
					drawCurrentTime();

					//Calculate minimum and maximum values during the dive
					if (maxDepthInMeter < depthInMeter) {
						maxDepthInMeter = depthInMeter;
						drawMaximumDepth(maxDepthInMeter);
					}
				}
			} else {
				//We are in test - simulation - mode, where the data comes from the serial interface
				if (Serial.available() > 0) {
					pressureInMillibar = Serial.parseFloat();
					depthInMeter = Serial.parseFloat();
					diveDurationInSeconds = Serial.parseInt();
					temperatureInCelsius = Serial.parseFloat();

					Serial.print("Pressure: ");
					Serial.print(pressureInMillibar, 2);
					drawCurrentPressure(pressureInMillibar);

					Serial.print(" Depth: ");
					Serial.print(depthInMeter, 2);
					drawDepth(depthInMeter);

					Serial.print(" Temperature: ");
					Serial.print(temperatureInCelsius, 2);
					drawCurrentTemperature(temperatureInCelsius);

					Serial.print(" Duration: ");
					Serial.println(diveDurationInSeconds);
					drawDiveDuration(diveDurationInSeconds);

					drawCurrentTime();

					//Calculate minimum and maximum values during the dive
					if (maxDepthInMeter < depthInMeter) {
						maxDepthInMeter = depthInMeter;
						drawMaximumDepth(maxDepthInMeter);
					}
				}
			}
		}
		break;
		case DIVE_SCREEN: {
			if (!testModeSetting) {
				if (DIVE_DURATION_TIMER.TimeHasChanged()) {
					pressureInMillibar = sensor.getPressure(ADC_4096);
					depthInMeter = diveDeco.calculateDepthFromPressure(pressureInMillibar);
					temperatureInCelsius = sensor.getTemperature(CELSIUS, ADC_512);
					diveDurationInSeconds = DIVE_DURATION_TIMER.ShowTotalSeconds();

					//The timer fires in every second
					calculateSafetyStop(maxDepthInMeter, depthInMeter, 1);

					//Progress with the algorithm in every second
					diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
				}
			} else {
				//We are in test - simulation - mode, where the data comes from the serial interface
				if (Serial.available() > 0) {
					pressureInMillibar = Serial.parseFloat();
					depthInMeter = Serial.parseFloat();
					unsigned int testDiveDurationInSeconds = Serial.parseInt();
					temperatureInCelsius = Serial.parseFloat();

					unsigned int intervalDuration = 0;
					//Calculate the delta between the current and the previous duration information
					if (diveDurationInSeconds < testDiveDurationInSeconds) {
						intervalDuration = testDiveDurationInSeconds - diveDurationInSeconds;
					}
					diveDurationInSeconds = testDiveDurationInSeconds;

					Serial.print(" Test data available at ");
					Serial.println(depthInMeter);

					calculateSafetyStop(maxDepthInMeter, depthInMeter, intervalDuration);
					diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
				}
			}
		}
		break;
	}
}

void calculateSafetyStop(float maxDepthInMeter, float depthInMeter, unsigned int intervalDuration)
{
	//Safety stop calculation - only if the dive was deeper than 10 m
	if (maxDepthInMeter > 10) {

		//Between 3 and 6 meters the diver can do the safety stop
		if (3 < depthInMeter && depthInMeter < 6) {

			if (safetyStopState == SAFETY_STOP_NOT_STARTED) {
				Serial.print(" Safety stop STARTED at ");
				Serial.println(depthInMeter);

				safetyStopState = SAFETY_STOP_IN_PROGRESS;

			} else if (safetyStopState == SAFETY_STOP_VIOLATED) {
				safetyStopState = SAFETY_STOP_IN_PROGRESS;
			}
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				Serial.print(" Safety stop IN PROGRESS at ");
				Serial.println(depthInMeter);

				safetyStopDurationInSeconds += intervalDuration;

				Serial.print(" Safety stop duration: ");
				Serial.println(safetyStopDurationInSeconds);

				//Display safety stop duration
				drawSafetyStop(safetyStopDurationInSeconds);

				if (180 <= safetyStopDurationInSeconds) {
					Serial.print(" Safety stop DONE at ");
					Serial.println(depthInMeter);

					safetyStopState = SAFETY_STOP_DONE;
				}
			}
		} else if (10 < depthInMeter) {
			//Reset the safety stop mode, if the diver descends back down to 10 m
			if (safetyStopState != SAFETY_STOP_NOT_STARTED) {
				Serial.print(" Safety stop RESET after DONE at ");
				Serial.println(depthInMeter);

				safetyStopState = SAFETY_STOP_NOT_STARTED;
			}
		} else {
			//Between 10-6 meters and above 3 meters the safety stop is violated - the counter will stop
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				Serial.print(" Safety stop VIOLATED at ");
				Serial.println(depthInMeter);
				safetyStopState = SAFETY_STOP_VIOLATED;
			}
		}
	}
}

void startDive()
{
	if (!testModeSetting) {
		DIVE_DURATION_TIMER.StartTimer();
	}

	//Set default values before the dive
	maxDepthInMeter = 0;
	minTemperatureInCelsius = 60;
	maxTemperatureInCelsius = 0;

	safetyStopState = SAFETY_STOP_NOT_STARTED;
	safetyStopDurationInSeconds = 0;

//	if (previousDiveResult == NULL) {
//		//No previous dive result is available, so do the default compartment initialization
//		diveDeco.startDive(diveDeco.initializeCompartments());
//	} else {
//		diveDeco.startDive(previousDiveResult);
//	}

	//currentMode = DIVE_PROGRESS_MODE;
}

void stopDive()
{
	if (!testModeSetting) {
		DIVE_DURATION_TIMER.StopTimer();
	}
}

void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) {

	//Draw the data to the screen
	drawCurrentTemperature(temperatureInCelsius);
	drawCurrentPressure(pressureInMillibar);
	drawDepth(depthInMeter);
	drawDiveDuration(durationInSeconds);

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

//	DiveData diveData = {pressureInMillibar, durationInSeconds};
//	DiveInfo diveInfo = diveDeco.progressDive(&diveData);
//	drawAscend(diveInfo.ascendRate);
//
//	if (safetyStopState != SAFETY_STOP_IN_PROGRESS) {
//		drawDecoArea(diveInfo);
//	}
//
//	if (seaLevelPressureSetting >= pressureInMillibar) {
//		if (testModeSetting) {
//			Serial.println("STOP");
//		}
//
//		previousDiveResult = diveDeco.stopDive();
//
//		//Switch to DIVE_STOP mode
//		currentMode = DIVE_STOP_MODE;
//		//displayScreen(DIVE_STOP_SCREEN);
//
//		//TODO Start surface time counter
//	}
}

////////////////////////////////////////////////////////////////////

void processRemoteButtonPress(decode_results *results) {
	if (results->value == 0xFD8877 || results->value == 0xFF629D) {
		if (testModeSetting) {
			Serial.println("Up");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		upButtonPressed();
	} else if (results->value == 0xFD9867 || results->value == 0xFFA857) {
		if (testModeSetting) {
			Serial.println("Down");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		downButtonPressed();
	} else if (results->value == 0xFD28D7) {
		if (testModeSetting) {
			Serial.println("Left");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		leftButtonPressed();
	} else if (results->value == 0xFD6897) {
		if (testModeSetting) {
			Serial.println("Right");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		rightButtonPressed();
	} else if (results->value == 0xFDA857 || results->value == 0xFF02FD) {
		if (testModeSetting) {
			Serial.println("OK");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		selectButtonPressed();
	} else if (results->value == 0xFD30CF) {
		if (testModeSetting) {
			Serial.println("*");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		asterixButtonPressed();
	} else if (results->value == 0xFD708F) {
		if (testModeSetting) {
			Serial.println("#");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		hashButtonPressed();
	} else if (results->value == 0xFF6897 || results->value == 0xFDB04F) {
		numericButtonPressed(0);
	} else if (results->value == 0xFF30CF || results->value == 0xFD00FF) {
		numericButtonPressed(1);
	} else if (results->value == 0xFF18E7 || results->value == 0xFD807F) {
		numericButtonPressed(2);
	} else if (results->value == 0xFF7A85 || results->value == 0xFD40BF) {
		numericButtonPressed(3);
    } else if (results->value == 0xFF10EF || results->value == 0xFD20DF) {
    	numericButtonPressed(4);
    } else if (results->value == 0xFF38C7 || results->value == 0xFDA05F) {
    	numericButtonPressed(5);
    } else if (results->value == 0xFF5AA5 || results->value == 0xFD609F) {
    	numericButtonPressed(6);
    } else if (results->value == 0xFF42BD || results->value == 0xFD10EF) {
    	numericButtonPressed(7);
    } else if (results->value == 0xFF4AB5 || results->value == 0xFD906F) {
    	numericButtonPressed(8);
    } else if (results->value == 0xFF52AD || results->value == 0xFD50AF) {
    	numericButtonPressed(9);
    }
}

void numericButtonPressed(byte number) {
	if (testModeSetting) {
		Serial.println(number);
	}
	if (soundSetting) {
		tone(speakerPin, 261, 10);
	}

	// TODO
}

void hashButtonPressed()
{
	if (currentScreen == SETTINGS_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	}
}

void asterixButtonPressed()
{
	// DO NOTHING
}

// This is the OK or the Prev button on the IR Remote Control
void selectButtonPressed()
{
	if (currentScreen == MENU_SCREEN) {
		switch (selectedMenuItemIndex) {
			case 1: { // Dive
				currentMode = DIVE_START_MODE;
				startDive();
				displayScreen(DIVE_SCREEN);
			}
			break;
			case 2: { // Logbook
				currentMode = SURFACE_MODE;
				displayScreen(LOGBOOK_SCREEN);
			}
			break;
			case 3: { // Surface time
				currentMode = SURFACE_MODE;
				displayScreen(SURFACE_TIME_SCREEN);
			}
			break;
			case 4: { // Gauge
				currentMode = GAUGE_MODE;
				displayScreen(GAUGE_SCREEN);
			}
			break;
			case 5: { // Settings
				currentMode = SURFACE_MODE;
				displayScreen(SETTINGS_SCREEN);
			}
			break;
			case 6: { // About
				currentMode = SURFACE_MODE;
				displayScreen(ABOUT_SCREEN);
			}
			break;
		}
	} else if (currentScreen == DIVE_SCREEN) {
		currentMode = SURFACE_MODE;
		stopDive();
		displayScreen(MENU_SCREEN);
	} else if (currentScreen == LOGBOOK_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	} else if (currentScreen == SURFACE_TIME_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	} else if (currentScreen == GAUGE_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	} else if (currentScreen == SETTINGS_SCREEN) {
		if (selectedSettingIndex == 5) { // Save
			currentMode = SURFACE_MODE;
			displayScreen(MENU_SCREEN);
			saveSettings();
		}
		if (selectedSettingIndex == 6) { // Default
			currentMode = SURFACE_MODE;
			displayScreen(MENU_SCREEN);
			setSettingsToDefault();
		}
	} else if (currentScreen == ABOUT_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	}
}

// This is the Up or the Mode button on the IR Remote Control
void upButtonPressed()
{
	switch (currentScreen) {
		case MENU_SCREEN: {
			if (selectedMenuItemIndex == 1) {
				// Position to the last menu item
				menuSelect(MENU_SIZE);
			} else {
				// Move down the selection
				menuSelect(selectedMenuItemIndex - 1);
			}
		}
		break;
		case SETTINGS_SCREEN: {
			if (selectedSettingIndex == 0) {
				settingsSelect(6);
			} else {
				settingsSelect(selectedSettingIndex - 1);
			}
		}
		break;
	}
}

// This is the Down or the - button on the IR Remote Control
void downButtonPressed()
{
	switch (currentScreen) {
		case MENU_SCREEN: {
			if (selectedMenuItemIndex < MENU_SIZE) {
				// Move down the selection
				menuSelect(selectedMenuItemIndex + 1);
			} else {
				// Position to the first menu item
				menuSelect(1);
			}
		}
		break;
		case SETTINGS_SCREEN: {
			if (selectedSettingIndex < 6) {
				settingsSelect(selectedSettingIndex + 1);
			} else {
				settingsSelect(0);
			}
		}
		break;
	}
}

void leftButtonPressed()
{
	switch (currentScreen) {
		case SETTINGS_SCREEN: {
			switch (selectedSettingIndex) {
				case 0: {
					if (seaLevelPressureSetting > 1001) {
						seaLevelPressureSetting = seaLevelPressureSetting - 0.05;
					}
					settingsSelect(0);
				}
				break;
				case 1: {
					if (oxygenRateSetting > 0.16) {
						oxygenRateSetting = oxygenRateSetting - 0.01;
					}
					settingsSelect(1);
				}
				break;
				case 2: {
					testModeSetting = !testModeSetting;
					settingsSelect(2);
				}
				break;
				case 3: {
					soundSetting = !soundSetting;
					settingsSelect(3);
				}
				break;
				case 4: {
					imperialUnitsSetting = !imperialUnitsSetting;
					settingsSelect(4);
				}
				break;
			}
		}
		break;
	}
}

void rightButtonPressed()
{
	switch (currentScreen) {
		case SETTINGS_SCREEN: {
			switch (selectedSettingIndex) {
				case 0: {
					if (seaLevelPressureSetting < 1100) {
						seaLevelPressureSetting = seaLevelPressureSetting + 0.05;
					}
					settingsSelect(0);
				}
				break;
				case 1: {
					if (oxygenRateSetting < 0.50) {
						oxygenRateSetting = oxygenRateSetting + 0.01;
					}
					settingsSelect(1);
				}
				break;
				case 2: {
					testModeSetting = !testModeSetting;
					settingsSelect(2);
				}
				break;
				case 3: {
					soundSetting = !soundSetting;
					settingsSelect(3);
				}
				break;
				case 4: {
					imperialUnitsSetting = !imperialUnitsSetting;
					settingsSelect(4);
				}
				break;
			}
		}
		break;
	}
}

///////////////////////////////////////////////////////////////

void menuSelect(byte menuItemIndex)
{
	tft.setColor(VGA_YELLOW);

	// Remove the highlight from the currently selected menu item
	tft.setBackColor(VGA_BLACK);
	tft.print(mainMenu[selectedMenuItemIndex], 0, ((selectedMenuItemIndex - 1) * 40) + MENU_TOP);

	// Highlight the new selected menu item
	tft.setBackColor(VGA_BLUE);
	tft.print(mainMenu[menuItemIndex], 0, ((menuItemIndex - 1) * 40) + MENU_TOP);

	// Save the selection
	selectedMenuItemIndex = menuItemIndex;
}

///////////////////////////////////////////////////////////////////////////////

void displayScreen(byte screen) {
	currentScreen = screen;
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
			break;
		case LOGBOOK_SCREEN:
			displayLogbookScreen();
			break;
		case SURFACE_TIME_SCREEN:
			displaySurfaceTimeScreen();
			break;
		case GAUGE_SCREEN:
			displayGaugeScreen();
			break;
		case SETTINGS_SCREEN:
			displaySettingsScreen(0);
			break;
		case ABOUT_SCREEN:
			displayAboutScreen();
			break;
	}
}

void displayMenuScreen()
{
	tft.clrScr();
	tft.setBackColor(VGA_BLACK);
	tft.setFont(Grotesk16x32);

	// Display the header of the menu - the header is the first item
	tft.setColor(VGA_LIME);
	tft.print(mainMenu[0], 48, 10);

	// Draw separation line
	tft.drawLine(0, MENU_TOP-10, tft.getDisplayXSize()-1, MENU_TOP-10);

	// Display other menu items
	tft.setColor(VGA_YELLOW);
	for (int i = 1; i <= MENU_SIZE; i++) {
		tft.print(mainMenu[i], 0, ((i - 1) * 40) + MENU_TOP);
	}

	//Reset the maximum depth
	maxDepthInMeter = 0;
}

void displayDiveScreen()
{
	tft.clrScr();

	tft.setColor(VGA_SILVER);
	tft.setBackColor(VGA_BLACK);
	tft.drawRect(0, 0, tft.getDisplayXSize()-1, tft.getDisplayYSize()-1);

	tft.drawLine(0, 190, tft.getDisplayXSize()-1, 190);
	tft.drawLine(220, 0, 220, 190);
}

void displayLogbookScreen()
{
	tft.clrScr();
	tft.setBackColor(VGA_BLACK);
	tft.setFont(Grotesk16x32);

	tft.setColor(VGA_MAROON);
	tft.print("Logbook", CENTER, 100);
}

void displaySurfaceTimeScreen()
{
	tft.clrScr();
	tft.setBackColor(VGA_BLACK);
	tft.setFont(Grotesk16x32);

	tft.setColor(VGA_MAROON);
	tft.print("Surface Time", CENTER, 100);
}

void displayGaugeScreen()
{
	tft.clrScr();
	tft.setColor(VGA_SILVER);
	tft.setBackColor(VGA_BLACK);
	tft.drawRect(0, 0, tft.getDisplayXSize()-1, tft.getDisplayYSize()-1);

	tft.drawLine(0, 190, tft.getDisplayXSize()-1, 190);
	tft.drawLine(220, 0, 220, 190);

	int paddingLeft = 50;

	tft.setFont(Grotesk16x32);
	tft.setColor(VGA_GREEN);
	tft.print("GAUGE", paddingLeft + tft.getFontXsize() * 4, 215);

	if (testModeSetting) {
		tft.setColor(VGA_RED);
		tft.print("TEST", paddingLeft + tft.getFontXsize() * 15, 215);
	}
}

void displaySettingsScreen(byte selectionIndex)
{
	// Settings:
	//
	// From SETTINGS.TXT:
	//  seaLevelPressure = 1013.25
	//  oxygenRate = 0.21
	//  testMode = 1 :  0 = Off, 1 = On
	//  sound = 1 : 0 = Off, 1 = On
	//  units = 0 : 0 = metric, 1 = imperial
	//
	// Real Time Clock settings - for this a separate application can be used:
	//  time = 18:05
	//  date = 2015-11-14

	tft.clrScr();
	tft.setBackColor(VGA_BLACK);
	tft.setFont(Grotesk16x32);

	// Display the header of the screen
	tft.setColor(VGA_LIME);
	tft.print("DiveIno - Settings", 48, 10);

	// Draw separation line
	tft.drawLine(0, SETTINGS_TOP-10, tft.getDisplayXSize()-1, SETTINGS_TOP-10);

	// Display settings
	tft.setColor(VGA_YELLOW);
	for (int i = 0; i < SETTINGS_SIZE; i++) {
		tft.print(settingsList[i], 0, (i * 40) + SETTINGS_TOP);
	}

	displaySettings(selectionIndex);
}

void settingsSelect(byte settingIndex)
{
	displaySettings(settingIndex);
	selectedSettingIndex = settingIndex;
}

void displaySettings(byte settingIndex)
{
	if (settingIndex == 0) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_AQUA);
		tft.setBackColor(VGA_BLACK);
	}
	tft.printNumF(seaLevelPressureSetting, 2, 240, SETTINGS_TOP);

	if (settingIndex == 1) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_AQUA);
		tft.setBackColor(VGA_BLACK);
	}
	tft.printNumF(oxygenRateSetting, 2, 240, 40 + SETTINGS_TOP);

	if (settingIndex == 2) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_AQUA);
		tft.setBackColor(VGA_BLACK);
	}
	if (testModeSetting) {
		tft.print("On ", 240, 80 + SETTINGS_TOP);
	} else {
		tft.print("Off", 240, 80 + SETTINGS_TOP);
	}

	if (settingIndex == 3) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_AQUA);
		tft.setBackColor(VGA_BLACK);
	}
	if (soundSetting) {
		tft.print("On ", 240, 120 + SETTINGS_TOP);
	} else {
		tft.print("Off", 240, 120 + SETTINGS_TOP);
	}

	if (settingIndex == 4) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_AQUA);
		tft.setBackColor(VGA_BLACK);
	}
	if (imperialUnitsSetting) {
		tft.print("Imperial", 240, 160 + SETTINGS_TOP);
	} else {
		tft.print("Metric  ", 240, 160 + SETTINGS_TOP);
	}

	if (settingIndex == 5) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_GREEN);
		tft.setBackColor(VGA_BLACK);
	}
	tft.print("Save", 120, 210 + SETTINGS_TOP);

	if (settingIndex == 6) {
		tft.setColor(VGA_WHITE);
		tft.setBackColor(VGA_BLUE);
	} else {
		tft.setColor(VGA_MAROON);
		tft.setBackColor(VGA_BLACK);
	}
	tft.print("Default", 210, 210 + SETTINGS_TOP);
}

void setSettingsToDefault()
{
	seaLevelPressureSetting = 1013.25;
	oxygenRateSetting = 0.21;
	testModeSetting = true;
	soundSetting = true;
	imperialUnitsSetting = false;
}

void saveSettings()
{
	if (isSdCardPresent) {
		DiveInoSettings* diveInoSettings = new DiveInoSettings;
		diveInoSettings->seaLevelPressureSetting = seaLevelPressureSetting;
		diveInoSettings->oxygenRateSetting = oxygenRateSetting;
		diveInoSettings->testModeSetting = testModeSetting;
		diveInoSettings->soundSetting = soundSetting;
		diveInoSettings->imperialUnitsSetting = imperialUnitsSetting;
		settings.saveDiveInoSettings(diveInoSettings);

		if (diveInoSettings->testModeSetting) {
			Serial.println("Settings were saved to the SD Card.");
		}
	}

	diveDeco.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
	diveDeco.setNitrogenRateInGas(1 - oxygenRateSetting);
}

void displayAboutScreen()
{
	tft.clrScr();
	tft.setBackColor(VGA_BLACK);
	tft.setFont(Grotesk16x32);

	tft.setColor(VGA_YELLOW);
	tft.print("DiveIno", CENTER, 100);
	tft.setColor(VGA_WHITE);
	tft.print("Version: 1.3.1", CENTER, 150);
	tft.setColor(VGA_GREEN);
	tft.print("kornel@schrenk.hu", CENTER, 200);
}

void drawDepth(float depth)
{
	if (currentScreen == DIVE_SCREEN || currentScreen == GAUGE_SCREEN) {
		tft.setFont(SevenSeg_XXXL);
		tft.setColor(VGA_YELLOW);

		int paddingLeft = 240;
		int paddingTop = 15;

		tft.printNumI(depth, paddingLeft, paddingTop, 2, '/'); //Two digits - the / is the SPACE in the font

		// Draw the decimal point
		tft.fillRect(paddingLeft + tft.getFontXsize()*2 + 2, paddingTop + tft.getFontYsize() - 10, paddingLeft + tft.getFontXsize()*2 + 10, paddingTop + tft.getFontYsize() - 2);
		// Draw the first decimal fraction digit
		tft.printNumI(((int)(depth*10))%10, paddingLeft + tft.getFontXsize()*2 + 12, paddingTop, 1); //One digits

		tft.setFont(BigFont);
		tft.setColor(VGA_SILVER);
		tft.print("m", paddingLeft + 64*3 + 12, 100 + paddingTop - tft.getFontYsize());
	}
}

void drawMaximumDepth(float maximumDepth)
{
	if (currentScreen == DIVE_SCREEN || currentScreen == GAUGE_SCREEN) {
		tft.setFont(SevenSegNumFontPlusPlus);
		tft.setColor(VGA_PURPLE);

		int paddingLeft = 10; // 10
		int paddingTop = 130;  // 10 + 50 + 10 + 50 + 10

		tft.printNumF(maximumDepth, 1, paddingLeft, paddingTop, '.', 4, '/');

		tft.setFont(BigFont);
		tft.setColor(VGA_SILVER);
		tft.print("m", paddingLeft + 32*4 + 5, paddingTop + 50 - tft.getFontYsize());
	}
}

void drawCurrentTemperature(float currentTemperature)
{
	if (currentScreen == DIVE_SCREEN || currentScreen == GAUGE_SCREEN) {
		tft.setFont(SevenSegNumFontPlusPlus);
		tft.setColor(VGA_LIME);

		int paddingLeft = 74; // 10 + 2*32
		int paddingTop = 10;

		tft.printNumI(currentTemperature, paddingLeft, paddingTop, 2, '/'); // Two digits - the / is the SPACE in the font

		tft.setFont(BigFont);
		tft.setColor(VGA_SILVER);
		tft.print("cel", paddingLeft + 32*2 + 5, paddingTop + 50 - tft.getFontYsize());
	}
}

void drawCurrentPressure(int currentPressure)
{
	if (currentScreen == DIVE_SCREEN || currentScreen == GAUGE_SCREEN) {
		tft.setFont(SevenSegNumFontPlusPlus);
		tft.setColor(VGA_AQUA);

		int paddingLeft = 10; // 10
		int paddingTop = 70;  // 10 + 50 + 10

		tft.printNumI(currentPressure, paddingLeft, paddingTop, 4, '/');

		tft.setFont(BigFont);
		tft.setColor(VGA_SILVER);
		tft.print("mBar", paddingLeft + 32*4 + 5, paddingTop + 50 - tft.getFontYsize());
	}
}

void drawDiveDuration(int duration) // The dive duration is always in seconds
{
	if (currentScreen == DIVE_SCREEN || currentScreen == GAUGE_SCREEN) {
		tft.setFont(SevenSegNumFontPlusPlus);
		tft.setColor(VGA_WHITE);

		byte hours = duration / 3600;
		duration = duration % 3600;
		byte minutes = duration / 60;
		byte seconds = duration % 60;

		int paddingLeft = 240;
		int paddingTop = 130;

		tft.printNumI(seconds, paddingLeft + tft.getFontXsize() * 5, paddingTop, 2, '0');
		tft.print(":", paddingLeft + tft.getFontXsize() * 4, paddingTop, 1);
		tft.printNumI(minutes, paddingLeft + tft.getFontXsize() * 2, paddingTop, 2, '0');
		tft.print(":", paddingLeft + tft.getFontXsize() * 1, paddingTop, 1);
		tft.printNumI(hours, paddingLeft, paddingTop, 1); // Max 9 hours
	}
}

void drawCurrentTime()
{
	if (currentScreen == GAUGE_SCREEN) {
		tft.setFont(Grotesk16x32);
		tft.setColor(VGA_TEAL);

		byte year, month, date, DoW, hour, minute, second;
		Clock.getTime(year, month, date, DoW, hour, minute, second);

		int paddingLeft = 50;
		int paddingTop = 265;

		tft.print("20", paddingLeft + tft.getFontXsize(), paddingTop);
		tft.printNumI(year, paddingLeft + tft.getFontXsize() * 3, paddingTop, 2, '0');

		tft.print("-", paddingLeft + tft.getFontXsize() * 5, paddingTop);
		tft.printNumI(month, paddingLeft + tft.getFontXsize() * 6, paddingTop, 2, '0');
		tft.print("-", paddingLeft + tft.getFontXsize() * 8, paddingTop);
		tft.printNumI(date, paddingLeft + tft.getFontXsize() * 9, paddingTop, 2, '0');

		tft.print(" ", paddingLeft + tft.getFontXsize() * 11, paddingTop);

		tft.printNumI(hour, paddingLeft + tft.getFontXsize() * 13, paddingTop, 2, '0');
		tft.print(":", paddingLeft + tft.getFontXsize() * 15, paddingTop);
		tft.printNumI(minute, paddingLeft + tft.getFontXsize() * 16, paddingTop, 2, '0');
		tft.print(":", paddingLeft + tft.getFontXsize() * 18, paddingTop);
		tft.printNumI(second, paddingLeft + tft.getFontXsize() * 19, paddingTop, 2, '0');
	}
}

void drawOxigenPercentage(int oxigenPercentage)
{
	//TODO
}

void drawStaticDecoArea(bool decoNeeded)
{
	//Clear deco area

	//Draw the static text
	if (!decoNeeded) {
		//tft.print("Safe");
	} else {
		//tft.print("Deco");
	}
}

void drawDecoArea(DiveInfo diveInfo)
{
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

void drawSafetyStop(unsigned int safetyStopDurationInSeconds)
{
	if (currentScreen == DIVE_SCREEN) {

		tft.setFont(Grotesk16x32);
		tft.setColor(VGA_LIME);
		tft.print("Stay", 10, 230);

		tft.setFont(SevenSegNumFontPlusPlus);

		unsigned int duration = safetyStopDurationInSeconds % 3600;
		byte minutes = duration / 60;
		byte seconds = duration % 60;

		int paddingLeft = 80;
		int paddingTop = 230;

		tft.printNumI(seconds, paddingLeft + tft.getFontXsize() * 5, paddingTop, 2, '0');
		tft.print(":", paddingLeft + tft.getFontXsize() * 4, paddingTop, 1);
		tft.printNumI(minutes, paddingLeft + tft.getFontXsize() * 2, paddingTop, 2, '0');
	}
}

void drawAscend(int ascendRate)
{
	if (currentScreen == DIVE_SCREEN) {

		int paddingLeft = 350;
		int paddingTop = 200;

		tft.setFont(Grotesk16x32);
		switch (ascendRate) {
			case ASCEND_OK:
				tft.setColor(VGA_GREEN);
				tft.print("----", paddingLeft, paddingTop);
				break;
			case ASCEND_SLOW:
				tft.setColor(VGA_WHITE);
				tft.print("---|", paddingLeft, paddingTop);
				break;
			case ASCEND_NORMAL:
				tft.setColor(VGA_WHITE);
				tft.print("--||", paddingLeft, paddingTop);
				break;
			case ASCEND_ATTENTION:
				tft.setColor(VGA_YELLOW);
				tft.print("-|||", paddingLeft, paddingTop);
				break;
			case ASCEND_CRITICAL:
				tft.setColor(VGA_YELLOW);
				tft.print("||||", paddingLeft, paddingTop);
				break;
			case ASCEND_DANGER:
				tft.setColor(VGA_RED);
				tft.print("SLOW", paddingLeft, paddingTop);
				break;
			default:
				tft.setColor(VGA_GREEN);
				tft.print("<<--", paddingLeft, paddingTop);
		}
	}
}
