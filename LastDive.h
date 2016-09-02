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
	int noFlyTimeInMinutes = 0;
	float compartmentPartialPressures[COMPARTMENT_COUNT];
};

class LastDive {
public:
	LastDive();
	LastDiveData* loadLastDiveData();
	void storeLastDiveData(LastDiveData* lastDiveData);
private:
	long readLongFromLineEnd(String line);
	int readIntFromLineEnd(String line);
	float readFloatFromLineEnd(String line);
	String readStringFromLineEnd(String line);
};

#endif /* LASTDIVE_H_ */
