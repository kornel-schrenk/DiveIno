#ifndef LastDive_h
#define LastDive_h

#include "Arduino.h"
#include "Preferences.h"
#include "M5ez.h"
#include "ezTime.h"

#include "deco/Buhlmann.h"

#define COMPARTMENT_COUNT 16

struct LastDiveData {
	long diveDateTimestamp = 0;
	float maxDepthInMeters = 0.0;
	int durationInSeconds = 0;
	long noFlyTimeInMinutes = 0;
	bool wasDecoDive = false;
	float compartmentPartialPressures[COMPARTMENT_COUNT];
};

class LastDive {
public:
	LastDiveData* loadLastDiveData();
	void storeLastDiveData(LastDiveData* lastDiveData);
	bool clearLastDiveData();

    long getNoFlyTimeInMinutes();
    long getCurrentSurfaceIntervalInMinutes();
    DiveResult* spendTimeOnSurface(int surfaceIntervalInMinutes, Buhlmann* buhlmann);
private:
    void _debugLastDiveData(LastDiveData* lastDiveData);
};

#endif