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
			//Skip the Summary section
			logbookFile.seek(40);

			String line;
			int counter = 0;
			while (logbookFile.available()) {
				line = logbookFile.readStringUntil('\n');

				if (counter == 0) {
					logbookData->totalNumberOfDives = readIntFromLineEnd(line);
				} else if (counter == 1) {
					logbookData->totalDiveHours = readIntFromLineEnd(line);
				} else if (counter == 2) {
					logbookData->totalDiveMinutes = readIntFromLineEnd(line);
				} else if (counter == 3) {
					logbookData->totalMaximumDepth = readFloatFromLineEnd(line);
				} else if (counter == 4) {
					logbookData->lastDiveDateTime = readStringFromLineEnd(line);
				}
				counter++;
			}
			// The first 4 lines were skipped - this is the Summary section
			logbookData->numberOfStoredProfiles = counter - 10;
			logbookFile.close();
		}
	}
	return logbookData;
}

String Logbook::getProfileFileName(int profileNumber)
{
	String fileName = "";
	if (SD.exists(LOGBOOK_FILE_NAME)) {
		File logbookFile = SD.open(LOGBOOK_FILE_NAME);
		if (logbookFile) {
			String line;
			int counter = 0;
			while (logbookFile.available() && counter < profileNumber + 14) {
				fileName = logbookFile.readStringUntil('\n');
				counter++;
			}
			logbookFile.close();
		}
	}
	return fileName;
}

ProfileData* Logbook::loadProfileDataFromFile(String profileFileName)
{
	ProfileData* profileData = new ProfileData;

	//The String has to be converted into a char array, otherwise the board will reset itself
	char fileName[profileFileName.length()+1];
	profileFileName.toCharArray(fileName, profileFileName.length()+1);

	if (SD.exists(fileName)) {
		File profileFile = SD.open(fileName);
		if (profileFile) {
			//Skip the Summary section
			profileFile.seek(40);

			String line;
			int counter = 0;
			while (profileFile.available()) {
				line = profileFile.readStringUntil('\n');

				if (counter == 0) {
					profileData->diveDuration = readIntFromLineEnd(line);
				} else if (counter == 1) {
					profileData->maximumDepth = readFloatFromLineEnd(line);
				} else if (counter == 2) {
					profileData->minimumTemperature = readFloatFromLineEnd(line);
				} else if (counter == 3) {
					profileData->oxigenPercentage = readFloatFromLineEnd(line);
				} else if (counter == 4) {
					profileData->diveDate = readStringFromLineEnd(line);
				} else if (counter == 5) {
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

/////////////////////
// Private methods //
/////////////////////

int Logbook::readIntFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toInt();
}

float Logbook::readFloatFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toFloat();
}

String Logbook::readStringFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+2);
}
