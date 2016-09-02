#include "Logbook.h"

#define LOGBOOK_FILE_NAME "LOGBOOK.TXT"

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
			String line;
			int counter = 0;
			while (logbookFile.available()) {
				line = readStringUntil(&logbookFile, '\n');

				if (counter == 4) {
					logbookData->totalNumberOfDives = readIntFromLineEnd(line);
				} else if (counter == 5) {
					logbookData->totalDiveHours = readIntFromLineEnd(line);
				} else if (counter == 6) {
					logbookData->totalDiveMinutes = readIntFromLineEnd(line);
				} else if (counter == 7) {
					logbookData->totalMaximumDepth = readFloatFromLineEnd(line);
				} else if (counter == 8) {
					logbookData->lastDiveDateTime = readStringFromLineEnd(line);
				}
				counter++;
			}
			// The first 4 lines were skipped - this is the Summary section
			logbookData->numberOfStoredProfiles = counter - 14;
			logbookFile.close();
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

		logbookFile.print(F("************\n"));
		logbookFile.print(F("* Summary: *\n"));
		logbookFile.print(F("************\n"));
		logbookFile.print(NEW_LINE);
		logbookFile.print(F("Number of dives = "));
		logbookFile.print(logbookData->totalNumberOfDives);
		logbookFile.print(NEW_LINE);
		logbookFile.print(F("Logged dive hours = "));
		logbookFile.print(logbookData->totalDiveHours);
		logbookFile.print(NEW_LINE);
		logbookFile.print(F("Logged dive minutes = "));
		logbookFile.print(logbookData->totalDiveMinutes);
		logbookFile.print(NEW_LINE);
		logbookFile.print(F("Maximum depth (meter) = "));
		logbookFile.print(logbookData->totalMaximumDepth, 1);
		logbookFile.print(NEW_LINE);
		logbookFile.print(F("Last dive = "));
		logbookFile.print(logbookData->lastDiveDateTime);
		logbookFile.print(NEW_LINE);
		logbookFile.print(NEW_LINE);
		logbookFile.flush();

		logbookFile.print(F("**********\n"));
		logbookFile.print(F("* Dives: *\n"));
		logbookFile.print(F("**********\n"));
		logbookFile.print(NEW_LINE);
		logbookFile.flush();

		for (int i=1; i<=logbookData->numberOfStoredProfiles; i++) {
			logbookFile.print(getFileNameFromProfileNumber(i, false));
			logbookFile.print(NEW_LINE);
		}
		logbookFile.flush();

		logbookFile.close();
	}
}

String Logbook::getFileNameFromProfileNumber(int profileNumber, bool isTemp)
{
	//Create the name of the new profile file
	String fileName = "";
	if (isTemp) {
		fileName += "TEMP";
	} else {
		fileName += "DIVE";
	}

	if (profileNumber < 10) {
		fileName += "000";
	} else if (profileNumber < 100) {
		fileName += "00";
	} else if (profileNumber < 1000) {
		fileName += "0";
	}
	fileName += profileNumber;
	fileName += ".TXT";

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
	profileFile.print(pressure, 0);
	profileFile.print(",");
	profileFile.print(depth, 1);
	profileFile.print(",");
	profileFile.print(temperature, 1);
	profileFile.print(",");
	profileFile.println(duration);

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

		finalFile.print(F("************\n"));
		finalFile.print(F("* Summary: *\n"));
		finalFile.print(F("************\n"));
		finalFile.print(NEW_LINE);
		finalFile.print(F("Duration (seconds) = "));
		finalFile.print(duration);
		finalFile.print(NEW_LINE);
		finalFile.print(F("Maximum depth (meter) = "));
		finalFile.print(maxDepth, 1);
		finalFile.print(NEW_LINE);
		finalFile.print(F("Minimum temperature (celsius) = "));
		finalFile.print(minTemperature, 1);
		finalFile.print(NEW_LINE);
		finalFile.print(F("Oxygen percentage = "));
		finalFile.print(oxigenPercentage, 1);
		finalFile.print(NEW_LINE);
		finalFile.print(F("Dive date = "));
		finalFile.print(date);
		finalFile.print(NEW_LINE);
		finalFile.print(F("Dive time = "));
		finalFile.print(time);
		finalFile.print(NEW_LINE);
		finalFile.print(NEW_LINE);
		finalFile.print(F("**********\n"));
		finalFile.print(F("* Notes: *\n"));
		finalFile.print(F("**********\n"));
		finalFile.print(NEW_LINE);
		finalFile.print(NEW_LINE);
		finalFile.print(NEW_LINE);
		finalFile.print(NEW_LINE);
		finalFile.print(F("************\n"));
		finalFile.print(F("* Profile: *\n"));
		finalFile.print(F("************\n"));
		finalFile.print(NEW_LINE);
		finalFile.print(F("Pressure (milliBar), Depth (meter), Temperature (celsius), Duration (seconds)\n"));
		finalFile.println(F("-----------------------------------------------------------------------------"));
		finalFile.flush();

		String line;
		while (tempProfileFile.available()) {
			line = readStringUntil(&tempProfileFile, '\n');
			finalFile.print(line);
			finalFile.print('\n');
			finalFile.flush();
		}

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
