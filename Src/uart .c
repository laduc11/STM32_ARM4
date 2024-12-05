/*
 * uart .c
 *
 *  Created on: Nov 26, 2024
 *      Author: DELL
 */

#include "uart.h"

uint8_t rcv_chr = 0;
uint8_t msg[100];
uint8_t process_flag = 0;

/**
 * @brief Initialize UART RS232
 * @return None
 * 
 */
void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &rcv_chr, 1);
	initBuffer();
}

/**
 * @brief Send a string via UART
 * 
 * @param str Pointer to head of string need to be sent
 * @return None
 */
void uart_Rs232SendString(uint8_t* str){
	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
}

/**
 * @brief Send a byte array via UART
 * 
 * @param bytes Byte array which need to be sent
 * @param size Number of bytes
 */
void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

/**
 * @brief Send a number via UART
 * 
 * @param num Number (size: 32 bits)
 */
void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString((uint8_t *)"0");
		return;
	}
    uint8_t num_flag = 0;
    int i;
	if(num < 0) uart_Rs232SendString((uint8_t *)"-");
    for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((void*)msg,"%ld",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString((uint8_t *)"0");
        }
        num %= mypow(10, i-1);
    }
}

/**
 * @brief Send a percentage via UART
 * 
 * @param num 
 */
void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((void*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString((uint8_t *)".");
    sprintf((void*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}

/**
 * @brief UART Callback. This function will be call when MCU receive new message via UART
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		// rs232 isr
		// Push received char to buffer
		if (!pushBuffer(rcv_chr)) {
            uint8_t err_str[] = "Buffer is full";
            uart_Rs232SendString(err_str);
        } else {
        	process_flag = 1;
        }

		// turn on the receice interrupt
		HAL_UART_Receive_IT(&huart1, &rcv_chr, 1);
	}
}
