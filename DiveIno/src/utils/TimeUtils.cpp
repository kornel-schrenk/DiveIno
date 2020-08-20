#include "utils/TimeUtils.h"

bool TimeUtils::setTimeFromRtc()
{
	//Disable automatic updates on the NTP server
	//The update needs several seconds to execute, which makes the seconds counter freeze until the update
	setInterval(0);

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

String TimeUtils::_getTimezoneLocation()
{
	Preferences prefs;
	prefs.begin("M5ez", true); // read-only
	String savedTimezone = prefs.getString("timezone", "GeoIP");
	prefs.end();
	return savedTimezone;
}

void TimeUtils::_storeTimeInRtc()
{
	//Update the RTC based time
	DateTime rtcDateTime = DateTime(ez.clock.tz.now());

	RTC_DS1307 rtc;
	rtc.adjust(rtcDateTime);

	Serial.println(F("RTC time was set as: "));
	Serial.print(rtcDateTime.year(), DEC);
	Serial.print('-');
	Serial.print(rtcDateTime.month(), DEC);
	Serial.print('-');
	Serial.print(rtcDateTime.day(), DEC);
	Serial.print(' ');
	Serial.print(rtcDateTime.hour(), DEC);
	Serial.print(':');
	Serial.print(rtcDateTime.minute(), DEC);
	Serial.print(':');
	Serial.print(rtcDateTime.second(), DEC);
	Serial.println();
}

bool TimeUtils::updateTimeFromNTPServer()
{
	updateNTP();
	if (timeStatus() == timeSet)
	{
		//Update timezone based on Preferences
		String storedTimezone = _getTimezoneLocation();
		Serial.println("Stored timezone: " + storedTimezone);
		ez.clock.tz.setLocation(storedTimezone);
		Serial.println("New timezone was set to " + storedTimezone);

		this->_storeTimeInRtc();

		return true;
	}
	return false;
}

String TimeUtils::setDateTime(int year, int month, int day, int hour, int minute)
{
	DateTime dateTime = DateTime(year, month, day, hour, minute);

	//Set time in ezTime
	setTime(dateTime.unixtime());
	//Set time in RTC
	this->_storeTimeInRtc();

	//Return with the current time
	return ez.clock.tz.dateTime("Y-m-d H:i:s");
}
