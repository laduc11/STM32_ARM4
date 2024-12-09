/*
 * button.h
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

#define BUTTON_COUNT_PRESS 3

extern uint16_t button_count[16];

void button_init();
void button_Scan();

#endif /* INC_BUTTON_H_ */
