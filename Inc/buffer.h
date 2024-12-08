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

#define WAITING_STATE 			0
#define READING_MESSGAGE_STATE 	1
#define READING_COMPLETE_STATE	2

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

void setStartChar(uint8_t start);
void setStopChar(uint8_t stop);

uint8_t isMsgStop();
void getMsg(uint8_t *buffer, uint16_t *size, uint8_t *start);

#endif /* INC_BUFFER_H_ */
