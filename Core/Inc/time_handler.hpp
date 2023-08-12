/*
 * time_handler.hpp
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#ifndef INC_TIME_HANDLER_HPP_
#define INC_TIME_HANDLER_HPP_

#include <main.hpp>
#include <i2c.h>

// DS3231 I2C ADDRESSES
#define DS3121_SLAVE_ADDRESS 0xD0
#define DS3121_MASTER_ADDRESS 0xD1

#define ADDR_SECONDS		0x00
#define ADDR_MINUTES		0x01
#define ADDR_HOURS		0x02
#define ADDR_DAY			0x03
#define ADDR_DATE		0x04
#define ADDR_MONTH		0x05	//also includes century
#define ADDR_YEAR		0x06
#define ADDR_A1SECONDS	0x07
#define ADDR_A1MINUTES	0x08
#define ADDR_A1HOURS		0x09
#define ADDR_A1DATE		0x0A	//also includes alarm 1 day
#define ADDR_A2MINUTES	0x0B
#define ADDR_A2HOURS		0x0C
#define ADDR_A2DATE		0x0D	//also includes alarm 1 day
#define ADDR_CONTROL		0x0E
#define ADDR_STATUS		0x0F	//also includes further control bits
#define ADDR_AGING_OFFSET 0x10
#define ADDR_MSB_TEMP	0x11
#define ADDR_LSB_TEMP	0x12


void write_i2c(uint8_t, uint8_t);	//data/address
uint8_t read_i2c(uint8_t);			//data/address


//debug
static uint8_t houres = 0;
static uint8_t minutes = 0;
static uint8_t seconds = 0;
//read_time
//write_time


#endif /* INC_TIME_HANDLER_HPP_ */
