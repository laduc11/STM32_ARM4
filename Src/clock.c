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
uint8_t blink_mode;
uint8_t update_time_mode;

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

// Update time via UART
uint8_t waiting_response;
uint8_t is_update_complete;
uint8_t timeout_update;
uint8_t counter_update;

/**
 * @brief Initialize clock
 * 
 */
void clockInit()
{
	alarmInit();	// Initialize alarm attribute

	// Initialize time
	updateTimeFull(16, 06, 07, 5, 5, 12, 24);

	// Initialize mode
	clock_mode = WATCH_MODE;
	set_time_mode = MODIFY_SEC_STATE;
	alarm_mode = MODIFY_SEC_STATE;
	blink_mode = TURN_OFF;
	update_time_mode = UPDATE_SEC;

	// Initialize temparary variable
	sec_temp = 0;
	min_temp = 0;
	hours_temp = 0;
	date_temp = 0;
	day_temp = 0;
	month_temp = 0;
	year_temp = 0;
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
}

/**
 * @brief Initialize time attribute for setting time mode
 * 
 */
void setTimeInit()
{
	sec_temp = ds3231_sec;
	min_temp = ds3231_min;
	hours_temp = ds3231_hours;
	day_temp = ds3231_day;
	date_temp = ds3231_date;
	month_temp = ds3231_month;
	year_temp = ds3231_year;
}

/**
 * @brief Initialize attributes for updating time via UART
 * 
 */
void Uart4ClockInit()
{
	waiting_response = 0;
	is_update_complete = 0;
	counter_update = MAX_COUNTER;
	timeout_update = 1000 * TIMEOUT / PERIOD;
	update_time_mode = UPDATE_SEC;
	lcd_Clear(BLACK);
	lcd_ShowStr(20, 160, (uint8_t *)"Updating Second...", YELLOW, BLACK, 24, 1);
}

/**
 * @brief Return 1 when update time via UART completely
 * 
 * @return uint8_t 
 */
uint8_t isUpdateComplete()
{
	if (is_update_complete) {
		is_update_complete = 0;
		return 1;
	}
	return 0;
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
void updateTimeFull(uint8_t second, 
					uint8_t minute, 
					uint8_t hours, 
					uint8_t day, 
					uint8_t date, 
					uint8_t month, 
					uint8_t year){
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
	// button_count[14]: B button
	switch (clock_mode)
	{
	case WATCH_MODE:
		// Display time
		ds3231_ReadTime();
		displayTime();
		// check button state
		if (button_count[11] >= BUTTON_COUNT_PRESS) {
			// Change mode
			button_count[11] = 0;
			clock_mode = SET_TIME_MODE;
			setTimeInit();
			lcd_Clear(BLACK);
		} else if (button_count[14] >= BUTTON_COUNT_PRESS) {
			// Change mode to update time via UART
			button_count[14] = 0;
			clock_mode = UPDATE_TIME_VIA_UART;
			Uart4ClockInit();
		} else {
			checkAlarm();
		}
		break;
	case SET_TIME_MODE:
		displayTimeFull(sec_temp, min_temp, hours_temp, day_temp,
						date_temp, month_temp, year_temp);
		if (button_count[11] >= BUTTON_COUNT_PRESS) {
			// Update Time and Change mode
			updateTime();
			button_count[11] = 0;
			clock_mode = ALARM_MODE;
			setTimeInit();
		} else {
			setTimeFSM();
		}
		break;
	case ALARM_MODE:
		displayTimeFull(sec_temp, min_temp, hours_temp, day_temp,
						date_temp, month_temp, year_temp);
		if (button_count[11] >= BUTTON_COUNT_PRESS) {
			// Change mode
			button_count[11] = 0;
			clock_mode = WATCH_MODE;
		} else {
			setAlarmFSM();
		}
		break;
	case UPDATE_TIME_VIA_UART:
		if (button_count[0] >= BUTTON_COUNT_PRESS || isUpdateComplete()) {
			clock_mode = WATCH_MODE;
			button_count[0] = 0;
			lcd_Clear(BLACK);
			updateTime();
			break;
		}
		updateTimeViaUartFSM();
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
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to MINUTE
			ds3231_sec = sec_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_MIN_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase second
				sec_temp = (sec_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_MIN_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to HOURS
			ds3231_min = min_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_HOUR_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase minute
				min_temp = (min_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_HOUR_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to DAY
			ds3231_hours = hours_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DAY_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase minute
				hours_temp = (hours_temp + 1) % 24;
			}
		}
		break;
	case MODIFY_DAY_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to DATE
			ds3231_day = day_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DATE_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase day
				day_temp = (day_temp % 7) + 1;
			}
		}
		break;
	case MODIFY_DATE_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to MONTH
			ds3231_date = date_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_MONTH_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase date
				date_temp = (date_temp % 31) + 1;
			}
		}
		break;
	case MODIFY_MONTH_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to YEAR
			ds3231_month = month_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_YEAR_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase month
				month_temp = (month_temp % 12) + 1;
			}
		}
		break;
	case MODIFY_YEAR_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to SECOND
			ds3231_year = year_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_SEC_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase year
				year_temp = (year_temp % 99) + 1;
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
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to MINUTE
			sec_alarm = sec_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_MIN_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase second
				sec_temp = (sec_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_MIN_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to HOURS
			min_alarm = min_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_HOUR_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase minute
				min_temp = (min_temp + 1) % 60;
			}
		}
		break;
	case MODIFY_HOUR_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to DAY
			hours_alarm = hours_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DAY_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase minute
				hours_temp = (hours_temp + 1) % 24;
			}
		}
		break;
	case MODIFY_DAY_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to DATE
			day_alarm = day_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_DATE_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase day
				day_temp = (day_temp % 7) + 1;
			}
		}
		break;
	case MODIFY_DATE_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to MONTH
			date_alarm = date_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_MONTH_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase date
				date_temp = (date_temp % 31) + 1;
			}
		}
		break;
	case MODIFY_MONTH_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to YEAR
			month_alarm = month_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_YEAR_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase month
				month_temp = (month_temp % 12) + 1;
			}
		}
		break;
	case MODIFY_YEAR_STATE:
		if (button_count[12] >= BUTTON_COUNT_PRESS) {
			// Save and change state to SECOND
			year_alarm = year_temp;
			button_count[12] = 0;
			set_time_mode = MODIFY_SEC_STATE;
		} else {
			if (button_count[3] >= BUTTON_COUNT_PRESS) {
				// Increase year
				year_temp = (year_temp % 99) + 1;
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
	if (clock_mode == SET_TIME_MODE || clock_mode == ALARM_MODE) {
		switch (blink_mode) {
		case TURN_OFF:
			switch (set_time_mode) {
			case MODIFY_SEC_STATE:
				lcd_ShowIntNum(150, 190, sec_temp, 2, BLACK, BLACK, 24);
				break;
			case MODIFY_MIN_STATE:
				lcd_ShowIntNum(100, 190, min_temp, 2, BLACK, BLACK, 24);
				break;
			case MODIFY_HOUR_STATE:
				lcd_ShowIntNum(50, 190, hours_temp, 2, BLACK, BLACK, 24);
				break;
			case MODIFY_DAY_STATE:
				lcd_ShowIntNum(10, 100, day_temp, 2, BLACK, BLACK, 24);
				break;
			case MODIFY_DATE_STATE:
				lcd_ShowIntNum(60, 100, date_temp, 2, BLACK, BLACK, 24);
				break;
			case MODIFY_MONTH_STATE:
				lcd_ShowIntNum(120, 100, month_temp, 2, BLACK, BLACK, 24);
				break;
			case MODIFY_YEAR_STATE:
				lcd_ShowIntNum(180, 100, year_temp, 2, BLACK, BLACK, 24);
				break;
			default:
				break;
			}
			blink_mode = TURN_ON;
			break;
		case TURN_ON:
			switch (set_time_mode) {
			case MODIFY_SEC_STATE:
				lcd_ShowIntNum(150, 190, sec_temp, 2, GREEN, BLACK, 24);
				break;
			case MODIFY_MIN_STATE:
				lcd_ShowIntNum(100, 190, min_temp, 2, GREEN, BLACK, 24);
				break;
			case MODIFY_HOUR_STATE:
				lcd_ShowIntNum(50, 190, hours_temp, 2, GREEN, BLACK, 24);
				break;
			case MODIFY_DAY_STATE:
				lcd_ShowIntNum(10, 100, day_temp, 2, YELLOW, BLACK, 24);
				break;
			case MODIFY_DATE_STATE:
				lcd_ShowIntNum(60, 100, date_temp, 2, YELLOW, BLACK, 24);
				break;
			case MODIFY_MONTH_STATE:
				lcd_ShowIntNum(120, 100, month_temp, 2, YELLOW, BLACK, 24);
				break;
			case MODIFY_YEAR_STATE:
				lcd_ShowIntNum(180, 100, year_temp, 2, YELLOW, BLACK, 24);
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
 * @brief Finite State Machine for Updating Time Via UART 
 * 
 */
void updateTimeViaUartFSM()
{
	if (update_time_mode != UPDATE_FAIL) {
		// Check timeout for update time
		if (counter_update == 0) {
			timeout_update = 1 * 1000 / PERIOD;		// Display error message within 1 sec
			update_time_mode = UPDATE_FAIL;
			lcd_Clear(BLACK);
			return;
		}
		if (timeout_update > 0) {
			timeout_update--;
		} else {
			counter_update--;
			timeout_update = 1000 * TIMEOUT / PERIOD;
		}
	} else {
		// Check timeout for showing error message
		if (timeout_update > 0) {
			timeout_update--;
		} else {
			timeout_update = TIMEOUT * 1000 / PERIOD;
			clock_mode = WATCH_MODE;
		}
	}
	switch (update_time_mode)
	{
	case UPDATE_SEC:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_sec = (uint8_t)atoi((char *)msg) % 60;
					update_time_mode = UPDATE_MIN;
					waiting_response = 0;
					lcd_Clear(BLACK);
					lcd_ShowStr(20, 160, (uint8_t *)"Updating Minute...", YELLOW, BLACK, 24, 1);

					uart_Rs232SendString((uint8_t *)"second = ");
					uart_Rs232SendNum((uint32_t)ds3231_sec);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!second#");
			waiting_response = 1;
		}
		break;
	case UPDATE_MIN:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_min = (uint8_t)atoi((char *)msg) % 60;
					update_time_mode = UPDATE_HOURS;
					waiting_response = 0;
					lcd_Clear(BLACK);
					lcd_ShowStr(20, 160, (uint8_t *)"Updating Hous...", YELLOW, BLACK, 24, 1);

					uart_Rs232SendString((uint8_t *)"minute = ");
					uart_Rs232SendNum((uint32_t)ds3231_min);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!minute#");
			waiting_response = 1;
		}
		break;
	case UPDATE_HOURS:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_hours = (uint8_t)atoi((char *)msg) % 24;
					update_time_mode = UPDATE_DAY;
					waiting_response = 0;
					lcd_Clear(BLACK);
					lcd_ShowStr(20, 160, (uint8_t *)"Updating Day...", YELLOW, BLACK, 24, 1);

					uart_Rs232SendString((uint8_t *)"hours = ");
					uart_Rs232SendNum((uint32_t)ds3231_hours);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!hours#");
			waiting_response = 1;
		}
		break;
	case UPDATE_DAY:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_day = (uint8_t)atoi((char *)msg) % 7;
					if (ds3231_day == 0) {
						ds3231_day = 7;
					}
					update_time_mode = UPDATE_DATE;
					waiting_response = 0;
					lcd_Clear(BLACK);
					lcd_ShowStr(20, 160, (uint8_t *)"Updating Date...", YELLOW, BLACK, 24, 1);

					uart_Rs232SendString((uint8_t *)"day = ");
					uart_Rs232SendNum((uint32_t)ds3231_day);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!day#");
			waiting_response = 1;
		}
		break;
	case UPDATE_DATE:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_date = (uint8_t)atoi((char *)msg) % 31;
					if (ds3231_date == 0) {
						ds3231_date = 31;
					}
					update_time_mode = UPDATE_MONTH;
					waiting_response = 0;
					lcd_Clear(BLACK);
					lcd_ShowStr(20, 160, (uint8_t *)"Updating Month...", YELLOW, BLACK, 24, 1);

					uart_Rs232SendString((uint8_t *)"date = ");
					uart_Rs232SendNum((uint32_t)ds3231_sec);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!date#");
			waiting_response = 1;
		}
		break;
	case UPDATE_MONTH:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_month = (uint8_t)atoi((char *)msg) % 12;
					if (ds3231_month == 0) {
						ds3231_month = 12;
					}
					update_time_mode = UPDATE_YEAR;
					waiting_response = 0;
					lcd_Clear(BLACK);
					lcd_ShowStr(20, 160, (uint8_t *)"Updating Year...", YELLOW, BLACK, 24, 1);

					uart_Rs232SendString((uint8_t *)"month = ");
					uart_Rs232SendNum((uint32_t)ds3231_month);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!month#");
			waiting_response = 1;
		}
		break;
	case UPDATE_YEAR:
		if (waiting_response) {
			if (isMsgStop() == READING_COMPLETE_STATE) {
				uint8_t start = 0;
				uint16_t size = getBufSize();
				uint8_t *buffer = flushBuffer();

				getMsg(buffer, &size, &start);
				if (size > 0) {
					uint8_t *msg = malloc(size * sizeof(uint8_t) + 1);
					for (uint16_t i = 0; i < size; ++i) {
						msg[i] = buffer[start + i];
					}
					msg[size] = '\0';

					// Save second and change state
					ds3231_year = (uint8_t)atoi((char *)msg) % 100;
					lcd_Clear(BLACK);
					is_update_complete = 1;	// Special case when time is updated conpletely

					uart_Rs232SendString((uint8_t *)"year = ");
					uart_Rs232SendNum((uint32_t)ds3231_year);
					uart_Rs232SendString((uint8_t *)"\n");
					free(msg);
				}
			}
		} else {
			uart_Rs232SendString((uint8_t *)"!year#");
			waiting_response = 1;
		}
		break;
	case UPDATE_FAIL:
		lcd_ShowStr(20, 160, (uint8_t *)"Update Time Fail", YELLOW, BLACK, 24, 1);
		break;
	default:
		break;
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
	lcd_ShowIntNum(180, 100, year + 2000, 2, YELLOW, BLACK, 24);

	lcd_ShowStr(40, 160, (uint8_t *)"Gio", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(90, 160, (uint8_t *)"Phut", YELLOW, BLACK, 24, 1);
	lcd_ShowStr(150, 160, (uint8_t *)"Giay", YELLOW, BLACK, 24, 1);

	lcd_ShowIntNum(50, 190, hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(100, 190, minute, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 190, second, 2, GREEN, BLACK, 24);

	lcd_ShowStr(40, 250, (uint8_t *)"Mode: ", BLUE, BLACK, 24, 1);
	lcd_ShowIntNum(100, 250, clock_mode, 2, GREEN, BLACK, 24);
}
