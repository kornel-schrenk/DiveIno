#include "TimeUtils.h"

#include <Arduino.h>

const char ZERO = '0';

TimeUtils::TimeUtils() {
}

DateTime* TimeUtils::getCurrentTime() {
	return &rtc.now();
}

String TimeUtils::getCurrentTimeText() {

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
