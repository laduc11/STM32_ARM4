/*
 * software_timer.h
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"

extern uint16_t flag_timer2;

void timer_init();
void setTimer2(uint16_t duration);
void setFlagModifyTimer(uint16_t timer);
uint16_t isFlagModify();

#endif /* INC_SOFTWARE_TIMER_H_ */
