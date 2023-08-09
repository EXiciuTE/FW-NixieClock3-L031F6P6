/*
 * time_handler.hpp
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#ifndef INC_TIME_HANDLER_HPP_
#define INC_TIME_HANDLER_HPP_

#include <main.hpp>

// DS3231 I2C ADDRESSES
#define DS3121_SLAVE_ADDRESS 0b1101000
#define DS3121_MASTER_ADDRESS 0b1101001

#define SECONDS		0x00
#define MINUTES		0x01
#define HOURS		0x02
#define DAY			0x03
#define DATE		0x04
#define MONTH		0x05	//also includes century
#define YEAR		0x06
#define A1SECONDS	0x07
#define A1MINUTES	0x08
#define A1HOURS		0x09
#define A1DATE		0x0A	//also includes alarm 1 day
#define A2MINUTES	0x0B
#define A2HOURS		0x0C
#define A2DATE		0x0D	//also includes alarm 1 day
#define CONTROL		0x0E
#define STATUS		0x0F	//also includes further control bits
#define AGING_OFFSET 0x10
#define MSB_TEMP	0x11
#define LSB_TEMP	0x12


void write_i2c(uint8_t, uint8_t);	//data/address
uint8_t read_i2c(uint8_t);			//data/address

//read_time
//write_time


#endif /* INC_TIME_HANDLER_HPP_ */
