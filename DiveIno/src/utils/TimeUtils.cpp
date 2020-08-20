#include "utils/TimeUtils.h"

const char ZERO = '0';

TimeUtils::TimeUtils() {
}

bool TimeUtils::setTimeFromRtc()
{
  if (timeStatus() == timeNotSet || timeStatus() == timeNeedsSync)
  {
    Serial.println("Network time is unavailable");

    RTC_DS1307 rtc;
    //Real Time Clock (RTC) initialization
    if (rtc.begin())
    {
      unsigned long timerTimestamp = rtc.now().unixtime();
      Serial.println(F("RTC time available: "));

      DateTime now = rtc.now();
      Serial.print(now.year(), DEC);
      Serial.print('-');
      Serial.print(now.month(), DEC);
      Serial.print('-');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();

      setTime(timerTimestamp);
      Serial.println("RTC based time was set.");
      return true;
    }
    else
    {
      Serial.println(F("RTC is NOT available."));
      // No clock will be displayed - update has to be done manually with the Update button
    }
  }
  return false;
}

String TimeUtils::getCurrentDateText() {
	String time = "";
	DateTime now = rtc.now();

	time+=now.year();
	time+="-";
	if (now.month() <10) {
		time+=ZERO;
	}
	time+=now.month();
	time+="-";
	if (now.day()<10) {
		time+=ZERO;
	}
	time+=now.day();

	return time;
}

String TimeUtils::getCurrentTimeText() {

	String time = "";
	DateTime now = rtc.now();

	if (now.hour()<10) {
		time+=ZERO;
	}
	time+=now.hour();
	time+=":";
	if (now.minute()<10) {
		time+=ZERO;
	}
	time+=now.minute();
	time+=":";
	if (now.second()<10) {
		time+=ZERO;
	}
	time+=now.second();

	return time;
}

String TimeUtils::getCurrentDateTimeText() {

	String time = "";
	DateTime now = rtc.now();

	time+=now.year();
	time+="-";
	if (now.month() <10) {
		time+=ZERO;
	}
	time+=now.month();
	time+="-";
	if (now.day()<10) {
		time+=ZERO;
	}
	time+=now.day();
	time+=" ";

	if (now.hour()<10) {
		time+=ZERO;
	}
	time+=now.hour();
	time+=":";
	if (now.minute()<10) {
		time+=ZERO;
	}
	time+=now.minute();
	time+=":";
	if (now.second()<10) {
		time+=ZERO;
	}
	time+=now.second();

	return time;
}

