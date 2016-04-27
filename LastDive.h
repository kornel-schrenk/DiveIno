#ifndef LASTDIVE_H_
#define LASTDIVE_H_

#include "Arduino.h"
#include "SD.h"

#define COMPARTMENT_COUNT 16

typedef struct LastDiveData {
	long diveDateTimestamp;
	String diveDate;
	float maxDepthInMeters;
	int durationInSeconds;
	int noFlyTimeInMinutes;
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
