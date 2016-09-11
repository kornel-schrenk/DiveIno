#ifndef LASTDIVE_H_
#define LASTDIVE_H_

#include "Arduino.h"
#include "SdFat.h"

extern SdFat SD;

#define COMPARTMENT_COUNT 16

typedef struct LastDiveData {
	long diveDateTimestamp = 0;
	String diveDate;
	float maxDepthInMeters = 0.0;
	int durationInSeconds = 0;
	long noFlyTimeInMinutes = 0;
	bool wasDecoDive = false;
	float compartmentPartialPressures[COMPARTMENT_COUNT];
};

class LastDive {
public:
	LastDive();
	LastDiveData* loadLastDiveData();
	void storeLastDiveData(LastDiveData* lastDiveData);
private:
	bool readBoolFromLineEnd(String line);
	unsigned long readLongFromLineEnd(String line);
	int readIntFromLineEnd(String line);
	float readFloatFromLineEnd(String line);
	String readStringFromLineEnd(String line);
	String readStringUntil(SdFile* file, char terminator);
};

#endif /* LASTDIVE_H_ */
