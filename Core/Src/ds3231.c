/*
 * ds3231.c
 *
 *  Created on: Dec 4, 2024
 *      Author: DELL
 */

#include "ds3231.h"

#define DS3231_ADDRESS 0x68<<1

uint8_t ds3231_buffer[7];

uint8_t ds3231_hours;
uint8_t ds3231_min;
uint8_t ds3231_sec;
uint8_t ds3231_date;
uint8_t ds3231_day;
uint8_t ds3231_month;
uint8_t ds3231_year;

void ds3231_init(){
	ds3231_buffer[0] = DEC2BCD(07); //second
	ds3231_buffer[1] = DEC2BCD(06); //minute
	ds3231_buffer[2] = DEC2BCD(16); //hour
	ds3231_buffer[3] = DEC2BCD(05); //day
	ds3231_buffer[4] = DEC2BCD(05); //date
	ds3231_buffer[5] = DEC2BCD(12); //month
	ds3231_buffer[6] = DEC2BCD(24); //year
	if(HAL_I2C_IsDeviceReady(&hi2c1, DS3231_ADDRESS, 3, 50) != HAL_OK){
		while(1);
	};
}

void ds3231_Write(uint8_t address, uint8_t value){
	uint8_t temp = DEC2BCD(value);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, &temp, 1,10);
}

void ds3231_ReadTime() {
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, ds3231_buffer, 7, 10);
	ds3231_sec = BCD2DEC(ds3231_buffer[0]);
	ds3231_min = BCD2DEC(ds3231_buffer[1]);
	ds3231_hours = BCD2DEC(ds3231_buffer[2]);
	ds3231_day = BCD2DEC(ds3231_buffer[3]);
	ds3231_date = BCD2DEC(ds3231_buffer[4]);
	ds3231_month = BCD2DEC(ds3231_buffer[5]);
	ds3231_year = BCD2DEC(ds3231_buffer[6]);
}

