/*
 * uart.h
 *
 *  Created on: Nov 26, 2024
 *      Author: DELL
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "buffer.h"
#include "lcd.h"

void uart_init_rs232();
void uart_Rs232SendString(uint8_t* str);
void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size);
void uart_Rs232SendNum(uint32_t num);
void uart_Rs232SendNumPercent(uint32_t num);

#endif /* INC_UART_H_ */
