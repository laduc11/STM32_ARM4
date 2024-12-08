/*
 * buffer.c
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */


#include "buffer.h"

struct RingBuffer buffer;
uint8_t flush_data[BUFFER_LENGTH];

uint8_t start_char;
uint8_t stop_char;
uint8_t flag_stop_msg;

/**
 * @brief Initialize the ring buffer
 *
 */
void initBuffer() {
    buffer.read_idx = 0;
    buffer.write_idx = 0;
    buffer.size = 0;

    start_char = 0;
    stop_char = 0;
    flag_stop_msg = WAITING_STATE;
}

/**
 * @brief Push an item to ring buffer. If success, return 1. Otherwise return 0
 *
 * @param item
 * @return uint8_t
 */
uint8_t pushBuffer(uint8_t item) {
    if (buffer.size >= BUFFER_LENGTH) {
        // Buffer is full
        return 0;
    }
    if (start_char != 0 && stop_char != 0) {
        if (item == start_char) {
            flag_stop_msg = READING_MESSGAGE_STATE;
        } else if (item == stop_char) {
            flag_stop_msg = READING_COMPLETE_STATE;
        }
    }
    buffer.data[buffer.write_idx] = item;
    buffer.size++;
    buffer.write_idx = (buffer.write_idx + 1) % BUFFER_LENGTH;
    return 1;
}

/**
 * @brief Pop the first item in the buffer. If success, return 1. Otherwise return 0
 *
 * @return uint8_t
 */
uint8_t popBuffer() {
    if (buffer.size == 0) {
        // Buffer is empty
        return 0;
    }
    buffer.read_idx = (buffer.read_idx + 1) % BUFFER_LENGTH;
    buffer.size--;
    return 1;
}

/**
 * @brief Get the pointer to first item in buffer. If buffer is empty, return NULL
 *
 * @return uint8_t*
 */
uint8_t* frontBuffer() {
    if (buffer.size == 0) {
        // Buffer is empty
        return NULL;
    }
    return buffer.data + buffer.read_idx;
}

/**
 * @brief Flush all item buffer contain. If buffer is empty, return NULL
 *
 * @return uint8_t*
 */
uint8_t* flushBuffer() {
    if (buffer.size == 0) {
        // Buffer is empty
        return NULL;
    }
    uint16_t size = buffer.size;
    for (uint16_t i = 0; i < size; ++i) {
        flush_data[i] = *frontBuffer();
        popBuffer();
    }
    if (start_char != 0 && stop_char != 0) {
        flag_stop_msg = WAITING_STATE;
    }
    return flush_data;
}

/**
 * @brief Get the Buffer Size object
 *
 * @return uint16_t
 */
uint16_t getBufSize() {
    return buffer.size;
}

/**
 * @brief Clear buffer
 *
 */
void clearBuffer() {
    initBuffer();
}

/**
 * @brief Set the Start Char object
 * 
 * @param start Character represent for the begin of message
 */
void setStartChar(uint8_t start)
{
    start_char = start;
}

/**
 * @brief Set the Stop Char object
 * 
 * @param stop Character represent for the end of message
 */
void setStopChar(uint8_t stop)
{
    stop_char = stop;
}

/**
 * @brief Check if the message in buffer is received completely
 * 
 * @return uint8_t 
 */
uint8_t isMsgStop()
{
    return flag_stop_msg;
}

/**
 * @brief Preprocess buffer and get the fisrt message in buffer0
 * 
 * @param buffer 
 * @param size 
 * @param start 
 */
void getMsg(uint8_t *buffer, uint16_t *size, uint8_t *start)
{
    while (*start < BUFFER_LENGTH && *size > 0 && *(buffer + *start) != start_char) {
        (*start)++;
        (*size)--;
    }
    // Handle empty buffer
    if (*size == 0) {
        return;
    } 
    (*size)--;
    uint8_t idx = ++(*start), count = 0;
    while (idx < BUFFER_LENGTH && *size > 0 && *(buffer + idx) != stop_char) {
        idx++;
        (*size)--;
        count++;
    }
    if (idx >= BUFFER_LENGTH || *(buffer + idx) != stop_char) {
        *start = 0;
        *size = 0;
    }
    *size = count;
}
