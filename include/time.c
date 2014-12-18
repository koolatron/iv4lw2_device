#include "time.h"

void update_time(time_t* time) {
	time->ticks++;

	if(time->ticks >= TICKS_PER_SEC) {
		time->seconds++;
		time->ticks = 0;
	}

	if(time->seconds >= 60) {
		time->minutes++;
		time->seconds = 0;
	}

	if(time->minutes >= 60) {
		time->hours++;
		time->minutes = 0;
	}

	if(time->hour_mode == HOUR_MODE_12) {
		if (time->hours >= 13) {
			time->hours = 1;
		}
	}

	if(time->hour_mode == HOUR_MODE_24) {
		if (time->hours >= 24) {
			time->hours = 0;
		}
	}
}

void set_hours(time_t* time, uint8_t hours) {
	time->hours = hours;
}

void set_minutes(time_t* time, uint8_t minutes) {
	time->minutes = minutes;
}

void set_seconds(time_t* time, uint8_t seconds) {
	time->seconds = seconds;
}

void set_ticks(time_t* time, uint8_t ticks) {
	time->ticks = ticks;
}

void set_hour_mode(time_t* time, uint8_t hour_mode) {
	time->hour_mode = hour_mode;
}

/** Given a time_t object, load a buffer with that time's ascii representation.
  */
uint8_t* time_string(time_t* time, uint8_t* buffer) {
	uint8_t hours = time->hours;
	uint8_t minutes = time->minutes;

	buffer[3] = (minutes % 10) + 48;
	buffer[2] = ((minutes - (minutes % 10)) / 10) + 48;
	buffer[1] = (hours % 10) + 48;
	buffer[0] = ((hours - (hours % 10)) / 10) + 48;

	return buffer;
}

/** Given a four-character ascii buffer, translate that into a time and set the
  * supplied time_t with it.
  */
time_t* string_time(uint8_t* buffer, time_t* time) {
	time->minutes = ((buffer[3] - 48) + ((buffer[2] - 48) * 10));
	time->hours = ((buffer[1] - 48) + ((buffer[0] - 48) * 10));
	time->seconds = 0;
	time->ticks = 0;

	return time;
}
