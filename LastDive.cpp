#include "LastDive.h"

#define LASTDIVE_FILE_NAME "Lastdive.json"

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

			char fileContent[lastDiveFile.fileSize()];
			int counter = 0;
			while (lastDiveFile.available()) {
				fileContent[counter] = lastDiveFile.read();
				counter++;
			}
			lastDiveFile.close();

			StaticJsonBuffer<JSON_OBJECT_SIZE(8)+JSON_ARRAY_SIZE(16)> jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(fileContent);

			lastDiveData->diveDateTimestamp = root["timestamp"];
			lastDiveData->diveDate = root["diveDate"].as<String>();
			lastDiveData->diveTime = root["diveTime"].as<String>();
			lastDiveData->durationInSeconds = root["diveDuration"];
			lastDiveData->noFlyTimeInMinutes = root["noFlyTime"];
			lastDiveData->maxDepthInMeters = root["maxDepth"];
			lastDiveData->wasDecoDive = root["wasDecoDive"];

			for (int i=0; i<COMPARTMENT_COUNT; i++) {
				lastDiveData->compartmentPartialPressures[i] = root["compartmentPartialPressures"][i];
			}
		}
	}
	return lastDiveData;
}

void LastDive::storeLastDiveData(LastDiveData* lastDiveData)
{
	SD.remove(LASTDIVE_FILE_NAME);

	SdFile lastDiveFile;
	if (lastDiveFile.open(LASTDIVE_FILE_NAME, O_WRITE | O_CREAT | O_APPEND)) {

		StaticJsonBuffer<JSON_OBJECT_SIZE(8)+JSON_ARRAY_SIZE(16)> jsonBuffer;

		JsonObject& root = jsonBuffer.createObject();
		root.set("timestamp", lastDiveData->diveDateTimestamp);
		root.set("diveDate", lastDiveData->diveDate);
		root.set("diveTime", lastDiveData->diveTime);
		root.set("diveDuration", lastDiveData->durationInSeconds);
		root.set("noFlyTime", lastDiveData->noFlyTimeInMinutes);
		root.set("maxDepth", lastDiveData->maxDepthInMeters, 2);
		root.set("wasDecoDive", lastDiveData->wasDecoDive);

		JsonArray& partialPressures = root.createNestedArray("compartmentPartialPressures");
		for (int i=0; i<COMPARTMENT_COUNT; i++) {
			partialPressures.add(lastDiveData->compartmentPartialPressures[i]);
		}

		root.prettyPrintTo(lastDiveFile);

		lastDiveFile.flush();
		lastDiveFile.close();
	}
}
