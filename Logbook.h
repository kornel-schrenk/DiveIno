#ifndef LOGBOOK_H_
#define LOGBOOK_H_

#include "Arduino.h"
#include "SdFat.h"
#include "UTFT.h"

extern SdFat SD;

// Error messages stored in flash.
#define error(msg) SD.errorHalt(F(msg))

typedef struct LogbookData {
	int totalNumberOfDives = 0;
	int totalDiveHours = 0;
	int totalDiveMinutes = 0;
	float totalMaximumDepth = 0.0;
	String lastDiveDateTime;
	int numberOfStoredProfiles = 0;
};

typedef struct ProfileData {
	int diveDuration = 0;
	float maximumDepth = 0.0;
	float minimumTemperature = 0.0;
	float oxigenPercentage = 21;
	String diveDate;
	String diveTime;
	int numberOfProfileItems = 0;
};

class Logbook {
public:
	Logbook();
	LogbookData* loadLogbookData();
	void updateLogbookData(LogbookData* logbookData);
	String getFileNameFromProfileNumber(int profileNumber, bool isTemp);
	bool createNewProfileFile(int profileNumber);
	void storeProfileItem(float pressure, float depth, float temperature, int duration);
	void storeDiveSummary(int profileNumber, unsigned int duration, float maxDepth, float minTemperature, float oxigenPercentage, String date, String time);
	ProfileData* loadProfileDataFromFile(String profileFileName);
    void drawProfileItems(UTFT* tft, int profileNumber, int pageNumber);
private:
    SdFile profileFile;

    float getDepthFromProfileLine(String line);
	int readIntFromLineEnd(String line);
	float readFloatFromLineEnd(String line);
	String readStringFromLineEnd(String line);
};

#endif /* LOGBOOK_H_ */
