#ifndef LOGBOOK_H_
#define LOGBOOK_H_

#include "Arduino.h"
#include "SD.h"
#include "UTFT.h"

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
	String getProfileFileName(int profileNumber);
	File createNewProfileFile(int profileNumber);
	void storeProfileItem(File profileFile, float pressure, float depth, float temperature, int duration);
	void storeDiveSummary(int profileNumber, File profileFile, unsigned int duration, float maxDepth, float minTemperature, float oxigenPercentage, String date, String time);
	void printFile(String fileName);
	ProfileData* loadProfileDataFromFile(String profileFileName);
    void drawProfileItems(UTFT* tft, int profileNumber, int pageNumber);
private:
    String getFileNameFromProfileNumber(int profileNumber, bool isTemp);
    float getDepthFromProfileLine(String line);
	int readIntFromLineEnd(String line);
	float readFloatFromLineEnd(String line);
	String readStringFromLineEnd(String line);
};

#endif /* LOGBOOK_H_ */
