/*
 * software_timer.c
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#include "software_timer.h"

#define TIMER_CYCLE_2 1
#define TIME_CYCLE 1	// unit: ms

// Attribute for timer 2
uint16_t flag_timer2 = 0;
uint16_t timer2_counter = 0;
uint16_t timer2_MUL = 0;

// Attribute for blink LCD monitor
uint16_t flagModify = 0;
uint16_t timerModify_counter = 0;
uint8_t blinkLCD_flag;
uint16_t blinkLCD_total_tick;
uint16_t blinkLCD_counter;

// Attribute for update clock state
uint8_t clock_flag;
uint16_t clock_total_tick;
uint16_t clock_counter;

void timer_init(){
	HAL_TIM_Base_Start_IT(&htim2);

	blinkLCD_flag = 0;
	blinkLCD_total_tick = 0;
	blinkLCD_counter = 0;
}

// Function for set timer
void setTimer2(uint16_t duration){
	timer2_MUL = duration/TIMER_CYCLE_2;
	timer2_counter = timer2_MUL;
	flag_timer2 = 0;
}

/**
 * @brief Set period for flag blink LCD
 * 
 * @param duration unit: ms
 */
void setBlinkLCDTimer(uint16_t duration)
{
	blinkLCD_total_tick = duration / TIME_CYCLE;
	blinkLCD_counter = blinkLCD_total_tick;
	blinkLCD_flag = 0;
}

/**
 * @brief Set period for flag update clock state
 * 
 * @param duration unit: ms
 */
void setClockTimer(uint16_t duration)
{
	clock_total_tick = duration / TIME_CYCLE;
	clock_counter = clock_total_tick;
	clock_flag = 0;
}

void setFlagModifyTimer(uint16_t timer)
{
	timerModify_counter = timer;
}

// Function for get flag
uint8_t isFlagModify()
{
	if(flagModify == 1){
		flagModify = 0;
		return 1;
	}
	return 0;
}

uint8_t isBlinkLCD()
{
	if (blinkLCD_flag) {
		blinkLCD_flag = 0;
		return 1;
	}
	return 0;
}

uint8_t isClock()
{
	if (clock_flag) {
		clock_flag = 0;
		return 1;
	}
	return 0;
}

uint8_t isFlagTimer2()
{
	if (flag_timer2) {
		flag_timer2 = 0;
		return 1;
	}
	return 0;
}

/**
 * @brief Callback for Timer Interrupt
 * 
 * @param htim 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM2){
		if (timer2_counter > 0){
			timer2_counter--;
			if (timer2_counter == 0) {
				flag_timer2 = 1;
				timer2_counter = timer2_MUL;
			}
		}

		if (timerModify_counter > 0){
			timerModify_counter--;
			if (timerModify_counter == 0) {
				flagModify = 1;
				timerModify_counter = 500;
			}
		}

		if (blinkLCD_counter > 0){
			blinkLCD_counter--;
			if (blinkLCD_counter == 0) {
				blinkLCD_flag = 1;
				blinkLCD_counter = blinkLCD_total_tick;
			}
		}

		if (clock_counter > 0){
			clock_counter--;
			if(clock_counter == 0) {
				clock_flag = 1;
				clock_counter = clock_total_tick;
			}
		}

		led7_Scan();
	}
}

