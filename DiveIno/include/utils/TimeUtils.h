#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

#include "Arduino.h"
#include "Preferences.h"

#include "M5ez.h"
#include "ezTime.h"
#include "RTClib.h"

extern RTC_DS1307 rtc;

class TimeUtils
{
	public:
		bool setTimeFromRtc();
		bool updateTimeFromNTPServer();
		String setDateTime(int year, int month, int day, int hour, int minute);

	private:
		String _getTimezoneLocation();
		void _storeTimeInRtc();
};

#endif /* TIMEUTILS_H_ */