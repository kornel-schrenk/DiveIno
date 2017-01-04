#include "SPI.h"
#include "Wire.h"
#include "UTFT.h"
#include "SdFat.h"
#include "IRremote2.h"
#include "MAX17043.h"
#include "MS5803_14.h"
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

const String VERSION_NUMBER = "1.3.2";

SdFat SD;

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
UTFT_SdRaw sdFatFiles(&tft);
View view(&tft, &sdFatFiles);


// Currently elected menu item
byte selectedMenuItemIndex;

Settings settings = Settings();
float seaLevelPressureSetting = 1013.2;
float oxygenRateSetting = 0.21;
bool soundSetting = true;
bool imperialUnitsSetting = false;

DateTimeSettings* currentDateTimeSettings;

byte selectedSettingIndex = 0;
byte selectedDateTimeSettingIndex = 0;

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

Logbook logbook = Logbook();
int currentProfileNumber = 0; //There is no stored profile - default state
int maximumProfileNumber = 0;

LastDive lastDive = LastDive();

//Serial API implementation related variables
String messageBuffer = "";
bool recordMessage = false;

bool emulatorEnabled = false;
bool replayEnabled = false;

void setup() {

	Serial.begin(115200);
	// Wait for USB Serial
	while (!Serial) {
		SysCall::yield();
	}
	delay(1000);

	Serial.println(F(" ____   _              ___"));
	Serial.println(F("|  _ \\ (_)__   __ ___ |_ _| _ __    ___"));
	Serial.println(F("| | | || |\\ \\ / // _ \\ | | | '_ \\  / _ \\"));
	Serial.println(F("| |_| || | \\ V /|  __/ | | | | | || (_) |"));
	Serial.print(F("|____/ |_|  \\_/  \\___||___||_| |_| \\___/  "));

	Serial.print(F("v"));
	Serial.print(VERSION_NUMBER);
	Serial.println("\n");

	// SD Card initialization
	pinMode(csPin, OUTPUT);
	if (SD.begin(csPin, SPI_HALF_SPEED)) {
		Serial.println(F("SD card: OK\n"));
	} else {
		Serial.println(F("SD card: FAILED\n"));
		//Stop the sketch execution
		SD.initErrorHalt();
	}

	DiveInoSettings* diveInoSettings = settings.loadDiveInoSettings();
	seaLevelPressureSetting = diveInoSettings->seaLevelPressureSetting;
	oxygenRateSetting = diveInoSettings->oxygenRateSetting;
	soundSetting = diveInoSettings->soundSetting;
	imperialUnitsSetting = diveInoSettings->imperialUnitsSetting;

	Serial.println(F("Settings:\n"));
	Serial.print(F("Pressure: "));
	Serial.print(seaLevelPressureSetting, 2);
	Serial.println(F(" millibar"));
	Serial.print(F("Oxygen %: "));
	Serial.println(oxygenRateSetting, 2);
	Serial.print(F("Sound:    "));
	if (soundSetting) {
		Serial.println(F("On"));
	} else {
		Serial.println(F("Off"));
	}
	Serial.print(F("Units:    "));
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

    if (sensor.initializeMS_5803(false)) {
      Serial.println(F("Pressure sensor: OK\n"));
    }
    else {
      Serial.println(F("Pressure sensor: FAILED\n"));
    }

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	setSyncProvider((unsigned long int (*)())RTC.get);
#endif
    Serial.print(F("Time: "));
    Serial.print(settings.getCurrentTimeText());

    Serial.print(F(" = "));
    Serial.println(nowTimestamp());
    Serial.println("");

	tft.InitLCD();

	batterySoc = batteryMonitor.getSoC();

	Serial.print(F("Battery: "));
	Serial.print(batterySoc, 0);
	Serial.println(F(" %\n"));

	displayScreen(MENU_SCREEN);

	Serial.println(F("READY\n"));
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
		if (replayEnabled) {
			replayDive();
		} else {
			dive();
		}
	} else if (currentScreen == MENU_SCREEN) {
		//Read messages from the standard Serial interface
		if (Serial.available() > 0) {
			readMessageFromSerial(Serial.read());
		}
	} else if (currentScreen == ABOUT_SCREEN) {
		unsigned int measurementDifference = nowTimestamp() - timerTimestamp;

		if (measurementDifference > 0) {
			timerTimestamp = nowTimestamp();

			//Refresh the current time and battery information
			view.drawCurrentTime(settings.getCurrentTimeText());
			view.drawBatteryStateOfCharge(batterySoc);
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

////////////////
// Serial API //
////////////////

void readMessageFromSerial(char data)
{
	if (data == '@') {
		messageBuffer = "";
		recordMessage = true;
	} else if (data == '#') {
		handleMessage(messageBuffer);
		recordMessage = false;
	} else {
		if (recordMessage) {
			messageBuffer += data;
		}
	}
}

void handleMessage(String message) {
	String responseMessage = "";
	if (message.startsWith(F("PROFILE")) || message.startsWith(F("profile"))) {
		int profileNumber = message.substring(7).toInt();
		if (profileNumber > 0) {
			logbook.printProfile(profileNumber);
		} else {
			responseMessage += F("ERROR: Invalid argument - Positive Integer value expected!");
			Serial.println(responseMessage);
		}
	} else if (message.startsWith(F("LOGBOOK")) || message.startsWith(F("logbook"))) {
		logbook.printLogbook();
	} else if (message.startsWith(F("EMULATOR")) || message.startsWith(F("emulator"))) {
		String onOffEmulator = message.substring(8);
		onOffEmulator.trim();
		if (onOffEmulator.startsWith(F("on")) || onOffEmulator.startsWith(F("ON"))) {
			emulatorEnabled = true;
			responseMessage += F("EMULATOR - Enabled");
		} else {
			emulatorEnabled = false;
			responseMessage += F("EMULATOR - Disabled");
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("REPLAY")) || message.startsWith(F("replay"))) {
		String onOffReplay = message.substring(6);
		onOffReplay.trim();
		if (onOffReplay.startsWith(F("on")) || onOffReplay.startsWith(F("ON"))) {
			replayEnabled = true;
			responseMessage += F("REPLAY - Enabled");
		} else {
			replayEnabled = false;
			responseMessage += F("REPLAY - Disabled");
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("VERSION")) || message.startsWith(F("version"))) {
		responseMessage += VERSION_NUMBER;
		Serial.println(responseMessage);
	} else if (message.startsWith(F("DIAG")) || message.startsWith(F("diag"))) {
		String what = message.substring(4);
		what.trim();
		if (what.startsWith(F("TEMP")) || what.startsWith(F("temp"))) {
			sensor.readSensor();
			responseMessage += sensor.temperature();
		} else if (what.startsWith(F("PRES")) || what.startsWith(F("pres"))) {
			sensor.readSensor();
			responseMessage += sensor.pressure();
		} else if (what.startsWith(F("BAT")) || what.startsWith(F("bat"))) {
			responseMessage += batterySoc;
		} else if (what.startsWith(F("DATE")) || what.startsWith(F("date"))) {
			responseMessage += settings.getCurrentTimeText();
		} else if (what.startsWith(F("TIMESTAMP")) || what.startsWith(F("timestamp"))) {
			responseMessage += nowTimestamp();
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("SETTINGS")) || message.startsWith(F("settings"))) {
		settings.printSettings();
	} else if (message.startsWith(F("SOUND")) || message.startsWith(F("sound"))) {
		String state = message.substring(5);
		state.trim();
		if (state.startsWith(F("ON")) || state.startsWith(F("on"))) {
			soundSetting = true;
			responseMessage += F("SOUND - ON");
		} else {
			soundSetting = false;
			responseMessage += F("SOUND - OFF");
		}
		saveSettings();
		Serial.println(responseMessage);
	} else if (message.startsWith(F("METRIC")) || message.startsWith(F("metric"))) {
		String state = message.substring(5);
		state.trim();
		if (state.startsWith(F("ON")) || state.startsWith(F("on"))) {
			imperialUnitsSetting = false;
			responseMessage += F("METRIC - ON");
		} else {
			imperialUnitsSetting = true;
			responseMessage += F("METRIC - OFF");
		}
		saveSettings();
		Serial.println(responseMessage);
	} else if (message.startsWith(F("PRESSURE")) || message.startsWith(F("pressure"))) {
		float seaLevelPressure = message.substring(8).toFloat();
		if (seaLevelPressure > 900 && seaLevelPressure < 1200) {
			seaLevelPressureSetting = seaLevelPressure;
			responseMessage += F("PRESSURE - ");
			responseMessage += seaLevelPressureSetting;
			saveSettings();
		} else {
			responseMessage += F("ERROR: Invalid argument - Must be between 900 and 1200 millibar!");
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("OXYGEN")) || message.startsWith(F("oxygen"))) {
		float oxygenRate = message.substring(6).toFloat();
		if (oxygenRate >= 0.16 && oxygenRate <= 0.5) {
			oxygenRateSetting = oxygenRate;
			responseMessage += F("OXYGEN - ");
			responseMessage += oxygenRateSetting;
			saveSettings();
		} else {
			responseMessage += F("ERROR: Invalid argument - Must be between 0.16 and 0.50!");
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("DEFAULT")) || message.startsWith(F("default"))) {
		setSettingsToDefault();
		saveSettings();
		responseMessage += F("DEFAULT settings were set.");
		Serial.println(responseMessage);
	} else if (message.startsWith(F("DATETIME")) || message.startsWith(F("datetime"))) {
		String dateTime = message.substring(8);
		dateTime.trim();

		int year = 2016;
		int month = 0;
		int day = 0;
		int hour = 0;
		int minute = 0;
		int second = 0;

		year = dateTime.substring(0, 4).toInt();
		month = dateTime.substring(5, 7).toInt();
		day = dateTime.substring(8, 10).toInt();
		hour = dateTime.substring(11, 13).toInt();
		minute = dateTime.substring(14, 16).toInt();

		responseMessage += F("Year: ");
		responseMessage += year;
		responseMessage += F(" Month: ");
		responseMessage += month;
		responseMessage += F(" Day: ");
		responseMessage += day;

		responseMessage += F(" Hour: ");
		responseMessage += hour;
		responseMessage += F(" Minute: ");
		responseMessage += minute;
		responseMessage += "\n";

		if (year < 2017) {
			responseMessage += F("ERROR: Invalid argument - Year must be greater than 2017!");
		} else if (month < 1 || month > 12) {
			responseMessage += F("ERROR: Invalid argument - Month must be between 1 and 12!");
		} else if (day < 1 || day > 31) {
			responseMessage += F("ERROR: Invalid argument - Day must be between 1 and 31!");
		} else if (hour < 0 || hour > 23) {
			responseMessage += F("ERROR: Invalid argument - Hour must be between 0 and 23!");
		} else if (minute < 0 || minute > 59) {
			responseMessage += F("ERROR: Invalid argument - Minute must be between 0 and 59!");
		} else {
			DateTimeSettings* dateTimeSettings = new DateTimeSettings;
			dateTimeSettings->year = year;
			dateTimeSettings->month = month;
			dateTimeSettings->day = day;
			dateTimeSettings->hour = hour;
			dateTimeSettings->minute = minute;
			dateTimeSettings->second = 0;

			settings.setCurrentTime(dateTimeSettings);

			responseMessage += F("DATETIME settings were set to ");
			responseMessage += settings.getCurrentTimeText();
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("CLEAR")) || message.startsWith(F("clear"))) {
		if (lastDive.clearLastDiveData()) {
			responseMessage += F("CLEAR - OK");
		} else {
			responseMessage += F("ERROR: Surface time clean operation failed!");
		}
		Serial.println(responseMessage);
	} else if (message.startsWith(F("RESET")) || message.startsWith(F("reset"))) {
		//Revert settings to default
		responseMessage += F("SETTINGS - Default settings will be restored.\n");
		setSettingsToDefault();
		saveSettings();

		//Remove last dive information
		if (lastDive.clearLastDiveData()) {
			responseMessage += F("LAST DIVE - Last dive information was deleted.\n");
		} else {
			responseMessage += F("ERROR: Failed to delete last dive information!\n");
		}

		//Remove all dive profile files
		if (logbook.clearProfiles()) {
			responseMessage += F("PROFILE - All dive profiles were successfully deleted.\n");
		} else {
			responseMessage += F("ERROR: Failed to delete all dive profiles!\n");
		}

		//Reset logbook
		if (logbook.clearLogbook()) {
			responseMessage += F("LOGBOOK - Reset was successful.\n");
		} else {
			responseMessage += F("ERROR: Logbook reset failed!\n");
		}
		Serial.println(responseMessage);
	}

	Serial.flush();
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

				Serial.print(F(" Safety stop STARTED at "));
				Serial.println(depthInMeter);

			} else if (safetyStopState == SAFETY_STOP_VIOLATED) {
				safetyStopState = SAFETY_STOP_IN_PROGRESS;
			}
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				safetyStopDurationInSeconds += intervalDuration;
				view.drawSafetyStop(safetyStopDurationInSeconds);

				Serial.print(F(" Safety stop IN PROGRESS at "));
				Serial.println(depthInMeter);
				Serial.print(F(" Safety stop duration: "));
				Serial.println(safetyStopDurationInSeconds);

				if (180 <= safetyStopDurationInSeconds) {
					safetyStopState = SAFETY_STOP_DONE;

					Serial.print(F(" Safety stop DONE at "));
					Serial.println(depthInMeter);
				}
			}
		} else if (10 < depthInMeter) {
			//Reset the safety stop mode, if the diver descends back down to 10 m
			if (safetyStopState != SAFETY_STOP_NOT_STARTED) {
				safetyStopState = SAFETY_STOP_NOT_STARTED;

				Serial.print(F(" Safety stop RESET after DONE at "));
				Serial.println(depthInMeter);
			}
		} else {
			//Between 10-6 meters and above 3 meters the safety stop is violated - the counter will stop
			if (safetyStopState == SAFETY_STOP_IN_PROGRESS) {
				safetyStopState = SAFETY_STOP_VIOLATED;

				Serial.print(F(" Safety stop VIOLATED at "));
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
		view.drawMaximumDepth(maxDepthInMeter, imperialUnitsSetting);
	}
	if (maxTemperatureInCelsius < temperatureInCelsius) {
		maxTemperatureInCelsius = temperatureInCelsius;
	}
	if (minTemperatureInCelsius > temperatureInCelsius) {
		minTemperatureInCelsius = temperatureInCelsius;
	}
}

void replayDive()
{
	// Check if test data is available, which comes through the serial interface
	if (Serial.available() > 0) {
		float pressureInMillibar = Serial.readStringUntil(',').toFloat();
		float depthInMeter = Serial.readStringUntil(',').toFloat();
		diveDurationInSeconds = Serial.readStringUntil(',').toInt();
		float temperatureInCelsius = Serial.readStringUntil(',').toFloat();

		Serial.print(F("REPLAY: "));
		Serial.print(pressureInMillibar, 1);
		Serial.print(F(" mbar, "));
		Serial.print(depthInMeter, 1);
		Serial.print(F(" m, "));
		Serial.print(diveDurationInSeconds);
		Serial.print(F(" sec, "));
		Serial.print(temperatureInCelsius, 1);
		Serial.println(F(" cel\n"));

		view.drawCurrentPressure(pressureInMillibar);
		view.drawDepth(depthInMeter, imperialUnitsSetting);
		view.drawCurrentTemperature(temperatureInCelsius, imperialUnitsSetting);
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

void dive()
{
	unsigned int measurementDifference = nowTimestamp() - timerTimestamp;

	if (measurementDifference > 0) {
		timerTimestamp = nowTimestamp();

		float pressureInMillibar = seaLevelPressureSetting;
		float temperatureInCelsius = 99;
		if (emulatorEnabled) {
			Serial.println(F("@GET#"));
			if (Serial.available() > 0) {
				pressureInMillibar = Serial.parseFloat();
				temperatureInCelsius = Serial.parseFloat();
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
		view.drawCurrentTemperature(temperatureInCelsius, imperialUnitsSetting);
		view.drawCurrentPressure(pressureInMillibar);
		view.drawDepth(depthInMeter, imperialUnitsSetting);

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
	if (emulatorEnabled) {
		Serial.println(F("@START#"));
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

		Serial.println(F("DIVE - Started"));

		//Start dive profile logging - create a new file
		if (maximumProfileNumber <= 0) {
			maximumProfileNumber = logbook.loadLogbookData()->numberOfStoredProfiles;
		}
		if (!logbook.createNewProfileFile(maximumProfileNumber+1)) {
			Serial.println("ERROR: Temporary profile file creation was failed.");
			displayScreen(MENU_SCREEN);
			return;
		}

		DiveResult* diveResult = new DiveResult;
		diveResult = buhlmann.initializeCompartments();

		//Retrieve last dive data
		LastDiveData* lastDiveData = lastDive.loadLastDiveData();

		//Last dive happened within 48 hours and there is an active no fly time
		if (lastDiveData != NULL &&	(lastDiveData->diveDateTimestamp + 172800) > nowTimestamp() && lastDiveData->noFlyTimeInMinutes > 0) {

			Serial.print(F("BEFORE dive No Fly time (min): "));
			Serial.println(lastDiveData->noFlyTimeInMinutes);

			//Copy Last Dive Data
			diveResult->noFlyTimeInMinutes = lastDiveData->noFlyTimeInMinutes;
			diveResult->previousDiveDateTimestamp = lastDiveData->diveDateTimestamp;
			diveResult->wasDecoDive = lastDiveData->wasDecoDive;
			for (byte i=0; i <COMPARTMENT_COUNT; i++) {
				diveResult->compartmentPartialPressures[i] = lastDiveData->compartmentPartialPressures[i];

		        Serial.print(F("BEFORE: Dive compartment "));
		        Serial.print(i);
		        Serial.print(F(": "));
		        Serial.print(diveResult->compartmentPartialPressures[i], 0);
		        Serial.println(F(" ppN2"));
			}

			//Calculate surface time in minutes
			int surfaceTime = (nowTimestamp() - lastDiveData->diveDateTimestamp) / 60;

			Serial.print(F("Surface time (min): "));
			Serial.println(surfaceTime);

			//Spend the time on the surface
			diveResult = buhlmann.surfaceInterval(surfaceTime, diveResult);

			for (byte i=0; i <COMPARTMENT_COUNT; i++) {
		        Serial.print(F("AFTER: Dive compartment "));
		        Serial.print(i);
		        Serial.print(F(": "));
		        Serial.print(diveResult->compartmentPartialPressures[i], 0);
		        Serial.println(F(" ppN2"));
			}

			Serial.print(F("AFTER dive No Fly time (min): "));
			Serial.println(diveResult->noFlyTimeInMinutes);
		}

		buhlmann.startDive(diveResult, nowTimestamp());
	}
}

void stopDive()
{
	if (emulatorEnabled) {
		Serial.println(F("@STOP#"));
	}
	Serial.println(F("\nDIVE - Stopped"));
}

void diveProgress(float temperatureInCelsius, float pressureInMillibar, float depthInMeter, unsigned int durationInSeconds) {

	DiveInfo diveInfo = buhlmann.progressDive(pressureInMillibar, durationInSeconds);
	view.drawAscend(diveInfo.ascendRate);

	view.drawDecoArea(diveInfo, imperialUnitsSetting);

	//Store the current dive data in the dive profile
	logbook.storeProfileItem(pressureInMillibar, depthInMeter, temperatureInCelsius, durationInSeconds);

	//Finish the dive in about 1 m deep
	if ((seaLevelPressureSetting + 100) > pressureInMillibar) {

		/////////////////////
		// Finish the dive //

		Serial.println(F("DIVE - Finished"));

		DiveResult* diveResult = buhlmann.stopDive(nowTimestamp());

		String currentTimeText = settings.getCurrentTimeText();

		////////////////////
		// Update Logbook //

		maximumProfileNumber = maximumProfileNumber+1;
		logbook.storeDiveSummary(maximumProfileNumber,
				diveResult->durationInSeconds, diveResult->maxDepthInMeters, minTemperatureInCelsius,
				oxygenRateSetting*100, currentTimeText.substring(0, 10), currentTimeText.substring(11, 16));

		int durationInMinutes = diveResult->durationInSeconds / 60;

		LogbookData* logbookData = logbook.loadLogbookData();
		logbookData->totalNumberOfDives++;
		logbookData->numberOfStoredProfiles++;
		logbookData->lastDiveDateTime = currentTimeText.substring(0, 16);

		//Add the dive duration to the overall logged duration
		durationInMinutes = logbookData->totalDiveMinutes + durationInMinutes;

		logbookData->totalDiveHours += durationInMinutes / 60;
		logbookData->totalDiveMinutes = durationInMinutes % 60;

		if (diveResult->maxDepthInMeters > logbookData->totalMaximumDepth) {
			logbookData->totalMaximumDepth = diveResult->maxDepthInMeters;
		}
		logbook.updateLogbookData(logbookData);

		///////////////////////////
		// Update Last Dive Data //

		Serial.print(F("Dive stop timestamp: "));
		Serial.println(nowTimestamp());

		LastDiveData* lastDiveData = new LastDiveData;
		lastDiveData->diveDateTimestamp = nowTimestamp();
		lastDiveData->diveDate = currentTimeText.substring(0, 10);
		lastDiveData->diveTime = currentTimeText.substring(11);
		lastDiveData->maxDepthInMeters = diveResult->maxDepthInMeters;
		lastDiveData->durationInSeconds = diveResult->durationInSeconds;
		lastDiveData->noFlyTimeInMinutes = diveResult->noFlyTimeInMinutes;
		lastDiveData->wasDecoDive = diveResult->wasDecoDive;

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
		if (selectedSettingIndex == 4) { // Save
			displayScreen(MENU_SCREEN);
			saveSettings();
		}
		if (selectedSettingIndex == 5) { // Cancel
			displayScreen(MENU_SCREEN);
		}
		if (selectedSettingIndex == 6) { // Default
			displayScreen(MENU_SCREEN);
			setSettingsToDefault();
		}
		if (selectedSettingIndex == 7) {
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
				settingsSelect(7);
			} else {
				settingsSelect(selectedSettingIndex - 1);
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
		case SETTINGS_SCREEN: {
			if (selectedSettingIndex < 7) {
				settingsSelect(selectedSettingIndex + 1);
			} else {
				settingsSelect(0);
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
		case LOGBOOK_SCREEN: {
			if (maximumProfileNumber > 0) {
				displayScreen(PROFILE_SCREEN);
			}
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
			switch (selectedSettingIndex) {
				case 0: {
					if (seaLevelPressureSetting > 1001) {
						seaLevelPressureSetting = seaLevelPressureSetting - 0.1;
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
					soundSetting = !soundSetting;
					settingsSelect(2);
				}
				break;
				case 3: {
					imperialUnitsSetting = !imperialUnitsSetting;
					settingsSelect(3);
				}
				break;
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
			switch (selectedSettingIndex) {
				case 0: {
					if (seaLevelPressureSetting < 1100) {
						seaLevelPressureSetting = seaLevelPressureSetting + 0.1;
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
					soundSetting = !soundSetting;
					settingsSelect(2);
				}
				break;
				case 3: {
					imperialUnitsSetting = !imperialUnitsSetting;
					settingsSelect(3);
				}
				break;
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
	view.displaySettings(settingIndex, seaLevelPressureSetting, oxygenRateSetting, soundSetting, imperialUnitsSetting);
	selectedSettingIndex = settingIndex;
}

void dateTimeSettingsSelect(byte settingIndex)
{
	view.displayDateTimeSettings(settingIndex, currentDateTimeSettings);
	selectedDateTimeSettingIndex = settingIndex;
}

void setSettingsToDefault()
{
	seaLevelPressureSetting = 1013.2;
	oxygenRateSetting = 0.21;
	soundSetting = true;
	imperialUnitsSetting = false;
}

void saveSettings()
{
	DiveInoSettings* diveInoSettings = new DiveInoSettings;
	diveInoSettings->seaLevelPressureSetting = seaLevelPressureSetting;
	diveInoSettings->oxygenRateSetting = oxygenRateSetting;
	diveInoSettings->soundSetting = soundSetting;
	diveInoSettings->imperialUnitsSetting = imperialUnitsSetting;
	if (settings.saveDiveInoSettings(diveInoSettings)) {

		buhlmann.setSeaLevelAtmosphericPressure(seaLevelPressureSetting);
		buhlmann.setNitrogenRateInGas(1 - oxygenRateSetting);

		Serial.println(F("SETTINGS - Saved to the SD Card."));
	} else {
		Serial.println(F("ERROR: Save operation failed - Settings were not saved to the SD Card."));
	}
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
	unsigned long surfaceIntervalInMinutes = 0L;

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
			view.displayLogbookScreen(logbookData, imperialUnitsSetting);
			break;
		case PROFILE_SCREEN:
			if (maximumProfileNumber != 0 && currentProfileNumber != 0) {
				view.displayProfileScreen(logbook.loadProfileDataFromFile(logbook.getFileNameFromProfileNumber(currentProfileNumber, false)), currentProfileNumber, imperialUnitsSetting);
			}
			break;
		case SURFACE_TIME_SCREEN:
			currentMode = SURFACE_MODE;

			diveResult = new DiveResult;
			diveResult = buhlmann.initializeCompartments();

			lastDiveData = lastDive.loadLastDiveData();

			if (lastDiveData != NULL && lastDiveData->diveDateTimestamp > 0 ) {

				//Calculate surface interval
				surfaceIntervalInMinutes = (nowTimestamp() - lastDiveData->diveDateTimestamp) / 60;

				Serial.print(F("Surface interval (min): "));
				Serial.println(surfaceIntervalInMinutes);

				//Last dive happened after 10 minutes and within 48 hours plus there is active No Fly
				if (10 < surfaceIntervalInMinutes && surfaceIntervalInMinutes < 2880 && lastDiveData->noFlyTimeInMinutes > 0) {

					Serial.print(F("Within 48 hours with "));
					Serial.print(lastDiveData->noFlyTimeInMinutes);
					Serial.println(F(" minutes of No Fly time."));

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

					Serial.print(F("After "));
					Serial.print(surfaceIntervalInMinutes);
					Serial.print(F(" minutes surface interval the No Fly time was changed to "));
					Serial.print(diveResult->noFlyTimeInMinutes);
					Serial.println(F(" minutes."));

				} else if (10 >= surfaceIntervalInMinutes && lastDiveData->noFlyTimeInMinutes > 0) {
					//Within 10 minutes of the dive don't do surface interval calculation
					Serial.println(F("The last dive was within 10 minutes!"));

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
			view.displaySurfaceTimeScreen(diveResult, surfaceIntervalInMinutes, currentMode == DIVE_STOP_MODE, imperialUnitsSetting);
			break;
		case GAUGE_SCREEN:
			view.displayGaugeScreen();
			break;
		case SETTINGS_SCREEN:
			view.displaySettingsScreen(0, seaLevelPressureSetting, oxygenRateSetting, soundSetting, imperialUnitsSetting);
			break;
		case DATETIME_SCREEN:
			currentDateTimeSettings = settings.getCurrentTime();
			view.displayDateTimeSettingScreen(0, currentDateTimeSettings);
			break;
		case ABOUT_SCREEN:
			timerTimestamp = nowTimestamp();
			view.displayAboutScreen(VERSION_NUMBER);
			view.drawCurrentTime(settings.getCurrentTimeText());
			view.drawBatteryStateOfCharge(batterySoc);
			break;
		case UI_TEST_SCREEN:
			view.displayTestScreen();
			break;
	}
}
