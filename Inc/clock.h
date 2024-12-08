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
#include "buffer.h"
#include "utils.h"
#include "uart.h"

// State of clock
#define WATCH_MODE              0
#define SET_TIME_MODE           1
#define ALARM_MODE              2
#define UPDATE_TIME_VIA_UART    3

// State of set time FSM
#define MODIFY_SEC_STATE    0
#define MODIFY_MIN_STATE    1
#define MODIFY_HOUR_STATE   2
#define MODIFY_DAY_STATE    3
#define MODIFY_DATE_STATE   4
#define MODIFY_MONTH_STATE  5
#define MODIFY_YEAR_STATE   6

// State of blink time in set time mode
#define TURN_OFF    0
#define TURN_ON     1

// State of updating time via UART
#define UPDATE_SEC      0
#define UPDATE_MIN      1
#define UPDATE_HOURS    2
#define UPDATE_DAY      3
#define UPDATE_DATE     4
#define UPDATE_MONTH    5
#define UPDATE_YEAR     6

// General function of clock
void clockInit();
void alarmInit();
void setTimeInit();
void Uart4ClockInit();

void displayTime();
void updateTime();
void displayTimeFull(uint8_t second, uint8_t minute, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void updateTimeFull(uint8_t second, uint8_t minute, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year);

// General function of alarm
void checkAlarm();
void notifyAlarm();

// General function of update time via UART
uint8_t isUpdateComplete();

// Finite State Machines
void clockFSM();
void setTimeFSM();
void setAlarmFSM();
void blinkTimeFSM();
void updateTimeViaUartFSM();

#endif /* INC_CLOCK_H_ */
