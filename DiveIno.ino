#include "MS5803_I2C.h"
#include "SPI.h"
#include "Wire.h"
#include "UTFT.h"
#include "DS3231.h"
#include "IRremote.h"
#include "SD.h"
#include "SimpleTimer.h"
#include "MAX17043.h"

#include "DiveDeco.h"
#include "View.h"
#include "Settings.h"
#include "Logbook.h"

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

// Pin number of the piezo buzzer
int speakerPin = 8;

// TFT setup - 480x320 pixel
//UTFT tft(HX8357B,38,39,40,41);    //Banggood
UTFT tft(ILI9481,38,39,40,41);      //Deal Extreme
View view(&tft);

// Currently elected menu item
byte selectedMenuItemIndex;

Settings settings = Settings();
float seaLevelPressureSetting = 1013.25;
float oxygenRateSetting = 0.21;
bool testModeSetting = true;
bool soundSetting = true;
bool imperialUnitsSetting = false;

byte selectedSettingIndex = 0;
bool isSdCardPresent = false;

#define SURFACE_MODE 		0  // Menu, Logbook, Surface Time, Settings and About are available
#define DIVE_START_MODE 	1  // When the driver pressed the dive button but not below 2 meters
#define DIVE_PROGRESS_MODE 	2  // During the dive
#define DIVE_STOP_MODE 		3  // Once the diver ascended to the surface - for X minutes - the diver can go under water
#define GAUGE_MODE          4

byte currentMode = SURFACE_MODE;
byte currentScreen = MENU_SCREEN;

DiveDeco diveDeco = DiveDeco(400, 56.7);

//Utility variables
DiveResult* previousDiveResult;

float maxDepthInMeter;
float maxTemperatureInCelsius;
float minTemperatureInCelsius;

#define SAFETY_STOP_NOT_STARTED 0
#define SAFETY_STOP_IN_PROGRESS 1
#define SAFETY_STOP_DONE 		2
#define SAFETY_STOP_VIOLATED	3

byte safetyStopState = SAFETY_STOP_NOT_STARTED;

unsigned long diveDurationInSeconds;
unsigned long safetyStopDurationInSeconds;
unsigned long testPreviousDiveDurationInSeconds;

SimpleTimer diveDurationTimer;

MAX17043 batteryMonitor;

Logbook logbook = Logbook();
int currentProfileNumber = 0; //There is no stored profile - default state
int maximumProfileNumber = 0;
File profileFile;

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

	Serial.begin(115200);
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

    batteryMonitor.reset();
    batteryMonitor.quickStart();

	sensor.reset();
	sensor.begin();

	tft.InitLCD();

    // Set the function, which will be called in every second, if the timer is enabled
	diveDurationTimer.setTimer(1000, diveUnderWater, diveDurationTimer.RUN_FOREVER);
	diveDurationTimer.disable(diveDurationTimer.RUN_FOREVER);

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

	///////////////
	// Go Diving //
	///////////////

	if (currentScreen == GAUGE_SCREEN || currentScreen == DIVE_SCREEN) {
		if (testModeSetting) {
			diveSurface();
		} else {
			diveDurationTimer.run();
		}
	}
}

//////////
// Dive //
//////////

void diveSurface()
{
	// Check if test data is available, which comes through the serial interface
	if (Serial.available() > 0) {
		float pressureInMillibar = Serial.parseFloat();
		float depthInMeter = Serial.parseFloat();
		diveDurationInSeconds = Serial.parseInt();
		float temperatureInCelsius = Serial.parseFloat();

		if (testModeSetting) {
			Serial.print("Pressure: ");
			Serial.print(pressureInMillibar, 2);
			Serial.print(" Depth: ");
			Serial.print(depthInMeter, 2);
			Serial.print(" Temperature: ");
			Serial.print(temperatureInCelsius, 2);
			Serial.print(" Duration: ");
			Serial.println(diveDurationInSeconds);
		}

		view.drawCurrentPressure(pressureInMillibar);
		view.drawDepth(depthInMeter);
		view.drawCurrentTemperature(temperatureInCelsius);
		view.drawDiveDuration(diveDurationInSeconds);

		//Calculate minimum and maximum values during the dive
		if (maxDepthInMeter < depthInMeter) {
			maxDepthInMeter = depthInMeter;
			view.drawMaximumDepth(maxDepthInMeter);
		}
		if (maxTemperatureInCelsius < temperatureInCelsius) {
			maxTemperatureInCelsius = temperatureInCelsius;
		}
		if (minTemperatureInCelsius > temperatureInCelsius) {
			minTemperatureInCelsius = temperatureInCelsius;
		}

		switch (currentScreen) {
			case GAUGE_SCREEN: {
				//Instead of dive information we will display the current time
				view.drawCurrentTime(getCurrentTimeText());
			}
			break;
			case DIVE_SCREEN: {

				//Calculate the delta between the current and the previous duration information
				if (diveDurationInSeconds > testPreviousDiveDurationInSeconds) {
					unsigned long intervalDuration = diveDurationInSeconds - testPreviousDiveDurationInSeconds;

					testPreviousDiveDurationInSeconds = diveDurationInSeconds;

					//The timer fires in every second
					calculateSafetyStop(maxDepthInMeter, depthInMeter, intervalDuration);

					//Progress with the algorithm in every second
					diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
				}
			}
			break;
		}
	}
}

void diveUnderWater() // Called in every second on the GAUGE and DIVE screens
{
	diveDurationInSeconds++;

	float pressureInMillibar = sensor.getPressure(ADC_4096);
	float temperatureInCelsius = sensor.getTemperature(CELSIUS, ADC_512);

	float depthInMeter = 0;
	if (pressureInMillibar > seaLevelPressureSetting) {
		depthInMeter = diveDeco.calculateDepthFromPressure(pressureInMillibar);
	}

	//Draw the data to the screen
	view.drawCurrentTemperature(temperatureInCelsius);
	view.drawCurrentPressure(pressureInMillibar);
	view.drawDepth(depthInMeter);
	view.drawDiveDuration(diveDurationInSeconds);

	//Calculate minimum and maximum values during the dive
	if (maxDepthInMeter < depthInMeter) {
		maxDepthInMeter = depthInMeter;
		view.drawMaximumDepth(maxDepthInMeter);
	}
	if (maxTemperatureInCelsius < temperatureInCelsius) {
		maxTemperatureInCelsius = temperatureInCelsius;
	}
	if (minTemperatureInCelsius > temperatureInCelsius) {
		minTemperatureInCelsius = temperatureInCelsius;
	}

	switch (currentScreen) {
		case GAUGE_SCREEN: {
			//Instead of dive information we will display the current time
			view.drawCurrentTime(getCurrentTimeText());
		}
		break;
		case DIVE_SCREEN: {

			//The timer fires in every second
			calculateSafetyStop(maxDepthInMeter, depthInMeter, 1);

			//Progress with the algorithm in every second
			diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
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
				safetyStopState = SAFETY_STOP_IN_PROGRESS;

				if (testModeSetting) {
					Serial.print(" Safety stop STARTED at ");
					Serial.println(depthInMeter);
				}
			} else if (safetyStopState == SAFETY_STOP_VIOLATED) {
				safetyStopState = SAFETY_STOP_IN_PROGRESS;
			}
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				safetyStopDurationInSeconds += intervalDuration;
				view.drawSafetyStop(safetyStopDurationInSeconds);

				if (testModeSetting) {
					Serial.print(" Safety stop IN PROGRESS at ");
					Serial.println(depthInMeter);
					Serial.print(" Safety stop duration: ");
					Serial.println(safetyStopDurationInSeconds);
				}

				if (180 <= safetyStopDurationInSeconds) {
					safetyStopState = SAFETY_STOP_DONE;

					if (testModeSetting) {
						Serial.print(" Safety stop DONE at ");
						Serial.println(depthInMeter);
					}
				}
			}
		} else if (10 < depthInMeter) {
			//Reset the safety stop mode, if the diver descends back down to 10 m
			if (safetyStopState != SAFETY_STOP_NOT_STARTED) {
				safetyStopState = SAFETY_STOP_NOT_STARTED;

				if (testModeSetting) {
					Serial.print(" Safety stop RESET after DONE at ");
					Serial.println(depthInMeter);
				}
			}
		} else {
			//Between 10-6 meters and above 3 meters the safety stop is violated - the counter will stop
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				safetyStopState = SAFETY_STOP_VIOLATED;

				if (testModeSetting) {
					Serial.print(" Safety stop VIOLATED at ");
					Serial.println(depthInMeter);
				}
			}
		}
	}
}

void startDive()
{
	diveDurationTimer.enable(diveDurationTimer.RUN_FOREVER);

	//Set default values before the dive
	maxDepthInMeter = 0;
	minTemperatureInCelsius = 60;
	maxTemperatureInCelsius = 0;

	safetyStopState = SAFETY_STOP_NOT_STARTED;
	safetyStopDurationInSeconds = 0;
	diveDurationInSeconds = 0;
	testPreviousDiveDurationInSeconds = 0;

	if (currentScreen == DIVE_SCREEN) {

		//Start dive profile logging - create a new file
		if (maximumProfileNumber <= 0) {
			maximumProfileNumber = logbook.loadLogbookData()->numberOfStoredProfiles;
		}
		profileFile = logbook.createNewProfileFile(maximumProfileNumber+1);

		// Initialize the DiveDeco library based on the settings
		diveDeco.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
		diveDeco.setNitrogenRateInGas(1 - oxygenRateSetting);

		if (previousDiveResult == NULL) {
			//No previous dive result is available, so do the default compartment initialization
			diveDeco.startDive(diveDeco.initializeCompartments());
		} else {
			diveDeco.startDive(previousDiveResult);
		}
	}
}

void stopDive()
{
	diveDurationTimer.disable(diveDurationTimer.RUN_FOREVER);
}

void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) {

	DiveData diveData = {pressureInMillibar, durationInSeconds};
	DiveInfo diveInfo = diveDeco.progressDive(&diveData);
	view.drawAscend(diveInfo.ascendRate);

	if (safetyStopState != SAFETY_STOP_IN_PROGRESS) {
		view.drawDecoArea(diveInfo);
	}

	//Store the current dive data in the dive profile
	logbook.storeProfileItem(profileFile, pressureInMillibar, depthInMeter, temperatureInCelsius, durationInSeconds);

	if (seaLevelPressureSetting >= pressureInMillibar) {
		if (testModeSetting) {
			Serial.println("STOP");
		}

		previousDiveResult = diveDeco.stopDive();

		String currentTimeText = getCurrentTimeText();

		// Stop dive profile logging
		maximumProfileNumber = maximumProfileNumber+1;
		logbook.storeDiveSummary(maximumProfileNumber, profileFile,
				previousDiveResult->durationInSeconds, previousDiveResult->maxDepthInMeters, minTemperatureInCelsius,
				oxygenRateSetting*100, currentTimeText.substring(0, 10), currentTimeText.substring(11, 16));

		int durationInMinutes = previousDiveResult->durationInSeconds / 60;

		LogbookData* logbookData = logbook.loadLogbookData();
		logbookData->totalNumberOfDives++;
		logbookData->numberOfStoredProfiles++;
		logbookData->lastDiveDateTime = currentTimeText.substring(0, 16);

		//Add the dive duration to the overall logged duration
		Serial.print("Dive duration (min): ");
		Serial.println(durationInMinutes);
		Serial.print("Previous duration (min): ");
		Serial.println(logbookData->totalDiveMinutes);

		durationInMinutes = logbookData->totalDiveMinutes + durationInMinutes;
		Serial.print("New duration (min): ");
		Serial.println(durationInMinutes);

		logbookData->totalDiveHours += durationInMinutes / 60;
		logbookData->totalDiveMinutes = durationInMinutes % 60;

		if (previousDiveResult->maxDepthInMeters > logbookData->totalMaximumDepth) {
			logbookData->totalMaximumDepth = previousDiveResult->maxDepthInMeters;
		}
		logbook.updateLogbookData(logbookData);

		//Switch to DIVE_STOP mode
		currentMode = DIVE_STOP_MODE;
		displayScreen(SURFACE_TIME_SCREEN);

		//TODO Start surface time counter
	}
}

String getCurrentTimeText() {
	String time = "20";
	//Instead of dive information we will display the current time
	byte year, month, date, DoW, hour, minute, second;
	Clock.getTime(year, month, date, DoW, hour, minute, second);

	time+=year;
	if (year<10) {
		time+="0";
	}
	time+="-";
	if (month <10) {
		time+="0";
	}
	time+=month;
	time+="-";
	if (date<10) {
		time+="0";
	}
	time+=date;
	time+=" ";

	if (hour<10) {
		time+="0";
	}
	time+=hour;
	time+=":";
	if (minute<10) {
		time+="0";
	}
	time+=minute;
	time+=":";
	if (second<10) {
		time+="0";
	}
	time+=second;

	return time;
}

/////////////
// Buttons //
/////////////

void processRemoteButtonPress(decode_results *results) {
	if (results->value == 0xFD8877 || results->value == 0xFF629D) {
		if (testModeSetting) {
			Serial.println("Up || Mode");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		upButtonPressed();
	} else if (results->value == 0xFD9867 || results->value == 0xFFA857) {
		if (testModeSetting) {
			Serial.println("Down || -");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		downButtonPressed();
	} else if (results->value == 0xFD28D7 || results->value == 0xFF22DD) {
		if (testModeSetting) {
			Serial.println("Left || Play");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		leftButtonPressed();
	} else if (results->value == 0xFD6897 || results->value == 0xFFC23D) {
		if (testModeSetting) {
			Serial.println("Right || Forward");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		rightButtonPressed();
	} else if (results->value == 0xFDA857 || results->value == 0xFF02FD) {
		if (testModeSetting) {
			Serial.println("OK || Previous");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
		selectButtonPressed();
	} else if (results->value == 0xFD30CF || results->value == 0xFFE01F) {
		if (testModeSetting) {
			Serial.println("* || EQ");
		}
		if (soundSetting) {
			tone(speakerPin, 261, 10);
		}
	} else if (results->value == 0xFD708F || results->value == 0xFF906F) {
		if (testModeSetting) {
			Serial.println("# || +");
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

	if (currentScreen == PROFILE_SCREEN) {
		logbook.drawProfileItems(&tft, currentProfileNumber, number);
	}
}

void hashButtonPressed()
{
	if (currentScreen == SETTINGS_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	}
}

// This is the OK or the Prev button on the IR Remote Control
void selectButtonPressed()
{
	if (currentScreen == MENU_SCREEN) {
		switch (selectedMenuItemIndex) {
			case 1: { // Dive
				currentMode = DIVE_START_MODE;
				displayScreen(DIVE_SCREEN);
				startDive();
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
				startDive();
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
	} else if (currentScreen == GAUGE_SCREEN) {
		currentMode = SURFACE_MODE;
		stopDive();
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
		case LOGBOOK_SCREEN: {
			currentMode = SURFACE_MODE;
			displayScreen(MENU_SCREEN);
		}
		break;
		case PROFILE_SCREEN: {
			displayScreen(LOGBOOK_SCREEN);
		}
		break;
		case SURFACE_TIME_SCREEN: {
			currentMode = SURFACE_MODE;
			displayScreen(MENU_SCREEN);
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
		case ABOUT_SCREEN: {
			currentMode = SURFACE_MODE;
			displayScreen(MENU_SCREEN);
		}
		break;
		case UI_TEST_SCREEN: {
			currentMode = SURFACE_MODE;
			displayScreen(ABOUT_SCREEN);
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
		case LOGBOOK_SCREEN: {
			displayScreen(PROFILE_SCREEN);
		}
		break;
		case ABOUT_SCREEN: {
			currentMode = SURFACE_MODE;
			displayScreen(UI_TEST_SCREEN);
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
		case PROFILE_SCREEN: {
			if (currentProfileNumber != 0) {
				if (currentProfileNumber > 1) {
					currentProfileNumber--;
				} else {
					currentProfileNumber = maximumProfileNumber;
				}
				displayScreen(PROFILE_SCREEN);
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
		case PROFILE_SCREEN: {
			if (currentProfileNumber != 0 && maximumProfileNumber > 1) {
				if (currentProfileNumber == maximumProfileNumber) {
					currentProfileNumber = 1;
				} else {
					currentProfileNumber++;
				}
				displayScreen(PROFILE_SCREEN);
			}
		}
	}
}

//////////////
// Settings //
//////////////

void settingsSelect(byte settingIndex)
{
	view.displaySettings(settingIndex, seaLevelPressureSetting, oxygenRateSetting, testModeSetting, soundSetting, imperialUnitsSetting);
	selectedSettingIndex = settingIndex;
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

//////////
// Menu //
//////////

void menuSelect(byte menuItemIndex)
{
	view.moveMenuSelection(selectedMenuItemIndex, menuItemIndex);

	// Save the selection
	selectedMenuItemIndex = menuItemIndex;
}

void displayScreen(byte screen) {
	LogbookData* logbookData;
	ProfileData* profileData;

	currentScreen = screen;
	switch (screen) {
		case MENU_SCREEN:
			// Draw the menu
			view.displayMenuScreen();
			// Select 1st menu item
			selectedMenuItemIndex = 1;
			menuSelect(selectedMenuItemIndex);
			break;
		case DIVE_SCREEN:
			view.displayDiveScreen(oxygenRateSetting);
			break;
		case LOGBOOK_SCREEN:
			logbookData = logbook.loadLogbookData();
			maximumProfileNumber = logbookData->numberOfStoredProfiles;
			if (maximumProfileNumber > 0 && currentProfileNumber == 0) {
				//In the default case move to the first profile screen
				currentProfileNumber = 1;
			}
			view.displayLogbookScreen(logbookData);
			break;
		case PROFILE_SCREEN:
			if (maximumProfileNumber != 0 && currentProfileNumber != 0) {
				view.displayProfileScreen(logbook.loadProfileDataFromFile(logbook.getFileNameFromProfileNumber(currentProfileNumber, false)), currentProfileNumber);
			}
			break;
		case SURFACE_TIME_SCREEN:
			if (previousDiveResult == NULL) {
				previousDiveResult = new DiveResult;
				previousDiveResult->compartmentPartialPressures[0] = 1234;
				previousDiveResult->compartmentPartialPressures[1] = 2343;
				previousDiveResult->compartmentPartialPressures[2] = 3847;
				previousDiveResult->compartmentPartialPressures[3] = 2345;
				previousDiveResult->compartmentPartialPressures[4] = 1231;
				previousDiveResult->compartmentPartialPressures[5] = 1123;
				previousDiveResult->compartmentPartialPressures[6] = 1452;
				previousDiveResult->compartmentPartialPressures[7] = 1556;
				previousDiveResult->compartmentPartialPressures[8] = 2345;
				previousDiveResult->compartmentPartialPressures[9] = 3453;
				previousDiveResult->compartmentPartialPressures[10] = 1234;
				previousDiveResult->compartmentPartialPressures[11] = 2678;
				previousDiveResult->compartmentPartialPressures[12] = 3425;
				previousDiveResult->compartmentPartialPressures[13] = 2567;
				previousDiveResult->compartmentPartialPressures[14] = 1745;
				previousDiveResult->compartmentPartialPressures[15] = 1834;
				previousDiveResult->maxDepthInMeters = 36.9;
				previousDiveResult->durationInSeconds = 3445;
				previousDiveResult->noFlyTimeInMinutes = 1603;
			}
			view.displaySurfaceTimeScreen(previousDiveResult);
			break;
		case GAUGE_SCREEN:
			view.displayGaugeScreen(testModeSetting);
			break;
		case SETTINGS_SCREEN:
			view.displaySettingsScreen(0, seaLevelPressureSetting, oxygenRateSetting, testModeSetting, soundSetting, imperialUnitsSetting);
			break;
		case ABOUT_SCREEN:
			view.displayAboutScreen();
			view.drawCurrentTime(getCurrentTimeText());

			Serial.print("Voltage:\t\t");
			Serial.print(batteryMonitor.getVCell(), 4);
			Serial.println("V");

			Serial.print("State of charge:\t");
			Serial.print(batteryMonitor.getSoC());
			Serial.println("%");
			break;
		case UI_TEST_SCREEN:
			view.displayTestScreen();

//			if (maximumProfileNumber <= 0) {
//				maximumProfileNumber = logbook.loadLogbookData()->numberOfStoredProfiles;
//			}
//
//			File profileFile = logbook.createNewProfileFile(17);
//
//			float pressure = 1435.46;
//			float depth = 4.2;
//			float temperature = 15.3;
//			int duration = 20;
//
//			//Append the new profile information
//			logbook.storeProfileItem(profileFile, pressure, depth, temperature, duration);
//
//			pressure = 1525.89;
//			depth = 5.1;
//			temperature = 15.3;
//			duration = 40;
//
//			//Append the new profile information
//			logbook.storeProfileItem(profileFile, pressure, depth, temperature, duration);
//
//			view.displayTestScreen();
//
//			logbook.storeDiveSummary(17, profileFile, 45, 36.9, 16.7, 21.4, "2016-06-19", "10:36");
//
//			LogbookData* logbookData = new LogbookData;
//			logbookData->totalNumberOfDives = 56;
//			logbookData->totalDiveHours = 12;
//			logbookData->totalDiveMinutes = 45;
//			logbookData->totalMaximumDepth = 42.3;
//			logbookData->lastDiveDateTime = "2016-03-31 15:05";
//			logbookData->numberOfStoredProfiles = 12;
//
//			logbook.updateLogbookData(logbookData);
//			logbook.printFile("LOGBOOK.TXT");

			break;
	}
}
