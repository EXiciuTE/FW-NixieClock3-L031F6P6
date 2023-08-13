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

//time_struct data_from_RTC;	//data read from RTC gets here
//time_struct data_to_RTC;	//data that have to be written on the RTC goes here

/**
 * @brief function is constantly called to get fresh time data from rtc - get time every 500ms
 */
void run_time_handler(void){
	if(timeout(time_handler_timer)){
		time_handler_timer = start_timer_ms(TIME_UPDATE_MS);

		//get fresh data every 500ms
		//	read_i2c_single(0x00);
	}
}

//void get_time(void){
//	;
//}
//
//void set_time(void){
//	;
//}
//
//void get_date(void){
//	;
//}
//
//void set_date(void){
//	;
//}
//
//void read_time_i2c(void){
//	;
//}
//
//void read_date_i2c(void){
//	;
//}
//
//void write_time_i2c(void){
//	;
//}
//
//void write_date_i2c(void){
//	;
//}
//
///**
// * @brief write single register
// * @param cmd: register address to which the data should be written
// * @param data: 8bit data which should be written to the register
// */
//void write_i2c_single(uint8_t cmd, uint8_t data){
//	uint8_t data_t[2];
//	data_t[0] = cmd;
//	data_t[0] = data;
//	HAL_I2C_Master_Transmit(&hi2c1, DS3121_MASTER_ADDRESS, (uint8_t *)data_t, 2, 100);
//}
//
//
///**
// * @brief function to read one byte of data from specified register
// * @param cmd address which content should be read
// * @return data which is read from the specified register
// */
//uint8_t read_i2c_single(uint8_t cmd){
//	uint8_t data_t[1];
//	data_t[0] = cmd;
//	HAL_I2C_Master_Transmit(&hi2c1, DS3121_MASTER_ADDRESS, (uint8_t *)data_t, 1, 10);
//	HAL_I2C_Master_Receive(&hi2c1, DS3121_SLAVE_ADDRESS, (uint8_t *)data_t, 1, 10);
//	return data_t[0];
//}



#endif /* SRC_TIME_HANDLER_C_ */

