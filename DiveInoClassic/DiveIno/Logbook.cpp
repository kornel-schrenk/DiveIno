#include "Logbook.h"

#define LOGBOOK_FILE_NAME "Logbook.json"

const String NEW_LINE = "\n";

Logbook::Logbook() {
}

//////////////////////
// Public interface //
//////////////////////

LogbookData* Logbook::loadLogbookData()
{
	LogbookData* logbookData = new LogbookData;

	if (SD.exists(LOGBOOK_FILE_NAME)) {
		SdFile logbookFile;
		if (logbookFile.open(LOGBOOK_FILE_NAME, O_READ)) {

			char fileContent[logbookFile.fileSize()];
			int counter = 0;
			while (logbookFile.available()) {
				fileContent[counter] = logbookFile.read();
				counter++;
			}
			logbookFile.close();

			StaticJsonBuffer<JSON_OBJECT_SIZE(10)> jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(fileContent);

			logbookData->totalNumberOfDives = root["numberOfDives"];
			logbookData->totalDiveHours = root["loggedDiveHours"];
			logbookData->totalDiveMinutes = root["loggedDiveMinutes"];
			logbookData->totalMaximumDepth = root["maxDepth"];
			logbookData->lastDiveDateTime = root["lastDiveDate"].as<String>() + " " + root["lastDiveTime"].as<String>();
			logbookData->numberOfStoredProfiles = root["numberOfStoredProfiles"];
		}
	} else {
		//Create a new Logbook file, if it is not on the SD card with default values
		updateLogbookData(logbookData);
	}
	return logbookData;
}

void Logbook::updateLogbookData(LogbookData* logbookData)
{
	SD.remove(LOGBOOK_FILE_NAME);

	SdFile logbookFile;
	if (logbookFile.open(LOGBOOK_FILE_NAME, O_WRITE | O_CREAT | O_APPEND )) {

		StaticJsonBuffer<JSON_OBJECT_SIZE(10)> jsonBuffer;

		String lastDiveDate = "";
		String lastDiveTime = "";

		if (NULL != logbookData->lastDiveDateTime && logbookData->lastDiveDateTime.length() > 12) {
			lastDiveDate = logbookData->lastDiveDateTime.substring(0, 10);
			lastDiveTime = logbookData->lastDiveDateTime.substring(11);
		}

		JsonObject& root = jsonBuffer.createObject();
		root.set("numberOfDives", logbookData->totalNumberOfDives);
		root.set("loggedDiveHours", logbookData->totalDiveHours);
		root.set("loggedDiveMinutes", logbookData->totalDiveMinutes);
		root.set("maxDepth", logbookData->totalMaximumDepth);
		root.set("lastDiveDate", lastDiveDate);
		root.set("lastDiveTime", lastDiveTime);
		root.set("numberOfStoredProfiles", logbookData->numberOfStoredProfiles);

		root.prettyPrintTo(logbookFile);

		logbookFile.flush();
		logbookFile.close();
	}
}

void Logbook::printLogbook(Print* print)
{
	SdFile logbookFile;
	if (logbookFile.open(LOGBOOK_FILE_NAME, O_READ)) {
		  int data;
		  while ((data = logbookFile.read()) >= 0) {
			print->write(data);
		  }
		  logbookFile.close();
	} else {
		print->print(F("ERROR: Unable to open - "));
		print->print(LOGBOOK_FILE_NAME);
	}
}

bool Logbook::clearLogbook()
{
	bool result = SD.remove(LOGBOOK_FILE_NAME);
	if (result) {
		//Create a default logbook file
		loadLogbookData();
	}
	return result;
}

String Logbook::getFileNameFromProfileNumber(int profileNumber, bool isTemp)
{
	//Create the name of the new profile file
	String fileName = "";
	if (isTemp) {
		fileName += "Temp";
	} else {
		fileName += "Dive";
	}

	if (profileNumber < 10) {
		fileName += "000";
	} else if (profileNumber < 100) {
		fileName += "00";
	} else if (profileNumber < 1000) {
		fileName += "0";
	}
	fileName += profileNumber;
	fileName += ".json";

	return fileName;
}

/**
 * The value true is returned for success and the value false is returned for failure.
 */
bool Logbook::createNewProfileFile(int profileNumber)
{
	//The String has to be converted into a char array, otherwise the board will reset itself
    String tempProfileFileName = getFileNameFromProfileNumber(profileNumber, true);
	char tempProfileFileNameArray[tempProfileFileName.length()+1];
	tempProfileFileName.toCharArray(tempProfileFileNameArray, tempProfileFileName.length()+1);

	return profileFile.open(tempProfileFileNameArray, O_WRITE | O_CREAT | O_APPEND );
}

void Logbook::removeTempProfilefile(int profileNumber)
{
	profileFile.close();

	//The String has to be converted into a char array, otherwise the board will reset itself
    String tempProfileFileName = getFileNameFromProfileNumber(profileNumber, true);
	char tempProfileFileNameArray[tempProfileFileName.length()+1];
	tempProfileFileName.toCharArray(tempProfileFileNameArray, tempProfileFileName.length()+1);

	//Remove the temporary dive profile file
	if (SD.exists(tempProfileFileNameArray)) {
		SD.remove(tempProfileFileNameArray);
	}
}

void Logbook::storeProfileItem(float pressure, float depth, float temperature, int duration)
{
	StaticJsonBuffer<JSON_OBJECT_SIZE(5)> jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	root.set("depth", depth);
	root.set("pressure", pressure);
	root.set("duration", duration);
	root.set("temperature", temperature);

	root.printTo(profileFile);
	profileFile.flush();

	profileFile.print(F("\n"));
	profileFile.flush();

	if (!profileFile.sync() || profileFile.getWriteError()) {
		error("SD card write error during dive profile save!");
	}
}

void Logbook::storeDiveSummary(int profileNumber, unsigned int duration, float maxDepth, float minTemperature, float oxigenPercentage, String date, String time)
{
    profileFile.close();

	//The String has to be converted into a char array, otherwise the board will reset itself
    String tempProfileFileName = getFileNameFromProfileNumber(profileNumber, true);
	char tempProfileFileNameArray[tempProfileFileName.length()+1];
	tempProfileFileName.toCharArray(tempProfileFileNameArray, tempProfileFileName.length()+1);

	String finalProfileFileName = getFileNameFromProfileNumber(profileNumber, false);
	char finalProfileFileNameArray[finalProfileFileName.length()+1];
	finalProfileFileName.toCharArray(finalProfileFileNameArray, finalProfileFileName.length()+1);

	File finalFile;
	SdFile tempProfileFile;
	if (finalFile.open(finalProfileFileNameArray,  O_WRITE | O_CREAT | O_APPEND ) &&
		tempProfileFile.open(tempProfileFileNameArray, O_READ )) {

		finalFile.print(F("{ \"summary\":\n"));
		finalFile.flush();

		StaticJsonBuffer<JSON_OBJECT_SIZE(7)> jsonBuffer;

		JsonObject& root = jsonBuffer.createObject();
		root.set("diveDuration", duration);
		root.set("maxDepth", maxDepth);
		root.set("minTemperature", minTemperature);
		root.set("oxygenPercentage", oxigenPercentage);
		root.set("diveDate", date.c_str());
		root.set("diveTime", time.c_str());

		root.printTo(finalFile);
		finalFile.flush();

		finalFile.print(F(",\n"));
		finalFile.print(F("\"profile\": [\n"));
		finalFile.flush();

		bool isFirstline = true;
		String line;
		while (tempProfileFile.available()) {
			if (isFirstline) {
				isFirstline = false;
			} else {
				finalFile.print(",\n");
				finalFile.flush();
			}
			line = readStringUntil(&tempProfileFile, '\n');
			finalFile.print(line);
			finalFile.flush();
		}

		finalFile.print(F("\n]\n}\n"));
		finalFile.flush();

		tempProfileFile.close();
		finalFile.close();

		//Remove the temporary dive profile file
		if (SD.exists(tempProfileFileNameArray)) {
			SD.remove(tempProfileFileNameArray);
		}
	}
}

ProfileData* Logbook::loadProfileDataFromFile(String profileFileName)
{
	ProfileData* profileData = new ProfileData;

	//The String has to be converted into a char array, otherwise the board will reset itself
	char fileName[profileFileName.length()+1];
	profileFileName.toCharArray(fileName, profileFileName.length()+1);

	SdFile profileFile;
	if (profileFile.open(fileName, O_READ)) {
		String line;
		int counter = 0;
		while (profileFile.available()) {
			line = readStringUntil(&profileFile, '\n');
			line.trim();
			if (counter == 1) {
				String summaryJson = readJsonStringFromLineEnd(line);

				//Array conversion is required, because the JSON parser works only with arrays
				char summaryJsonArray[summaryJson.length()+1];
				summaryJson.toCharArray(summaryJsonArray, summaryJson.length()+1);

				StaticJsonBuffer<JSON_OBJECT_SIZE(10)> jsonBuffer;
				JsonObject& root = jsonBuffer.parseObject(summaryJsonArray);

				profileData->diveDuration = root["diveDuration"];
				profileData->maximumDepth = root["maxDepth"];
				profileData->minimumTemperature = root["minTemperature"];
				profileData->oxygenPercentage = root["oxygenPercentage"];
				profileData->diveDate = root["diveDate"].as<String>();
				profileData->diveTime = root["diveTime"].as<String>();
			}
			counter ++;
		}
		profileData->numberOfProfileItems = counter - 5;
		profileFile.close();

		Serial.print(fileName);
		Serial.print(F(" number of profile items: "));
		Serial.println(profileData->numberOfProfileItems);
	}
	return profileData;
}

void Logbook::drawProfileItems(UTFT* tft, int profileNumber, int pageNumber)
{
	String profileFileName = getFileNameFromProfileNumber(profileNumber, false);
	ProfileData* profileData = loadProfileDataFromFile(profileFileName);

	//The String has to be converted into a char array, otherwise the board will reset itself
	char fileName[profileFileName.length()+1];
	profileFileName.toCharArray(fileName, profileFileName.length()+1);

	if (SD.exists(fileName)) {
		File profileFile = SD.open(fileName);
		if (profileFile) {
			int firstLineNumberOnPage = 24 + ((pageNumber-1) * 460);
			int lastLineNumberOnPage = 24 + (pageNumber * 460);
			float heightUnit = 150/profileData->maximumDepth;

			tft->setColor(VGA_BLACK);
			tft->fillRect(10,160,470,315);
			tft->setColor(VGA_FUCHSIA);

			//Skip the Summary section
			profileFile.readStringUntil('\n');
			profileFile.readStringUntil('\n');
			profileFile.readStringUntil('\n');

			String line;
			int counter = 0;
			int positionOnPage = 0;
			while (profileFile.available() && counter < profileData->numberOfProfileItems) {
				line = profileFile.readStringUntil('\n');

				if (firstLineNumberOnPage < counter && counter <= lastLineNumberOnPage) {
					tft->drawLine(10+positionOnPage, 160 + heightUnit*getDepthFromProfileLine(line), 10+positionOnPage, 310);
					positionOnPage++;
				}
				counter ++;
			}
			profileFile.close();
		}
	}
}

void Logbook::printProfile(int profileNumber, Stream* out)
{
	String profileFileName = getFileNameFromProfileNumber(profileNumber, false);
	char profileFileNameArray[profileFileName.length()+1];
	profileFileName.toCharArray(profileFileNameArray, profileFileName.length()+1);

	SdFile profileFile;
	if (profileFile.open(profileFileNameArray, O_READ)) {
		unsigned long totalSize = profileFile.fileSize();
		unsigned long readSize = 0;
		byte buffer[1024];
		int readBytes;

		Serial.print(F("START dump for: "));
		Serial.println(profileFileNameArray);

		while (true) {
			readBytes = profileFile.read(buffer, sizeof(buffer));
			readSize += readBytes;
			if (readBytes == -1) {
				Serial.println(F("Profile file read error."));
				break;
			} else if (readBytes < 1024) {
				out->write(buffer, readBytes);
				out->flush();

				Serial.print(readSize);
				Serial.print("/");
				Serial.println(totalSize);

				break;
			} else {
				out->write(buffer, readBytes);
				out->flush();

				Serial.print(readSize);
				Serial.print("/");
				Serial.println(totalSize);

				delay(500); //Delay was added to switch the line back into normal state
			}
		}
		profileFile.close();

		Serial.print(F("END dump for: "));
		Serial.println(profileFileNameArray);

	} else {
		out->print(F("ERROR: The following file does not exist - "));
		out->println(profileFileName);
	}
}

bool Logbook::clearProfiles()
{
	int maximumProfileNumber = loadLogbookData()->numberOfStoredProfiles;
	if (maximumProfileNumber > 0) {

		for (int profileNumber = 1; profileNumber <= maximumProfileNumber; profileNumber++) {
		    String tempProfileFileName = getFileNameFromProfileNumber(profileNumber, false);
			char tempProfileFileNameArray[tempProfileFileName.length()+1];
			tempProfileFileName.toCharArray(tempProfileFileNameArray, tempProfileFileName.length()+1);

			if (SD.remove(tempProfileFileNameArray)) {
				Serial.print(F("PROFILE "));
				Serial.print(profileNumber);
				Serial.print(F(" - The "));
				Serial.print(tempProfileFileNameArray);
				Serial.println(F(" dive profile file was successfully removed."));
			} else {
				Serial.print(F("ERROR: The following dive profile file can't be removed: "));
				Serial.println(tempProfileFileNameArray);
				return false;
			}
		}
	}
	return true;
}

/////////////////////
// Private methods //
/////////////////////

float Logbook::getDepthFromProfileLine(String line)
{
	return line.substring(line.indexOf(':')+1, line.indexOf(',')).toFloat();
}

String Logbook::readJsonStringFromLineEnd(String line) {
	return line.substring(0, line.indexOf('}')+1);
}

String Logbook::readStringUntil(SdFile* file, char terminator)
{
  String ret;
  int c = file->read();
  while (c >= 0 && c != terminator)
  {
    ret += (char)c;
    c = file->read();
  }
  return ret;
}
