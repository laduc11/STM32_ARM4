/*
 * buffer.h
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

#include <stdio.h>

#define BUFFER_LENGTH 100

struct RingBuffer {
	uint8_t data[BUFFER_LENGTH];
	uint8_t read_idx, write_idx;
	uint16_t size;
};

void initBuffer();

uint8_t pushBuffer(uint8_t item);
uint8_t popBuffer();

uint8_t* frontBuffer();
uint8_t* flushBuffer();

uint16_t getBufSize();
void clearBuffer();

#endif /* INC_BUFFER_H_ */
