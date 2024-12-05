/*
 * software_timer.c
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#include "software_timer.h"

#define TIMER_CYCLE_2 1


uint16_t flag_timer2 = 0;
uint16_t timer2_counter = 0;
uint16_t timer2_MUL = 0;

// LAB 4
uint16_t flagModify = 0;
uint16_t timerModify_counter = 0;

void timer_init(){
	HAL_TIM_Base_Start_IT(&htim2);
}

void setTimer2(uint16_t duration){
	timer2_MUL = duration/TIMER_CYCLE_2;
	timer2_counter = timer2_MUL;
	flag_timer2 = 0;
}

uint16_t isFlagModify()
{
	if(flagModify == 1){
		flagModify = 0;
		return 1;
	}
	return 0;
}

void setFlagModifyTimer(uint16_t timer)
{
	timerModify_counter = timer;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		if(timer2_counter > 0){
			timer2_counter--;
			if(timer2_counter == 0) {
				flag_timer2 = 1;
				timer2_counter = timer2_MUL;
			}
		}

		if(timerModify_counter > 0){
			timerModify_counter--;
			if(timerModify_counter == 0) {
				flagModify = 1;
				timerModify_counter = 500;
			}
		}

		led7_Scan();
	}
}

