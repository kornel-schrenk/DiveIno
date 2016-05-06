#include "Logbook.h"

#define LOGBOOK_FILE_NAME "LOGBOOK.TXT"

Logbook::Logbook() {
}

//////////////////////
// Public interface //
//////////////////////

LogbookData* Logbook::loadLogbookData()
{
	LogbookData* logbookData = new LogbookData;

	if (SD.exists(LOGBOOK_FILE_NAME)) {
		File logbookFile = SD.open(LOGBOOK_FILE_NAME);
		if (logbookFile) {
			String line;
			int counter = 0;
			while (logbookFile.available()) {
				line = logbookFile.readStringUntil('\n');

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

	File logbookFile = SD.open(LOGBOOK_FILE_NAME, FILE_WRITE);

	logbookFile.print("************\n");
	logbookFile.print("* Summary: *\n");
	logbookFile.print("************\n");
	logbookFile.print("\n");
	logbookFile.print("Number of dives = ");
	logbookFile.print(logbookData->totalNumberOfDives);
	logbookFile.print("\n");
	logbookFile.print("Logged dive hours = ");
	logbookFile.print(logbookData->totalDiveHours);
	logbookFile.print("\n");
	logbookFile.print("Logged dive minutes = ");
	logbookFile.print(logbookData->totalDiveMinutes);
	logbookFile.print("\n");
	logbookFile.print("Maximum depth (meter) = ");
	logbookFile.print(logbookData->totalMaximumDepth, 1);
	logbookFile.print("\n");
	logbookFile.print("Last dive = ");
	logbookFile.print(logbookData->lastDiveDateTime);
	logbookFile.print("\n");
	logbookFile.print("\n");
	logbookFile.flush();

	logbookFile.print("**********\n");
	logbookFile.print("* Dives: *\n");
	logbookFile.print("**********\n");
	logbookFile.print("\n");
	logbookFile.flush();

	for (int i=1; i<=logbookData->numberOfStoredProfiles; i++) {
		logbookFile.print(getFileNameFromProfileNumber(i, false));
		logbookFile.print("\n");
	}
	logbookFile.flush();

	logbookFile.close();
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

File Logbook::createNewProfileFile(int profileNumber)
{
	return SD.open(getFileNameFromProfileNumber(profileNumber, true), FILE_WRITE);
}

void Logbook::storeProfileItem(File profileFile, float pressure, float depth, float temperature, int duration)
{
	profileFile.print(pressure, 0);
	profileFile.print(",");
	profileFile.print(depth, 1);
	profileFile.print(",");
	profileFile.print(temperature, 1);
	profileFile.print(",");
	profileFile.println(duration);

	profileFile.flush();
}

void Logbook::storeDiveSummary(int profileNumber, File profileFile, unsigned int duration, float maxDepth, float minTemperature, float oxigenPercentage, String date, String time)
{
	File finalFile = SD.open(getFileNameFromProfileNumber(profileNumber, false), FILE_WRITE);

	finalFile.println("************");
	finalFile.println("* Summary: *");
	finalFile.println("************");
	finalFile.println("");
	finalFile.print("Duration (seconds) = ");
	finalFile.println(duration);
	finalFile.print("Maximum depth (meter) = ");
	finalFile.println(maxDepth, 1);
	finalFile.print("Minimum temperature (celsius) = ");
	finalFile.println(minTemperature, 1);
	finalFile.print("Oxygen percentage = ");
	finalFile.println(oxigenPercentage, 1);
	finalFile.print("Dive date = ");
	finalFile.print(date);
	finalFile.print('\n');
	finalFile.print("Dive time = ");
	finalFile.print(time);
	finalFile.print('\n');
	finalFile.println("**********");
	finalFile.println("* Notes: *");
	finalFile.println("**********");
	finalFile.println("");
	finalFile.println("");
	finalFile.println("");
	finalFile.println("");
	finalFile.println("************");
	finalFile.println("* Profile: *");
	finalFile.println("************");
	finalFile.println("");
	finalFile.println("Pressure (milliBar), Depth (meter), Temperature (celsius), Duration (seconds)");
	finalFile.println("-----------------------------------------------------------------------------");
	finalFile.flush();

	profileFile.seek(0);
	String line;
	while (profileFile.available()) {
		line = profileFile.readStringUntil('\n');
		finalFile.print(line);
		finalFile.print('\n');
		finalFile.flush();
	}

	profileFile.close();
	finalFile.close();

	//Remove the temporary dive profile file

	//String to char* conversion has to be done, because remove() only works with char*
	String tempFileName = getFileNameFromProfileNumber(profileNumber, true);
	char* tempFileRef = new char[tempFileName.length()+1];
	tempFileName.toCharArray(tempFileRef, tempFileName.length()+1, 0);

	if (SD.exists(tempFileRef)) {
		SD.remove(tempFileRef);
	}
}

void Logbook::printFile(String fileName)
{
	File file = SD.open(fileName, FILE_READ);
	if (file) {
		String line;
		while (file.available()) {
			line = file.readStringUntil('\n');
			Serial.print(line);
			file.print(line);
		}
		file.close();
	}
}

ProfileData* Logbook::loadProfileDataFromFile(String profileFileName)
{
	ProfileData* profileData = new ProfileData;

	//The String has to be converted into a char array, otherwise the board will reset itself
	char fileName[profileFileName.length()+1];
	profileFileName.toCharArray(fileName, profileFileName.length()+1);

	if (SD.exists(fileName)) {
		Serial.print("Exists: ");
		Serial.println(fileName);

		File profileFile = SD.open(fileName);
		if (profileFile) {
			String line;
			int counter = 0;
			while (profileFile.available()) {
				line = profileFile.readStringUntil('\n');
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
