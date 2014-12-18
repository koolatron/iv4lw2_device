#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

#define TICKS_PER_SEC 	250
#define HOUR_MODE_12	0
#define HOUR_MODE_24 	1

typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t ticks;
	uint8_t hour_mode;
} time_t;

void update_time(time_t* time);
void set_hours(time_t* time, uint8_t hours);
void set_minutes(time_t* time, uint8_t minutes);
void set_seconds(time_t* time, uint8_t seconds);
void set_ticks(time_t* time, uint8_t ticks);
void set_hour_mode(time_t* time, uint8_t hour_mode);
uint8_t* time_string(time_t* time, uint8_t* buffer);
time_t* string_time(uint8_t* buffer, time_t* time);

#endif
