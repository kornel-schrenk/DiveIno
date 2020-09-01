#include "deco/LastDive.h"

void LastDive::_debugLastDiveData(LastDiveData* lastDiveData)
{
    Serial.print("lDate: ");
    Serial.println(lastDiveData->diveDateTimestamp);
    Serial.print("lMaxDepth: ");
    Serial.println(lastDiveData->maxDepthInMeters);
    Serial.print("lDur: ");
    Serial.println(lastDiveData->durationInSeconds);
    Serial.print("lNoFly: ");
    Serial.println(lastDiveData->noFlyTimeInMinutes);
    Serial.print("lDeco: ");
    Serial.println(lastDiveData->wasDecoDive);

    for (int i=0; i<COMPARTMENT_COUNT; i++) {        
        Serial.print("lComp");
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(lastDiveData->compartmentPartialPressures[i]);
    }
}

LastDiveData* LastDive::loadLastDiveData()
{
	LastDiveData* lastDiveData = new LastDiveData;

    Preferences prefs;
    prefs.begin("DiveIno", true);	// read-only        

    lastDiveData->diveDateTimestamp = prefs.getLong("lDate", ez.clock.tz.now());
    lastDiveData->maxDepthInMeters = prefs.getFloat("lMaxDepth", 0.0);
    lastDiveData->durationInSeconds = prefs.getInt("lDur", 0);
    lastDiveData->noFlyTimeInMinutes = prefs.getLong("lNoFly" , 0);
    lastDiveData->wasDecoDive = prefs.getBool("lDeco", false);

    for (int i=0; i<COMPARTMENT_COUNT; i++) {
        lastDiveData->compartmentPartialPressures[i] = prefs.getFloat("lComp" + i, 0.0);
    }

    prefs.end();

    Serial.println("LOAD Last Dive Data");
    this->_debugLastDiveData(lastDiveData);

    return lastDiveData;
}

void LastDive::storeLastDiveData(LastDiveData* lastDiveData)
{
    Preferences prefs;
    prefs.begin("DiveIno");

    prefs.putLong("lDate", lastDiveData->diveDateTimestamp);
    prefs.putFloat("lMaxDepth", lastDiveData->maxDepthInMeters);
    prefs.putInt("lDur", lastDiveData->durationInSeconds);
    prefs.putLong("lNoFly" , lastDiveData->noFlyTimeInMinutes);
    prefs.putBool("lDeco", lastDiveData->wasDecoDive);

    for (int i=0; i<COMPARTMENT_COUNT; i++) {
        prefs.putFloat("lComp" + i, lastDiveData->compartmentPartialPressures[i]);
    }

    prefs.end();

    Serial.println("STORE Last Dive Data");
    this->_debugLastDiveData(lastDiveData);
}

bool LastDive::clearLastDiveData()
{
    Preferences prefs;
    prefs.begin("DiveIno");

    prefs.remove("lDate");
    prefs.remove("lMaxDepth");
    prefs.remove("lDur");
    prefs.remove("lNoFly");
    prefs.remove("lDeco");

    for (int i=0; i<COMPARTMENT_COUNT; i++) {
        prefs.remove("lComp" + i);
    }

    prefs.end();

    Serial.println("CLEAR Last Dive Data");

    return true;
}

long LastDive::getNoFlyTimeInMinutes()
{
    Preferences prefs;
    prefs.begin("DiveIno", true);	// read-only        
    long noFlyTime = prefs.getLong("lNoFly" , 0L);
    prefs.end();

    return noFlyTime;
}

long LastDive::getCurrentSurfaceIntervalInMinutes()
{
    long nowTimestamp = ez.clock.tz.now();

    Preferences prefs;
    prefs.begin("DiveIno", true);	// read-only        
    long diveDateTimestamp = prefs.getLong("lDate", nowTimestamp);
    prefs.end();

	//Calculate surface interval
	return (nowTimestamp - diveDateTimestamp) / 60;
}

DiveResult* LastDive::spendTimeOnSurface(int surfaceIntervalInMinutes, Buhlmann* buhlmann)
{
    DiveResult* diveResult = new DiveResult;
    diveResult = buhlmann->initializeCompartments();

    //Retrieve last dive data
    LastDiveData* lastDiveData = this->loadLastDiveData();

    long nowTimestamp = ez.clock.tz.now();

    //Last dive happened within 48 hours and there is an active no fly time
    if (lastDiveData != NULL &&	(lastDiveData->diveDateTimestamp + 172800) > nowTimestamp && lastDiveData->noFlyTimeInMinutes > 0) {

        Serial.print(F("BEFORE dive No Fly time (min): "));
        Serial.println(lastDiveData->noFlyTimeInMinutes);

        //Copy Last Dive Data
        diveResult->noFlyTimeInMinutes = lastDiveData->noFlyTimeInMinutes;
        diveResult->previousDiveDateTimestamp = lastDiveData->diveDateTimestamp;
        diveResult->wasDecoDive = lastDiveData->wasDecoDive;
        for (byte i=0; i <COMPARTMENT_COUNT; i++) {
            diveResult->compartmentPartialPressures[i] = lastDiveData->compartmentPartialPressures[i];

            Serial.print(F("BEFORE: Dive compartment "));
            Serial.print(i);
            Serial.print(F(": "));
            Serial.print(diveResult->compartmentPartialPressures[i], 0);
            Serial.println(F(" ppN2"));
        }

        //Calculate surface time in minutes
        int surfaceTime = (nowTimestamp - lastDiveData->diveDateTimestamp) / 60;

        Serial.print(F("Surface time (min): "));
        Serial.println(surfaceTime);

        //Spend the time on the surface
        diveResult = buhlmann->surfaceInterval(surfaceTime, diveResult);

        for (byte i=0; i <COMPARTMENT_COUNT; i++) {
            Serial.print(F("AFTER: Dive compartment "));
            Serial.print(i);
            Serial.print(F(": "));
            Serial.print(diveResult->compartmentPartialPressures[i], 0);
            Serial.println(F(" ppN2"));
        }

        Serial.print(F("AFTER dive No Fly time (min): "));
        Serial.println(diveResult->noFlyTimeInMinutes);
    }   
    return diveResult;
}
