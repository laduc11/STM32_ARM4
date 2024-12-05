/*
 * buffer.c
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */


#include "buffer.h"

struct RingBuffer buffer;
uint8_t flush_data[BUFFER_LENGTH];

/**
 * @brief Initialize the ring buffer
 *
 */
void initBuffer() {
    buffer.read_idx = 0;
    buffer.write_idx = 0;
    buffer.size = 0;
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
