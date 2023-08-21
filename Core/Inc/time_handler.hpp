/*
 * time_handler.hpp
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#ifndef INC_TIME_HANDLER_HPP_
#define INC_TIME_HANDLER_HPP_

#include <i2c.h>
#include <main.hpp>

// DS3231 I2C ADDRESSES
#define DS3231_SLAVE_ADDRESS 0xD0
#define DS3231_MASTER_ADDRESS 0xD1

#define ADDR_SECONDS	0x00
#define ADDR_MINUTES	0x01
#define ADDR_HOURS		0x02
#define ADDR_DAY		0x03
#define ADDR_DATE		0x04
#define ADDR_MONTH		0x05	//also includes century
#define ADDR_YEAR		0x06
#define ADDR_A1SECONDS	0x07
#define ADDR_A1MINUTES	0x08
#define ADDR_A1HOURS	0x09
#define ADDR_A1DATE		0x0A	//also includes alarm 1 day
#define ADDR_A2MINUTES	0x0B
#define ADDR_A2HOURS	0x0C
#define ADDR_A2DATE		0x0D	//also includes alarm 1 day
#define ADDR_CONTROL	0x0E
#define ADDR_STATUS		0x0F	//also includes further control bits
#define ADDR_AGING_OFFSET 0x10
#define ADDR_MSB_TEMP	0x11
#define ADDR_LSB_TEMP	0x12

#define TIME_UPDATE_MS	100

static uint32_t time_handler_timer = 0;

//Data-transfer to/from RTC
void write_i2c_single(uint8_t, uint8_t);	//address/data
uint8_t read_i2c_single(uint8_t);			///address

struct time_struct {
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;
	uint8_t day = 0;
	uint8_t date = 0;
	uint8_t month = 0;
	uint16_t year = 0;
	bool century = 1;
	bool summer_time = 0;
	bool new_data=0;
};

extern time_struct data_from_RTC;	//data read from RTC gets here
extern time_struct data_to_RTC;	//data that have to be written on the RTC goes here

void write_time_i2c(void);
void read_time_i2c(void);	//read on startup to get time info in RAM
void write_date_i2c(void);
void read_date_i2c(void);	//read on startup to get date info in RAM

void set_time();			// enter time to write to RTC

// update time and date from RTC
void run_time_handler(void);

#endif /* INC_TIME_HANDLER_HPP_ */
