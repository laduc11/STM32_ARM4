/*
 * clock.h
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include "main.h"
#include "button.h"
#include "software_timer.h"
#include "ds3231.h"
#include "lcd.h"

// State of clock
#define WATCH_MODE 0
#define SET_TIME_MODE 1
#define ALARM_MODE 2

// State of set time FSM
#define MODIFY_SEC_STATE 0
#define MODIFY_MIN_STATE 1
#define MODIFY_HOUR_STATE 2
#define MODIFY_DAY_STATE 3
#define MODIFY_DATE_STATE 4
#define MODIFY_MONTH_STATE 5
#define MODIFY_YEAR_STATE 6

// State of blink time in set time mode
#define TURN_OFF 0
#define TURN_ON 1

// General function of clock
void clockInit();
void alarmInit();
void setTimeInit();
void displayTime();
void displayTimeFull(uint8_t second, uint8_t minute, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void updateTime(uint16_t second, uint16_t minute, uint16_t hours, uint16_t day, uint16_t date, uint16_t month, uint16_t year);

// General function of alarm
void checkAlarm();
void notifyAlarm();

// Finite State Machines
void clockFSM();
void setTimeFSM();
void setAlarmFSM();
void blinkTimeFSM();

#endif /* INC_CLOCK_H_ */
