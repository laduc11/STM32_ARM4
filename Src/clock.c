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
uint8_t hours_alarm;
uint8_t date_alarm;
uint8_t day_alarm;
uint8_t month_alarm;
uint8_t year_alarm;

// Time attribute temporary
uint8_t sec_temp;
uint8_t min_temp;
uint8_t hours_temp;
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
	hours_alarm = 0;
	day_alarm = 0;
	date_alarm = 0;
	month_alarm = 0;
	year_alarm = 0;

	alarm_mode = MODIFY_SEC_STATE;
}

/**
 * @brief Initialize time attribute for setting time mode
 * 
 */
void setTimeInit()
{
	sec_alarm = ds3231_sec;
	min_alarm = ds3231_min;
	hours_alarm = ds3231_hours;
	day_alarm = ds3231_day;
	date_alarm = ds3231_date;
	month_alarm = ds3231_month;
	year_alarm = ds3231_year;

	set_time_mode = MODIFY_SEC_STATE;
}

/**
 * @brief Update time to IC RTC DS3231 (reduce form)
 * 
 */
void updateTime()
{
	updateTimeFull(ds3231_sec, ds3231_min, ds3231_hours, ds3231_day,
				   ds3231_date, ds3231_month, ds3231_year);
}
/**
 * @brief Update time to IC RTC DS3231 (full form)
 * 
 * @param second 
 * @param minute 
 * @param hours 
 * @param day 
 * @param date 
 * @param month 
 * @param year 
 */
void updateTimeFull(uint16_t second, 
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
		// Display time
		ds3231_ReadTime();
		displayTime();
		// check button state
		if (button_count[11] == BUTTON_COUNT_PRESS) {
			// Change mode
			button_count[11] = 0;
			clock_mode = SET_TIME_MODE;
			setTimeInit();
		} else {
			checkAlarm();
		}
		break;
	case SET_TIME_MODE:
		displayTimeFull(sec_temp, min_temp, hours_temp, day_temp,
						date_temp, month_temp, year_temp);
		if (button_count[11] == BUTTON_COUNT_PRESS) {
			// Update Time and Change mode
			updateTime();
			button_count[11] = 0;
			clock_mode = ALARM_MODE;
		} else {
			setTimeFSM();
		}
		break;
	case ALARM_MODE:
		displayTimeFull(sec_temp, min_temp, hours_temp, day_temp,
						date_temp, month_temp, year_temp);
		if (button_count[11] == BUTTON_COUNT_PRESS) {
			// Change mode
			button_count[11] = 0;
			clock_mode = WATCH_MODE;
		} else {
			setAlarmFSM();
		}
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
			// Save and change state to MINUTE
			ds3231_sec = sec_temp;
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
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to HOURS
			ds3231_min = min_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_HOUR_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase minute
				min_temp = (min_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_HOUR_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to DAY
			ds3231_hours = hours_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DAY_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase minute
				hours_temp = (hours_temp + 1) % 24;
			}
		}
		break;
	case MODIFY_DAY_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to DATE
			ds3231_day = day_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DATE_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase day
				day_temp = (day_temp % 7) + 1;
			}
		}
		break;
	case MODIFY_DATE_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to MONTH
			ds3231_date = date_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_MONTH_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase date
				date_temp = (date_temp % 31) + 1;
			}
		}
		break;
	case MODIFY_MONTH_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to YEAR
			ds3231_month = month_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_YEAR_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase month
				month_temp = (month_temp % 12) + 1;
			}
		}
		break;
	case MODIFY_YEAR_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to SECOND
			ds3231_year = year_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_SEC_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase year
				year_temp = (year_temp % 12) + 1;
			}
		}
		break;
	
	default:
		break;
	}
}

/**
 * @brief Finite state machine for setting alarm
 * 
 */
void setAlarmFSM()
{
	switch (alarm_mode)
	{
	case MODIFY_SEC_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to MINUTE
			sec_alarm = sec_temp;
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
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to HOURS
			min_alarm = min_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_HOUR_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase minute
				min_temp = (min_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_HOUR_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to DAY
			hours_alarm = hours_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DAY_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase minute
				hours_temp = (hours_temp + 1) % 24;
			}
		}
		break;
	case MODIFY_DAY_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to DATE
			day_alarm = day_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DATE_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase day
				day_temp = (day_temp % 7) + 1;
			}
		}
		break;
	case MODIFY_DATE_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to MONTH
			date_alarm = date_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_MONTH_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase date
				date_temp = (date_temp % 31) + 1;
			}
		}
		break;
	case MODIFY_MONTH_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to YEAR
			month_alarm = month_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_YEAR_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase month
				month_temp = (month_temp % 12) + 1;
			}
		}
		break;
	case MODIFY_YEAR_STATE:
		if (button_count[12] == BUTTON_COUNT_PRESS) {
			// Save and change state to SECOND
			year_alarm = year_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_SEC_STATE;
		} else {
			if (button_count[3] == BUTTON_COUNT_PRESS) {
				// Increase year
				year_temp = (year_temp % 12) + 1;
			}
		}
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
		ds3231_hours == hours_alarm &&
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
	lcd_ShowStr(100, 160, (uint8_t *)"ALART", BLACK, RED, 24, 1);
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
	lcd_ShowStr(10, 70, (uint8_t *)"Thu", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(55, 70, (uint8_t *)"Ngay", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(110, 70, (uint8_t *)"Thang", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(180, 70, (uint8_t *)"Nam", YELLOW, BLACK, 24, 1);

	lcd_ShowIntNum(10, 100, day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(60, 100, date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(120, 100, month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(180, 100, year, 2, YELLOW, BLACK, 24);

	lcd_ShowStr(40, 160, (uint8_t *)"Gio", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(90, 160, (uint8_t *)"Phut", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(150, 160, (uint8_t *)"Giay", YELLOW, BLACK, 24, 1);

	lcd_ShowIntNum(50, 190, hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(100, 190, minute, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 190, second, 2, GREEN, BLACK, 24);

	lcd_ShowStr(40, 250, (uint8_t *)"Mode: ", BLUE, BLACK, 24, 1);
	lcd_ShowIntNum(100, 250, clock_mode, 2, GREEN, BLACK, 24);
}
