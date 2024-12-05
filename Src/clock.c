/*
 * clock.c
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#include "clock.h"

// State
uint8_t clock_mode;
uint8_t set_time_mode;
uint8_t alarm_mode;
uint8_t blink_mode; // mode = 0 (off); mode = 1 (on)

// Alarm attribte
uint8_t sec_alarm;
uint8_t min_alarm;
uint8_t hour_alarm;
uint8_t date_alarm;
uint8_t day_alarm;
uint8_t month_alarm;
uint8_t year_alarm;

// Time attribute temporary
uint8_t sec_temp;
uint8_t min_temp;
uint8_t hour_temp;
uint8_t date_temp;
uint8_t day_temp;
uint8_t month_temp;
uint8_t year_temp;

/**
 * @brief Initialize clock
 * 
 */
void clockInit()
{
	alarmInit();	// Initialize alarm attribute
	
	// Initialize mode
	clock_mode = WATCH_MODE;
	set_time_mode = MODIFY_SEC_STATE;
	alarm_mode = 0;		// Uncomplete initialize
	blink_mode = TURN_OFF;
}

/**
 * @brief Initialize attribute of alarm
 * 
 */
void alarmInit() 
{
	sec_alarm = 0;
	min_alarm = 0;
	hour_alarm = 0;
	day_alarm = 0;
	date_alarm = 0;
	month_alarm = 0;
	year_alarm = 0;
}

/**
 * @brief Initialize time attribute for setting time mode
 * 
 */
void setTimeInit()
{
	sec_alarm = ds3231_sec;
	min_alarm = ds3231_min;
	hour_alarm = ds3231_hours;
	day_alarm = ds3231_day;
	date_alarm = ds3231_date;
	month_alarm = ds3231_month;
	year_alarm = ds3231_year;
}

/**
 * @brief Update time to IC RTC DS3231
 * 
 * @param second 
 * @param minute 
 * @param hours 
 * @param day 
 * @param date 
 * @param month 
 * @param year 
 */
void updateTime(uint16_t second, 
				uint16_t minute, 
				uint16_t hours, 
				uint16_t day, 
				uint16_t date, 
				uint16_t month, 
				uint16_t year){
	ds3231_Write(ADDRESS_YEAR, year);
	ds3231_Write(ADDRESS_MONTH, month);
	ds3231_Write(ADDRESS_DATE, date);
	ds3231_Write(ADDRESS_DAY, day);
	ds3231_Write(ADDRESS_HOUR, hours);
	ds3231_Write(ADDRESS_MIN, minute);
	ds3231_Write(ADDRESS_SEC, second);
}

/**
 * @brief Finite state machine for clock
 *
 */
void clockFSM()
{
	// button_count[11]: ringt arrow
	// button_count[3]: up arrow
	// button_count[12]: E button
	switch (clock_mode)
	{
	case WATCH_MODE:
		if (button_count[11] == BUTTON_COUNT_PRESS) { // reduce noise
			// Change mode
			button_count[11] = 0;
			clock_mode = SET_TIME_MODE;
			displayTime();
			setFlagModifyTimer(250);
			break;
		}
		ds3231_ReadTime();
		displayTime();
		checkAlart();
		break;
	case SET_TIME_MODE:
		if (button_count[11] == BUTTON_COUNT_PRESS) {
			// Change mode
			button_count[11] = 0;
			clock_mode = ALARM_MODE;
			displayTime();
		}
/*--------------------------------------------------------------------*/
		// if (button_count[12] == 1) {
		// 	// Change updated field
		// 	button_count[12] = 0;
		// 	modifyStatus = (modifyStatus + 1) % 7;
		// }
		// if (modifyStatus == 0) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offSecond();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_sec++;
		// 		checkTime();
		// 	}
		// }
		// else if (modifyStatus == 1) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offMinute();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_min++;
		// 		checkTime();
		// 	}
		// }
		// else if (modifyStatus == 2) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offHour();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_hours++;
		// 		checkTime();
		// 	}
		// }
		// else if (modifyStatus == 3) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offArticle();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_day++;
		// 		checkTime();
		// 	}
		// }
		// else if (modifyStatus == 4) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offDay();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_date++;
		// 		checkTime();
		// 	}
		// }
		// else if (modifyStatus == 5) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offMonth();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_month++;
		// 		checkTime();
		// 	}
		// }
		// else if (modifyStatus == 6) {
		// 	if (isFlagModify() == 1) {
		// 		toggle = 1 - toggle;
		// 		if (toggle) {
		// 			displayTime();
		// 		}
		// 		else {
		// 			offYear();
		// 		}
		// 	}
		// 	if (button_count[3] == 1) {
		// 		button_count[3] = 0;
		// 		ds3231_year++;
		// 		checkTime();
		// 	}
		// }
/*--------------------------------------------------------------------*/
		break;
	case ALARM_MODE:
		if (button_count[11] == BUTTON_COUNT_PRESS) {
			// Change mode
			button_count[11] = 0;
			clock_mode = WATCH_MODE;
			lcd_Clear(BLACK);
			// displayAlartTime(modeStatus);
		}
		setAlarmFSM();
		break;
	default:
		break;
	}
}

/**
 * @brief Finite state machine for setting time
 * 
 */
void setTimeFSM()
{
	switch (set_time_mode)
	{
	case MODIFY_SEC_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Reset button count and change state
			button_count[12] = 0;
			set_time_mode = MODIFY_MIN_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase second
				sec_temp = (sec_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_MIN_STATE:
		break;
	case MODIFY_HOUR_STATE:
		break;
	case MODIFY_DAY_STATE:
		break;
	case MODIFY_DATE_STATE:
		break;
	case MODIFY_MONTH_STATE:
		break;
	case MODIFY_YEAR_STATE:
		break;
	
	default:
		break;
	}
}

/**
 * @brief Finite State Machine for blinking time in setting time mode
 * 
 */
void blinkTimeFSM()
{
	if (clock_mode == SET_TIME_MODE) {
		switch (blink_mode) {
			case TURN_OFF:
				switch (set_time_mode) {
				case MODIFY_SEC_STATE:
					lcd_ShowIntNum(150, 190, ds3231_sec, 2, BLACK, BLACK, 24);
					break;
				case MODIFY_MIN_STATE:
					lcd_ShowIntNum(100, 190, ds3231_min, 2, BLACK, BLACK, 24);
					break;
				case MODIFY_HOUR_STATE:
					lcd_ShowIntNum(50, 190, ds3231_hours, 2, BLACK, BLACK, 24);
					break;
				case MODIFY_DAY_STATE:
					lcd_ShowIntNum(10, 100, ds3231_day, 2, BLACK, BLACK, 24);
					break;
				case MODIFY_DATE_STATE:
					lcd_ShowIntNum(60, 100, ds3231_date, 2, BLACK, BLACK, 24);
					break;
				case MODIFY_MONTH_STATE:
					lcd_ShowIntNum(120, 100, ds3231_month, 2, BLACK, BLACK, 24);
					break;
				case MODIFY_YEAR_STATE:
					lcd_ShowIntNum(180, 100, ds3231_year, 2, BLACK, BLACK, 24);
					break;
				default:
					break;
				}
				blink_mode = TURN_ON;
				break;
			case TURN_ON:
				switch (set_time_mode) {
				case MODIFY_SEC_STATE:
					lcd_ShowIntNum(150, 190, ds3231_sec, 2, YELLOW, BLACK, 24);
					break;
				case MODIFY_MIN_STATE:
					lcd_ShowIntNum(100, 190, ds3231_min, 2, YELLOW, BLACK, 24);
					break;
				case MODIFY_HOUR_STATE:
					lcd_ShowIntNum(50, 190, ds3231_hours, 2, YELLOW, BLACK, 24);
					break;
				case MODIFY_DAY_STATE:
					lcd_ShowIntNum(10, 100, ds3231_day, 2, YELLOW, BLACK, 24);
					break;
				case MODIFY_DATE_STATE:
					lcd_ShowIntNum(60, 100, ds3231_date, 2, YELLOW, BLACK, 24);
					break;
				case MODIFY_MONTH_STATE:
					lcd_ShowIntNum(120, 100, ds3231_month, 2, YELLOW, BLACK, 24);
					break;
				case MODIFY_YEAR_STATE:
					lcd_ShowIntNum(180, 100, ds3231_year, 2, YELLOW, BLACK, 24);
					break;
				default:
					break;
				}
				blink_mode = TURN_OFF;
				break;
			
			default:
				break;
		}
			
	} 
}

/**
 * @brief Compare the current time to alarm
 * 
 */
void checkAlarm() 
{
	if (ds3231_sec == sec_alarm &&
		ds3231_min == min_alarm &&
		ds3231_hours == hour_alarm &&
		ds3231_day == day_alarm &&
		ds3231_date == date_alarm &&
		ds3231_month == month_alarm &&
		ds3231_year == year_alarm) {
		notifyAlarm();
	}
}

/**
 * @brief Display alarm to LCD	
 * 
 */
void notifyAlarm() {
	lcd_DrawCircle(120, 160, RED, 50, 1);
	lcd_ShowStr(100, 160, "ALART", BLACK, RED, 24, 1);
}

/**
 * @brief Display time to LCD with default parameter
 * 
 */
void displayTime() {
	displayTimeFull(ds3231_sec, ds3231_min, ds3231_hours, ds3231_day, ds3231_date, ds3231_month, ds3231_year);
}

/**
 * @brief Display time to LCD in the full form
 * 
 */
void displayTimeFull(uint8_t second, 
					 uint8_t minute, 
					 uint8_t hours, 
					 uint8_t day, 
					 uint8_t date, 
					 uint8_t month, 
					 uint8_t year) {
	lcd_ShowStr(10, 70, "Thu", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(55, 70, "Ngay", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(110, 70, "Thang", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(180, 70, "Nam", YELLOW, BLACK, 24, 1);

	lcd_ShowIntNum(10, 100, day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(60, 100, date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(120, 100, month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(180, 100, year, 2, YELLOW, BLACK, 24);

	lcd_ShowStr(40, 160, "Gio", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(90, 160, "Phut", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(150, 160, "Giay", YELLOW, BLACK, 24, 1);

	lcd_ShowIntNum(50, 190, hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(100, 190, minute, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 190, second, 2, GREEN, BLACK, 24);

	lcd_ShowStr(40, 250, "Mode: ", BLUE, BLACK, 24, 1);
	lcd_ShowIntNum(100, 250, clock_mode, 2, GREEN, BLACK, 24);
}
