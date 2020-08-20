#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"
#include "RTClib.h"

extern RTC_DS1307 rtc;

class TimeUtils {
public:
	TimeUtils();

	bool setTimeFromRtc();

	String getCurrentDateText();
	String getCurrentTimeText();	
	String getCurrentDateTimeText();
private:

};

#endif /* TIMEUTILS_H_ */