#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

#include <Arduino.h>
#include <RTClib.h>

extern RTC_DS1307 rtc;

class TimeUtils {
public:
	TimeUtils();

	String getCurrentDateText();
	String getCurrentTimeText();	
	String getCurrentDateTimeText();
private:

};

#endif /* TIMEUTILS_H_ */