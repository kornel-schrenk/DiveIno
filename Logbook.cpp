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

			StaticJsonBuffer<JSON_OBJECT_SIZE(8)+JSON_ARRAY_SIZE(16)> jsonBuffer;
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

		StaticJsonBuffer<JSON_OBJECT_SIZE(7)> jsonBuffer;

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

void Logbook::storeProfileItem(float pressure, float depth, float temperature, int duration)
{
	StaticJsonBuffer<JSON_OBJECT_SIZE(4)> jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	root.set("depth", depth);
	root.set("pressure", pressure, 5);
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

		StaticJsonBuffer<JSON_OBJECT_SIZE(6)> jsonBuffer;

		JsonObject& root = jsonBuffer.createObject();
		root.set("diveDuration", duration);
		root.set("maxDepth", maxDepth);
		root.set("minTemperature", minTemperature);
		root.set("oxigenPercentage", oxigenPercentage);
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
		Serial.print(F("Exists: "));
		Serial.println(fileName);

		String line;
		int counter = 0;
		while (profileFile.available()) {
			line = readStringUntil(&profileFile, '\n');
			line.trim();
			if (counter == 4) {
				profileData->diveDuration = readIntFromLineEnd(line);
			} else if (counter == 5) {
				profileData->maximumDepth = readFloatFromLineEnd(line);
			} else if (counter == 6) {
				profileData->minimumTemperature = readFloatFromLineEnd(line);
			} else if (counter == 7) {
				profileData->oxigenPercentage = readFloatFromLineEnd(line);
			} else if (counter == 8) {
				profileData->diveDate = readStringFromLineEnd(line);
			} else if (counter == 9) {
				profileData->diveTime = readStringFromLineEnd(line);
			}
			counter ++;
		}
		profileData->numberOfProfileItems = counter - 24;
		profileFile.close();
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
			//Skip the Summary section
			profileFile.seek(40);

			int firstLineNumberOnPage = 24 + ((pageNumber-1) * 460);
			int lastLineNumberOnPage = 24 + (pageNumber * 460);
			float heightUnit = 150/profileData->maximumDepth;

			tft->setColor(VGA_BLACK);
			tft->fillRect(10,160,470,315);
			tft->setColor(VGA_FUCHSIA);

			String line;
			int counter = 0;
			int positionOnPage = 0;
			while (profileFile.available()) {
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

/////////////////////
// Private methods //
/////////////////////

float Logbook::getDepthFromProfileLine(String line)
{
	return line.substring(line.indexOf(',')+1).substring(0, line.indexOf(',')).toFloat();
}

int Logbook::readIntFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toInt();
}

float Logbook::readFloatFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toFloat();
}

String Logbook::readStringFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+2);
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
