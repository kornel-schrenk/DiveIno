#include "LastDive.h"

#define LASTDIVE_FILE_NAME "LASTDIVE.TXT"

LastDive::LastDive() {
}

//////////////////////
// Public interface //
//////////////////////

LastDiveData* LastDive::loadLastDiveData()
{
	LastDiveData* lastDiveData = new LastDiveData;

	if (SD.exists(LASTDIVE_FILE_NAME)) {
		File lastDiveFile = SD.open(LASTDIVE_FILE_NAME);
		if (lastDiveFile) {
			String line;
			int counter = 0;
			while (lastDiveFile.available()) {
				line = lastDiveFile.readStringUntil('\n');

				if (counter == 0) {
					lastDiveData->diveDateTimestamp = readLongFromLineEnd(line);
				} else if (counter == 1) {
					lastDiveData->diveDate = readStringFromLineEnd(line);
				} else if (counter == 2) {
					lastDiveData->maxDepthInMeters = readFloatFromLineEnd(line);
				} else if (counter == 3) {
					lastDiveData->durationInSeconds = readIntFromLineEnd(line);
				} else if (counter == 4) {
					lastDiveData->noFlyTimeInMinutes = readIntFromLineEnd(line);
				} else {
					lastDiveData->compartmentPartialPressures[counter-5] = readFloatFromLineEnd(line);
				}
				counter++;
			}
			lastDiveFile.close();
		}
	}
	return lastDiveData;
}

void LastDive::storeLastDiveData(LastDiveData* lastDiveData)
{
	SD.remove(LASTDIVE_FILE_NAME);

	File lastDiveFile = SD.open(LASTDIVE_FILE_NAME, FILE_WRITE);

	lastDiveFile.print("Timestamp = ");
	lastDiveFile.print(lastDiveData->diveDateTimestamp);
	lastDiveFile.print("\n");
	lastDiveFile.print("Date = ");
	lastDiveFile.print(lastDiveData->diveDate);
	lastDiveFile.print("\n");
	lastDiveFile.print("Max depth (m) = ");
	lastDiveFile.print(lastDiveData->maxDepthInMeters);
	lastDiveFile.print("\n");
	lastDiveFile.print("Duration (sec) = ");
	lastDiveFile.print(lastDiveData->durationInSeconds);
	lastDiveFile.print("\n");
	lastDiveFile.print("No fly time (min) = ");
	lastDiveFile.print(lastDiveData->noFlyTimeInMinutes);
	lastDiveFile.print("\n");
	lastDiveFile.flush();

	for (int i=0; i<COMPARTMENT_COUNT; i++) {
		if (i<10) {
			lastDiveFile.print("0");
		}
		lastDiveFile.print(i);
		lastDiveFile.print(" Compartment ppN2 = ");
		lastDiveFile.print(lastDiveData->compartmentPartialPressures[i]);
		lastDiveFile.print("\n");
	}
	lastDiveFile.flush();

	lastDiveFile.close();
}

/////////////////////
// Private methods //
/////////////////////

long LastDive::readLongFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toInt();
}

int LastDive::readIntFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toInt();
}

float LastDive::readFloatFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+1).toFloat();
}

String LastDive::readStringFromLineEnd(String line) {
	return line.substring(line.indexOf('=')+2);
}
