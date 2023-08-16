/*
 * timer_hander.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: class for handling communication with real time clock
 */

#include <time_handler.hpp>

#ifndef SRC_TIME_HANDLER_C_
#define SRC_TIME_HANDLER_C_

struct time_struct data_from_RTC;
struct time_struct data_to_RTC;

/**
 * @brief function is constantly called to get fresh time data from rtc - get time every 500ms
 */
void run_time_handler(void){
	if(timeout(time_handler_timer)){
		time_handler_timer = start_timer_ms(TIME_UPDATE_MS);
		read_time_i2c();
		read_date_i2c();

		if(new_data==true){
			new_data = false;
			write_time_i2c();
			write_date_i2c();
		}
	}
}

void get_time(void){
	;
}

/**
 * @brief function to put new data for RTC in write buffer
 * @param value1: seconds in dec - write 0xff to ignore this value
 * @param value2: minutes in dec - write 0xff to ignore this value
 * @param value3: hours in dec  - write 0xff to ignore this value
 */
void set_time(uint8_t value1, uint8_t value2, uint8_t value3){
	new_data = true;

	if((value1!=0xff) && (value1<=59))
		data_to_RTC.seconds = value1;
	else
		data_to_RTC.seconds = data_from_RTC.seconds;

	if((value2!=0xff) && (value2<=59))
		data_to_RTC.minutes = value2;
	else
		data_to_RTC.minutes = data_from_RTC.minutes;

	if((value3!=0xff) && (value3<=23))
		data_to_RTC.hours = value3;
	else
		data_to_RTC.hours = data_from_RTC.hours;
}

void set_date(void){
	new_data = true;
}

/**
 * @brief function to read time information from RTC info RAM
 * store read data in time_struct data_from_RTC
 */
void read_time_i2c(void){
	uint8_t temp[3];
	temp[0] = ADDR_SECONDS;	//start address for read operation

	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS, (uint8_t *)temp, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, DS3231_SLAVE_ADDRESS, (uint8_t *)temp, 3, 10);

	data_from_RTC.seconds = ((temp[0]&0xf0)>>4)*10 + (temp[0]&0x0f);	//converstion from hex to dec
	data_from_RTC.minutes = ((temp[1]&0xf0)>>4)*10 + (temp[1]&0x0f);
	data_from_RTC.hours = ((temp[2]&0x30)>>4)*10 + (temp[2]&0x0f);
}

/**
 * @brief function to write time information from RAM into RTC
 * read data from data_to_RTC and transfer to RTC
 */
void write_time_i2c(void){
	uint8_t temp[4];
	temp[0] = ADDR_SECONDS;	//start address for write operation
	temp[1] = ((data_to_RTC.seconds/10)<<4)|data_to_RTC.seconds%10;
	temp[2] = ((data_to_RTC.minutes/10)<<4)|data_to_RTC.minutes%10;
	temp[3] = ((data_to_RTC.hours/10)<<4)|data_to_RTC.hours%10;

	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS,(uint8_t *)temp, 4, 10);
}

/**
 * @brief function to read date information from RTC info RAM
 * store read date in time_struct data_from_RTC
 */
void read_date_i2c(void){
	uint8_t temp[4];
	temp[0] = ADDR_DAY;	//start address of read operation

	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS, (uint8_t *)temp, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, DS3231_SLAVE_ADDRESS, (uint8_t *)temp, 4, 10);

	data_from_RTC.day = temp[0];
	data_from_RTC.date = ((temp[1]&0x30)>>4)*10 + (temp[1]&0x0f);
	data_from_RTC.month = ((temp[2]&0x10)>>4)*10 + (temp[2]&0x0f);
	data_from_RTC.century = (temp[2] & 0x80) >> 7;	//if century == 1 --> year is bigger that 2000
	data_from_RTC.year = 1900 + (data_from_RTC.century * 100) + ((temp[3]&0xf0)>>4)*10 + (temp[3]%0x0f);
}

/**
 * @brief function to write date information from RAM into RTC
 * read data from data_to_RTC and transfer to RTC
 */
void write_date_i2c(void){
	uint8_t temp[5];
	temp[0] = ADDR_DAY;	//start address for write operation
	temp[1] = data_to_RTC.day;
	temp[2] = ((data_to_RTC.date/10)<<4)|data_to_RTC.date%10;
	temp[3] = ((data_to_RTC.month/10)<<4)|data_to_RTC.month%10 | (data_to_RTC.year/2000)<<7;
	temp[4] = ((data_to_RTC.year %100) / 10)<<4 | data_to_RTC.year%10;

	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS, (uint8_t *)temp, 5, 10);
}

/**
 * @brief write single register
 * @param cmd: register address to which the data should be written
 * @param data: 8bit data which should be written to the register
 */
void write_i2c_single(uint8_t cmd, uint8_t data){
	uint8_t data_t[2];
	data_t[0] = cmd;
	data_t[0] = data;
	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS, (uint8_t *)data_t, 2, 100);
}


/**
 * @brief function to read one byte of data from specified register
 * @param cmd address which content should be read
 * @return data which is read from the specified register
 */
uint8_t read_i2c_single(uint8_t cmd){
	uint8_t data_t[1];
	data_t[0] = cmd;
	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS, (uint8_t *)data_t, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, DS3231_SLAVE_ADDRESS, (uint8_t *)data_t, 1, 10);
	return data_t[0];
}



#endif /* SRC_TIME_HANDLER_C_ */

