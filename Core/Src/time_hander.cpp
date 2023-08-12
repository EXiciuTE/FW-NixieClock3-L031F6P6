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

void write_i2c(uint8_t cmd, uint8_t value){
	uint8_t data[2] = {0};
	HAL_I2C_Master_Transmit(&hi2c1, DS3121_MASTER_ADDRESS, (uint8_t *)data, 2, 100);
}

uint8_t read_i2c(uint8_t cmd){
	uint8_t data[4];
	uint8_t data_new[4] = {0};
	HAL_I2C_Master_Receive(&hi2c1, DS3121_SLAVE_ADDRESS, (uint8_t *)data, 4, 100);
	for(uint8_t i=0; i<4; i++){
		data_new[i]= data[i];
	}
	return true;
}

#endif /* SRC_TIME_HANDLER_C_ */
