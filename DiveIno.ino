#include "MS5803_14.h"
#include "SPI.h"
#include "Wire.h"
#include "UTFT.h"
#include "SD.h"
#include "SimpleTimer.h"
#include "MAX17043.h"
#include "IRremote2.h"
#include "DS1307RTC.h"

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	uint8_t csPin = 53;
#elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
	#include "TimerFreeTone.h"
	uint8_t csPin = 4;
#endif

#include "Buhlmann.h"
#include "View.h"
#include "Settings.h"
#include "Logbook.h"
#include "LastDive.h"

//Infrared Receiver initialization
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

//Pressure Sensor initialization
MS_5803 sensor = MS_5803(512);

// Pin number of the piezo buzzer
#define TONE_PIN 8

// TFT setup - 480x320 pixel
UTFT tft(ILI9481,38,39,40,41);
View view(&tft);

// Currently elected menu item
byte selectedMenuItemIndex;

Settings settings = Settings();
float seaLevelPressureSetting = 1013.25;
float oxygenRateSetting = 0.21;
bool testModeSetting = false;
bool soundSetting = true;
bool imperialUnitsSetting = false;

DateTimeSettings* currentDateTimeSettings;

byte selectedSettingIndex = 0;
byte selectedDateTimeSettingIndex = 0;
bool isSdCardPresent = false;

#define SURFACE_MODE 		0  // Menu, Logbook, Surface Time, Settings and About are available
#define DIVE_START_MODE 	1  // When the driver pressed the dive button but not below 2 meters
#define DIVE_PROGRESS_MODE 	2  // During the dive
#define DIVE_STOP_MODE 		3  // Once the diver ascended to the surface - for X minutes - the diver can go under water
#define GAUGE_MODE          4

byte currentMode = SURFACE_MODE;
byte currentScreen = MENU_SCREEN;

Buhlmann buhlmann = Buhlmann(400, 56.7);

//Utility variables
float maxDepthInMeter;
float maxTemperatureInCelsius;
float minTemperatureInCelsius;

#define SAFETY_STOP_NOT_STARTED 0
#define SAFETY_STOP_IN_PROGRESS 1
#define SAFETY_STOP_DONE 		2
#define SAFETY_STOP_VIOLATED	3

byte safetyStopState = SAFETY_STOP_NOT_STARTED;

unsigned long diveStartTimestamp;
unsigned long timerTimestamp;

unsigned long diveDurationInSeconds;
unsigned long safetyStopDurationInSeconds;
unsigned long testPreviousDiveDurationInSeconds;

float previousPressureInMillibar;

MAX17043 batteryMonitor;
float batterySoc = 255;
float batteryVoltage = 5;

Logbook logbook = Logbook();
int currentProfileNumber = 0; //There is no stored profile - default state
int maximumProfileNumber = 0;
File profileFile;

LastDive lastDive = LastDive();

#define EMULATOR_ENABLED 1 // Valid values: 0 = disabled, 1 = enabled
#define TEST_ENABLED 0 //TODO

void setup() {

	// SD Card initialization
	pinMode(csPin, OUTPUT);
	if (SD.begin(csPin)) {
		isSdCardPresent = true;

		DiveInoSettings* diveInoSettings = settings.loadDiveInoSettings();
		seaLevelPressureSetting = diveInoSettings->seaLevelPressureSetting;
		oxygenRateSetting = diveInoSettings->oxygenRateSetting;
		testModeSetting = diveInoSettings->testModeSetting;
		soundSetting = diveInoSettings->soundSetting;
		imperialUnitsSetting = diveInoSettings->imperialUnitsSetting;
	}

	Serial.begin(115200);
	delay(1000);

	Serial.println("");
	Serial.println(F("DiveIno - START"));
	Serial.println("");

	Serial.print(F("SD Card present: "));
	if (isSdCardPresent) {
		Serial.println(F("YES"));
	} else {
		Serial.println(F("NO"));
	}
	Serial.println("");

	Serial.println(F("Settings: "));
	Serial.print(F("seaLevelPressure: "));
	Serial.println(seaLevelPressureSetting, 2);
	Serial.print(F("oxygenRate: "));
	Serial.println(oxygenRateSetting, 2);
	Serial.print(F("testMode: "));
	if (testModeSetting) {
		Serial.println(F("On"));
	} else {
		Serial.println(F("Off"));
	}
	Serial.print(F("sound: "));
	if (soundSetting) {
		Serial.println(F("On"));
	} else {
		Serial.println(F("Off"));
	}
	Serial.print(F("units: "));
	if (imperialUnitsSetting) {
		Serial.println(F("Imperial"));
	} else {
		Serial.println(F("Metric"));
	}
	Serial.println("");

	Wire.begin();

	irrecv.enableIRIn();

    batteryMonitor.reset();
    batteryMonitor.quickStart();

    if (sensor.initializeMS_5803()) {
      Serial.println(F("MS5803 CRC check OK."));
    }
    else {
      Serial.println(F("MS5803 CRC check FAILED!"));
    }

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	setSyncProvider((unsigned long int (*)())RTC.get);
    if(timeStatus() != timeSet){
        Serial.println(F("RTC - time was not set!"));
    } else {
        Serial.println(F("RTC - time was set"));
    }
#endif

    Serial.print(F("RTC time: "));
    Serial.println(settings.getCurrentTimeText());

    Serial.print(F("Now timestamp: "));
    Serial.println(nowTimestamp());

	tft.InitLCD();

	batterySoc = batteryMonitor.getSoC();
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

	/////////////
	// Battery //
	/////////////

	batterySoc = batteryMonitor.getSoC();

	///////////////
	// Go Diving //
	///////////////

	if (currentScreen == GAUGE_SCREEN || currentScreen == DIVE_SCREEN) {
		if (testModeSetting) {
			diveSurface();
		} else {
			diveUnderWater();
		}
	}
}

void beep()
{
	if (soundSetting) {
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		tone(TONE_PIN, 261, 50);
#elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
		TimerFreeTone(TONE_PIN, 261, 50);
#endif
	}
}

time_t nowTimestamp()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	return now();
#elif defined(__SAM3X8E__) || defined(__SAM3X8H__)
	tmElements_t tm;
	if (RTC.read(tm)) {
	    return makeTime(tm);
	}
#endif
	return 0;
}

//////////
// Dive //
//////////

void calculateSafetyStop(float maxDepthInMeter, float depthInMeter, unsigned int intervalDuration)
{
	//Safety stop calculation - only if the dive was deeper than 10 m
	if (maxDepthInMeter > 10) {

		//Between 3 and 6 meters the diver can do the safety stop
		if (3 < depthInMeter && depthInMeter < 6) {

			if (safetyStopState == SAFETY_STOP_NOT_STARTED) {
				safetyStopState = SAFETY_STOP_IN_PROGRESS;

				Serial.print(" Safety stop STARTED at ");
				Serial.println(depthInMeter);

			} else if (safetyStopState == SAFETY_STOP_VIOLATED) {
				safetyStopState = SAFETY_STOP_IN_PROGRESS;
			}
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				safetyStopDurationInSeconds += intervalDuration;
				view.drawSafetyStop(safetyStopDurationInSeconds);

				Serial.print(" Safety stop IN PROGRESS at ");
				Serial.println(depthInMeter);
				Serial.print(" Safety stop duration: ");
				Serial.println(safetyStopDurationInSeconds);

				if (180 <= safetyStopDurationInSeconds) {
					safetyStopState = SAFETY_STOP_DONE;

					Serial.print(" Safety stop DONE at ");
					Serial.println(depthInMeter);
				}
			}
		} else if (10 < depthInMeter) {
			//Reset the safety stop mode, if the diver descends back down to 10 m
			if (safetyStopState != SAFETY_STOP_NOT_STARTED) {
				safetyStopState = SAFETY_STOP_NOT_STARTED;

				Serial.print(" Safety stop RESET after DONE at ");
				Serial.println(depthInMeter);
			}
		} else {
			//Between 10-6 meters and above 3 meters the safety stop is violated - the counter will stop
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				safetyStopState = SAFETY_STOP_VIOLATED;

				Serial.print(" Safety stop VIOLATED at ");
				Serial.println(depthInMeter);
			}
		}
	}
}

void calculateMinMaxValues(float depthInMeter, float temperatureInCelsius)
{
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
}

void diveSurface()
{
	// Check if test data is available, which comes through the serial interface
	if (Serial.available() > 0) {
		float pressureInMillibar = Serial.parseFloat();
		float depthInMeter = Serial.parseFloat();
		diveDurationInSeconds = Serial.parseInt();
		float temperatureInCelsius = Serial.parseFloat();

		view.drawCurrentPressure(pressureInMillibar);
		view.drawDepth(depthInMeter);
		view.drawCurrentTemperature(temperatureInCelsius);
		view.drawDiveDuration(diveDurationInSeconds);

		view.drawBatteryStateOfCharge(batterySoc);

		calculateMinMaxValues(depthInMeter, temperatureInCelsius);

		switch (currentScreen) {
			case GAUGE_SCREEN: {
				//Instead of dive information we will display the current time
				view.drawCurrentTime(settings.getCurrentTimeText());
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

void diveUnderWater()
{
	unsigned int measurementDifference = nowTimestamp() - timerTimestamp;

	if (measurementDifference > 0) {
		timerTimestamp = nowTimestamp();

		float pressureInMillibar = seaLevelPressureSetting;
		float temperatureInCelsius = 99;
		if (EMULATOR_ENABLED) {
			Serial.println("#GET");
			if (Serial.available() > 0) {
				pressureInMillibar = Serial.parseFloat();
				temperatureInCelsius = Serial.parseFloat();

				Serial.print("Pressure: ");
				Serial.println(pressureInMillibar);
				Serial.print("Temperature: ");
				Serial.println(temperatureInCelsius);
			}
		} else {
			sensor.readSensor();
			pressureInMillibar = sensor.pressure();
			temperatureInCelsius = sensor.temperature();
		}

		//Check for sensor error - difference has to be less than 20 meters
		if (abs(pressureInMillibar-previousPressureInMillibar) > 2000) {
			//This is a sensor error - skip it!!!
			return;
		}

		previousPressureInMillibar = pressureInMillibar;

		float depthInMeter = buhlmann.calculateDepthFromPressure(pressureInMillibar);

		//Draw the data to the screen
		view.drawCurrentTemperature(temperatureInCelsius);
		view.drawCurrentPressure(pressureInMillibar);
		view.drawDepth(depthInMeter);

		calculateMinMaxValues(depthInMeter, temperatureInCelsius);

		view.drawBatteryStateOfCharge(batterySoc);

		switch (currentScreen) {
			case GAUGE_SCREEN: {
				view.drawDiveDuration(nowTimestamp()-diveStartTimestamp);

				//Instead of dive information we will display the current time
				view.drawCurrentTime(settings.getCurrentTimeText());
			}
			break;
			case DIVE_SCREEN: {
				if (pressureInMillibar > (seaLevelPressureSetting + 120) || diveDurationInSeconds > 60) {
					currentMode = DIVE_PROGRESS_MODE;
				}

				if (currentMode == DIVE_PROGRESS_MODE) {
					//Progress with the dive, if we are deeper than 1.2 meter
					unsigned int intervalDuration = nowTimestamp()-diveStartTimestamp-diveDurationInSeconds;
					calculateSafetyStop(maxDepthInMeter, depthInMeter, intervalDuration);

					diveDurationInSeconds = diveDurationInSeconds + intervalDuration;
					view.drawDiveDuration(diveDurationInSeconds);

					//Progress with the algorithm in every 5 seconds
					if (diveDurationInSeconds % 5 == 0) {
						diveProgress(temperatureInCelsius, pressureInMillibar, depthInMeter, diveDurationInSeconds);
					}
				} else {
					//No dive progress - reset the dive timestamp
					diveStartTimestamp = nowTimestamp();
				}
			}
			break;
		}
	}
}

void startDive()
{
	if (EMULATOR_ENABLED) {
		Serial.println("#START");
	}

	//Store the the current time as seconds since Jan 1 1970 at the start of the dive
	diveStartTimestamp = nowTimestamp();
	timerTimestamp = nowTimestamp();

	previousPressureInMillibar = seaLevelPressureSetting;

	//Set default values before the dive
	maxDepthInMeter = 0;
	minTemperatureInCelsius = 60;
	maxTemperatureInCelsius = 0;

	safetyStopState = SAFETY_STOP_NOT_STARTED;
	safetyStopDurationInSeconds = 0;
	diveDurationInSeconds = 0;
	testPreviousDiveDurationInSeconds = 0;

	// Initialize the DiveDeco library based on the settings
	buhlmann.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
	buhlmann.setNitrogenRateInGas(1 - oxygenRateSetting);

	if (currentScreen == DIVE_SCREEN) {

		Serial.println("DIVE - Started");

		//Start dive profile logging - create a new file
		if (maximumProfileNumber <= 0) {
			maximumProfileNumber = logbook.loadLogbookData()->numberOfStoredProfiles;
		}
		profileFile = logbook.createNewProfileFile(maximumProfileNumber+1);

		DiveResult* diveResult = new DiveResult;
		diveResult = buhlmann.initializeCompartments();

		//Retrieve last dive data
		LastDiveData* lastDiveData = lastDive.loadLastDiveData();

		//Last dive happened within 48 hours and there is an active no fly time
		if (lastDiveData != NULL &&	(lastDiveData->diveDateTimestamp + 172800) > nowTimestamp() && lastDiveData->noFlyTimeInMinutes > 0) {

			Serial.print("BEFORE dive No Fly time (min): ");
			Serial.println(lastDiveData->noFlyTimeInMinutes);

			//Copy Last Dive Data compartments
			diveResult->noFlyTimeInMinutes = lastDiveData->noFlyTimeInMinutes;
			for (byte i=0; i <COMPARTMENT_COUNT; i++) {
				diveResult->compartmentPartialPressures[i] = lastDiveData->compartmentPartialPressures[i];

		        Serial.print("BEFORE: Dive compartment ");
		        Serial.print(i);
		        Serial.print(": ");
		        Serial.print(diveResult->compartmentPartialPressures[i], 0);
		        Serial.println(" ppN2");
			}

			//Calculate surface time in minutes
			int surfaceTime = (nowTimestamp() - lastDiveData->diveDateTimestamp) / 60;

			Serial.print("Surface time (min): ");
			Serial.println(surfaceTime);

			//Spend the time on the surface
			diveResult = buhlmann.surfaceInterval(surfaceTime, diveResult);

			for (byte i=0; i <COMPARTMENT_COUNT; i++) {
		        Serial.print("AFTER: Dive compartment ");
		        Serial.print(i);
		        Serial.print(": ");
		        Serial.print(diveResult->compartmentPartialPressures[i], 0);
		        Serial.println(" ppN2");
			}

			Serial.print("AFTER dive No Fly time (min): ");
			Serial.println(diveResult->noFlyTimeInMinutes);
		}

		buhlmann.startDive(diveResult);
	}
}

void stopDive()
{
	if (EMULATOR_ENABLED) {
		Serial.println("#STOP");
	}
	Serial.println("DIVE - Stopped");
}

void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) {

	DiveData diveData = {pressureInMillibar, durationInSeconds};
	DiveInfo diveInfo = buhlmann.progressDive(&diveData);
	view.drawAscend(diveInfo.ascendRate);

	view.drawDecoArea(diveInfo);

	//Store the current dive data in the dive profile
	logbook.storeProfileItem(profileFile, pressureInMillibar, depthInMeter, temperatureInCelsius, durationInSeconds);

	//Finish the dive in about 1 m deep
	if ((seaLevelPressureSetting + 100) > pressureInMillibar) {

		/////////////////////
		// Finish the dive //

		Serial.println("DIVE - Finished");

		DiveResult* diveResult = buhlmann.stopDive();

		String currentTimeText = settings.getCurrentTimeText();

		////////////////////
		// Update Logbook //

		maximumProfileNumber = maximumProfileNumber+1;
		logbook.storeDiveSummary(maximumProfileNumber, profileFile,
				diveResult->durationInSeconds, diveResult->maxDepthInMeters, minTemperatureInCelsius,
				oxygenRateSetting*100, currentTimeText.substring(0, 10), currentTimeText.substring(11, 16));

		int durationInMinutes = diveResult->durationInSeconds / 60;

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

		if (diveResult->maxDepthInMeters > logbookData->totalMaximumDepth) {
			logbookData->totalMaximumDepth = diveResult->maxDepthInMeters;
		}
		logbook.updateLogbookData(logbookData);

		///////////////////////////
		// Update Last Dive Data //

		Serial.print("Stopped timestamp: ");
		Serial.println(nowTimestamp());

		LastDiveData* lastDiveData = new LastDiveData;
		lastDiveData->diveDateTimestamp = nowTimestamp();
		lastDiveData->diveDate = currentTimeText;
		lastDiveData->maxDepthInMeters = diveResult->maxDepthInMeters;
		lastDiveData->durationInSeconds = diveResult->durationInSeconds;
		lastDiveData->noFlyTimeInMinutes = diveResult->noFlyTimeInMinutes;

		for (byte i=0; i<COMPARTMENT_COUNT; i++) {
			lastDiveData->compartmentPartialPressures[i] = diveResult->compartmentPartialPressures[i];
		}

		lastDive.storeLastDiveData(lastDiveData);

		//////////////////////////////
		// Switch to DIVE_STOP mode //

		displayScreen(SURFACE_TIME_SCREEN);

		stopDive();
	}
}

/////////////
// Buttons //
/////////////

void processRemoteButtonPress(decode_results *results)
{
	if (results->value == 0xFFFFFF) {
		return;
	} else {
		beep();
	}

	if (results->value == 0xFD8877 || results->value == 0xFF629D) {        // Up || Mode
		upButtonPressed();
	} else if (results->value == 0xFD9867 || results->value == 0xFFA857) { // Down || -
		downButtonPressed();
	} else if (results->value == 0xFD28D7 || results->value == 0xFF22DD) { // Left || Play
		leftButtonPressed();
	} else if (results->value == 0xFD6897 || results->value == 0xFFC23D) { // Right || Forward
		rightButtonPressed();
	} else if (results->value == 0xFDA857 || results->value == 0xFF02FD) { // OK || Previous
		selectButtonPressed();
	} else if (results->value == 0xFD30CF || results->value == 0xFFE01F) { // * || EQ

	} else if (results->value == 0xFD708F || results->value == 0xFF906F) { // # || +
		hashButtonPressed();
	} else if (results->value == 0xFF6897 || results->value == 0xFDB04F) { // 0
		numericButtonPressed(0);
	} else if (results->value == 0xFF30CF || results->value == 0xFD00FF) { // 1
		numericButtonPressed(1);
	} else if (results->value == 0xFF18E7 || results->value == 0xFD807F) { // 2
		numericButtonPressed(2);
	} else if (results->value == 0xFF7A85 || results->value == 0xFD40BF) { // 3
		numericButtonPressed(3);
    } else if (results->value == 0xFF10EF || results->value == 0xFD20DF) { // 4
    	numericButtonPressed(4);
    } else if (results->value == 0xFF38C7 || results->value == 0xFDA05F) { // 5
    	numericButtonPressed(5);
    } else if (results->value == 0xFF5AA5 || results->value == 0xFD609F) { // 6
    	numericButtonPressed(6);
    } else if (results->value == 0xFF42BD || results->value == 0xFD10EF) { // 7
    	numericButtonPressed(7);
    } else if (results->value == 0xFF4AB5 || results->value == 0xFD906F) { // 8
    	numericButtonPressed(8);
    } else if (results->value == 0xFF52AD || results->value == 0xFD50AF) { // 9
    	numericButtonPressed(9);
    }
}

void numericButtonPressed(byte number)
{
	if (currentScreen == PROFILE_SCREEN && number > 0) {
		logbook.drawProfileItems(&tft, currentProfileNumber, number);
	}
}

void hashButtonPressed()
{
	if (currentScreen == SETTINGS_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(MENU_SCREEN);
	} else if (currentScreen == DATETIME_SCREEN) {
		currentMode = SURFACE_MODE;
		displayScreen(SETTINGS_SCREEN);
	}
}

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
		currentMode = SURFACE_MODE;
		if (selectedSettingIndex == 5) { // Save
			displayScreen(MENU_SCREEN);
			saveSettings();
		}
		if (selectedSettingIndex == 6) { // Cancel
			displayScreen(MENU_SCREEN);
		}
		if (selectedSettingIndex == 7) { // Default
			displayScreen(MENU_SCREEN);
			setSettingsToDefault();
		}
		if (selectedSettingIndex == 8) {
			displayScreen(DATETIME_SCREEN); //Date and Time setting screen
		}
	} else if (currentScreen == DATETIME_SCREEN) {
		currentMode = SURFACE_MODE;
		if (selectedDateTimeSettingIndex == 5) { // Save
			displayScreen(SETTINGS_SCREEN);
			settings.setCurrentTime(currentDateTimeSettings);
		}
		if (selectedDateTimeSettingIndex == 6) { // Cancel
			displayScreen(SETTINGS_SCREEN);
		}
	}
}

void upButtonPressed()
{
	switch (currentScreen) {
		case MENU_SCREEN: {
			view.drawBatteryStateOfCharge(batterySoc);
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
				settingsSelect(8);
			} else {
				settingsSelect(selectedSettingIndex - 1);
			}
		}
		break;
		case DATETIME_SCREEN: {
			if (selectedDateTimeSettingIndex == 0) {
				dateTimeSettingsSelect(6);
			} else {
				dateTimeSettingsSelect(selectedDateTimeSettingIndex - 1);
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

void downButtonPressed()
{
	switch (currentScreen) {
		case MENU_SCREEN: {
			view.drawBatteryStateOfCharge(batterySoc);
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
			if (selectedSettingIndex < 8) {
				settingsSelect(selectedSettingIndex + 1);
			} else {
				settingsSelect(0);
			}
		}
		break;
		case DATETIME_SCREEN: {
			if (selectedDateTimeSettingIndex < 6) {
				dateTimeSettingsSelect(selectedDateTimeSettingIndex + 1);
			} else {
				dateTimeSettingsSelect(0);
			}
		}
		break;
		case LOGBOOK_SCREEN: {
			if (maximumProfileNumber > 0) {
				displayScreen(PROFILE_SCREEN);
			}
		}
		break;
		case ABOUT_SCREEN: {
			if (testModeSetting) {
				currentMode = SURFACE_MODE;
				displayScreen(UI_TEST_SCREEN);
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
		case DATETIME_SCREEN: {
			if (currentDateTimeSettings != NULL) {
				switch (selectedDateTimeSettingIndex) {
					case 0: { //Year
						currentDateTimeSettings->year--;
						dateTimeSettingsSelect(0);
					}
					break;
					case 1: { //Month
						if (currentDateTimeSettings->month > 0) {
							currentDateTimeSettings->month--;
						} else {
							currentDateTimeSettings->month = 12;
						}
						dateTimeSettingsSelect(1);
					}
					break;
					case 2: { //Day
						if (currentDateTimeSettings->day > 1) {
							currentDateTimeSettings->day--;
						} else {
							currentDateTimeSettings->day = 31;
						}
						dateTimeSettingsSelect(2);
					}
					break;
					case 3: { //Hour
						if (currentDateTimeSettings->hour > 0) {
							currentDateTimeSettings->hour--;
						} else {
							currentDateTimeSettings->hour = 23;
						}
						dateTimeSettingsSelect(3);
					}
					break;
					case 4: { //Minute
						if (currentDateTimeSettings->minute > 0) {
							currentDateTimeSettings->minute--;
						} else {
							currentDateTimeSettings->minute = 59;
						}
						dateTimeSettingsSelect(4);
					}
					break;
				}
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
		case DATETIME_SCREEN: {
			if (currentDateTimeSettings != NULL) {
				switch (selectedDateTimeSettingIndex) {
					case 0: { //Year
						currentDateTimeSettings->year++;
						dateTimeSettingsSelect(0);
					}
					break;
					case 1: { //Month
						if (currentDateTimeSettings->month < 12) {
							currentDateTimeSettings->month++;
						} else {
							currentDateTimeSettings->month = 1;
						}
						dateTimeSettingsSelect(1);
					}
					break;
					case 2: { //Day
						if (currentDateTimeSettings->day < 31) {
							currentDateTimeSettings->day++;
						} else {
							currentDateTimeSettings->day = 1;
						}
						dateTimeSettingsSelect(2);
					}
					break;
					case 3: { //Hour
						if (currentDateTimeSettings->hour < 23) {
							currentDateTimeSettings->hour++;
						} else {
							currentDateTimeSettings->hour = 0;
						}
						dateTimeSettingsSelect(3);
					}
					break;
					case 4: { //Minute
						if (currentDateTimeSettings->minute < 59) {
							currentDateTimeSettings->minute++;
						} else {
							currentDateTimeSettings->minute = 0;
						}
						dateTimeSettingsSelect(4);
					}
					break;
				}
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

void dateTimeSettingsSelect(byte settingIndex)
{
	view.displayDateTimeSettings(settingIndex, currentDateTimeSettings);
	selectedDateTimeSettingIndex = settingIndex;
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

		Serial.println("Settings were saved to the SD Card.");
	}

	buhlmann.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
	buhlmann.setNitrogenRateInGas(1 - oxygenRateSetting);
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

	DiveResult* diveResult;
	LastDiveData* lastDiveData;
	unsigned long surfaceIntervalInMinutes = 0;

	currentScreen = screen;
	switch (screen) {
		case MENU_SCREEN:
			// Draw the menu
			view.displayMenuScreen();
			// Select 1st menu item
			selectedMenuItemIndex = 1;
			menuSelect(selectedMenuItemIndex);
			view.drawBatteryStateOfCharge(batterySoc);
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
			currentMode = SURFACE_MODE;

			diveResult = new DiveResult;
			diveResult = buhlmann.initializeCompartments();

			lastDiveData = lastDive.loadLastDiveData();

			if (lastDiveData != NULL && lastDiveData->diveDateTimestamp > 0 ) {

			    Serial.print(F("Now timestamp: "));
			    Serial.println(nowTimestamp());
				Serial.print("Last dive timestamp: ");
				Serial.println(lastDiveData->diveDateTimestamp);

				//Calculate surface interval
				surfaceIntervalInMinutes = (nowTimestamp() - lastDiveData->diveDateTimestamp) / 60;

				Serial.print("Surface interval (min): ");
				Serial.println(surfaceIntervalInMinutes);

				//Last dive happened after 10 minutes and within 48 hours plus there is active No Fly
				if (10 < surfaceIntervalInMinutes && surfaceIntervalInMinutes < 2880 && lastDiveData->noFlyTimeInMinutes > 0) {

					Serial.print("Within 48 hours with ");
					Serial.print(lastDiveData->noFlyTimeInMinutes);
					Serial.println(" minutes of No Fly time.");

					//Copy Last Dive Data compartments
					diveResult->noFlyTimeInMinutes = lastDiveData->noFlyTimeInMinutes;
					diveResult->durationInSeconds = lastDiveData->durationInSeconds;
					diveResult->maxDepthInMeters = lastDiveData->maxDepthInMeters;
					for (byte i=0; i <COMPARTMENT_COUNT; i++) {
						diveResult->compartmentPartialPressures[i] = lastDiveData->compartmentPartialPressures[i];
					}

					//Spend the time on the surface
					buhlmann.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
					buhlmann.setNitrogenRateInGas(1 - oxygenRateSetting);
					diveResult = buhlmann.surfaceInterval(surfaceIntervalInMinutes, diveResult);

					Serial.print("After ");
					Serial.print(surfaceIntervalInMinutes);
					Serial.print(" minutes surface interval the No Fly time was changed to ");
					Serial.print(diveResult->noFlyTimeInMinutes);
					Serial.println(" minutes.");

				} else if (10 >= surfaceIntervalInMinutes && lastDiveData->noFlyTimeInMinutes > 0) {
					//Within 10 minutes of the dive don't do surface interval calculation
					Serial.println("The last dive was within 10 minutes!");

					//We would like to see the last dive details on the screen
					currentMode = DIVE_STOP_MODE;

					//Copy Last Dive Data compartments
					diveResult->noFlyTimeInMinutes = lastDiveData->noFlyTimeInMinutes;
					diveResult->durationInSeconds = lastDiveData->durationInSeconds;
					diveResult->maxDepthInMeters = lastDiveData->maxDepthInMeters;
					for (byte i=0; i <COMPARTMENT_COUNT; i++) {
						diveResult->compartmentPartialPressures[i] = lastDiveData->compartmentPartialPressures[i];
					}
				}
			}
			view.displaySurfaceTimeScreen(diveResult, surfaceIntervalInMinutes, currentMode == DIVE_STOP_MODE);
			break;
		case GAUGE_SCREEN:
			view.displayGaugeScreen(testModeSetting);
			break;
		case SETTINGS_SCREEN:
			view.displaySettingsScreen(0, seaLevelPressureSetting, oxygenRateSetting, testModeSetting, soundSetting, imperialUnitsSetting);
			break;
		case DATETIME_SCREEN:
			currentDateTimeSettings = settings.getCurrentTime();
			view.displayDateTimeSettingScreen(0, currentDateTimeSettings);
			break;
		case ABOUT_SCREEN:
			view.displayAboutScreen();
			view.drawCurrentTime(settings.getCurrentTimeText());
			view.drawBatteryStateOfCharge(batteryMonitor.getSoC());
			break;
		case UI_TEST_SCREEN:
			view.displayTestScreen();
			break;
	}
}
