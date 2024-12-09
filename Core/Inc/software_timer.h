/*
 * software_timer.h
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"
#include "led7seg.h"

extern uint16_t flag_timer2;

void timer_init();

// Function for set timer
void setTimer2(uint16_t duration);
void setFlagModifyTimer(uint16_t timer);
void setBlinkLCDTimer(uint16_t duration);
void setClockTimer(uint16_t duration);

// Function for get flag
uint8_t isFlagModify();
uint8_t isBlinkLCD();
uint8_t isClock();
uint8_t isFlagTimer2();

#endif /* INC_SOFTWARE_TIMER_H_ */
