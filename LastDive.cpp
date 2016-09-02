#include "LastDive.h"

#define LASTDIVE_FILE_NAME "LASTDIVE.TXT"

const String NEW_LINE = "\n";

LastDive::LastDive() {
}

//////////////////////
// Public interface //
//////////////////////

LastDiveData* LastDive::loadLastDiveData()
{
	LastDiveData* lastDiveData = new LastDiveData;

	if (SD.exists(LASTDIVE_FILE_NAME)) {
		SdFile lastDiveFile;
		if (lastDiveFile.open(LASTDIVE_FILE_NAME, O_READ)) {
			String line;
			int counter = 0;
			while (lastDiveFile.available()) {
				line = readStringUntil(&lastDiveFile, '\n');

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

	SdFile lastDiveFile;
	if (lastDiveFile.open(LASTDIVE_FILE_NAME, O_WRITE | O_CREAT | O_APPEND)) {

		lastDiveFile.print(F("Timestamp = "));
		lastDiveFile.print(lastDiveData->diveDateTimestamp);
		lastDiveFile.print(NEW_LINE);
		lastDiveFile.print(F("Date = "));
		lastDiveFile.print(lastDiveData->diveDate);
		lastDiveFile.print(NEW_LINE);
		lastDiveFile.print(F("Max depth (m) = "));
		lastDiveFile.print(lastDiveData->maxDepthInMeters);
		lastDiveFile.print(NEW_LINE);
		lastDiveFile.print(F("Duration (sec) = "));
		lastDiveFile.print(lastDiveData->durationInSeconds);
		lastDiveFile.print(NEW_LINE);
		lastDiveFile.print(F("No fly time (min) = "));
		lastDiveFile.print(lastDiveData->noFlyTimeInMinutes);
		lastDiveFile.print(NEW_LINE);
		lastDiveFile.flush();

		for (int i=0; i<COMPARTMENT_COUNT; i++) {
			if (i<10) {
				lastDiveFile.print(F("0"));
			}
			lastDiveFile.print(i);
			lastDiveFile.print(F(" Compartment ppN2 = "));
			lastDiveFile.print(lastDiveData->compartmentPartialPressures[i]);
			lastDiveFile.print(NEW_LINE);
		}
		lastDiveFile.flush();

		lastDiveFile.close();
	}
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

String LastDive::readStringUntil(SdFile* file, char terminator)
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
