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
	static bool dls_flag_only = false;
	if(data_to_RTC.new_data==2){
		data_to_RTC.new_data = 0;
		write_date_i2c();
		dls_flag_only = true;	//enable change flag only for next Daylight safe check
	}
	if(data_to_RTC.new_data==1){
		data_to_RTC.new_data = 0;
		write_time_i2c();
		dls_flag_only = true;
	}


	//TODO: add summertime
	//TODO: add plausibility check when safing date

	if(timeout(time_handler_timer)){
		time_handler_timer = start_timer_ms(TIME_UPDATE_MS);

		read_date_i2c();
		read_time_i2c();
		dls_check(dls_flag_only);	//check for daylight saving + apply
		dls_flag_only = false;
	}
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

	data_from_RTC.seconds = ((temp[0]&0xf0)>>4)*10 + (temp[0]&0x0f);	//conversion from hex to dec
	data_from_RTC.minutes = ((temp[1]&0xf0)>>4)*10 + (temp[1]&0x0f);
	data_from_RTC.hours = ((temp[2]&0x30)>>4)*10 + (temp[2]&0x0f);

	//check for summer time

}

/**
 * @brief function to write time information from RAM into RTC
 * read data from data_to_RTC and transfer to RTC
 */
void write_time_i2c(void){
	uint8_t temp[4];

	//convert from summer time

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
	data_from_RTC.year = 1900 + (data_from_RTC.century * 100);
	data_from_RTC.year = data_from_RTC.year + (((temp[3]&0xf0)>>4)*10) + ((temp[3]&0x0f)%10);

	//check for summer time
}

/**
 * @brief function to write date information from RAM into RTC
 * read data from data_to_RTC and transfer to RTC
 */
void write_date_i2c(void){
	uint8_t temp[5]={0};

	//convert from summer time

	temp[0] = ADDR_DAY;	//start address for write operation
	temp[1] = data_to_RTC.day;
	temp[2] = ((data_to_RTC.date/10)<<4)|data_to_RTC.date%10;
	temp[3] = ((data_to_RTC.month/10)<<4)|data_to_RTC.month%10 | (data_to_RTC.year/2000)<<7;
	temp[4] = ((data_to_RTC.year %100) / 10)<<4 | data_to_RTC.year%10;

	HAL_I2C_Master_Transmit(&hi2c1, DS3231_MASTER_ADDRESS, (uint8_t *)temp, 5, 10);
}

/**
 *  @brief function checks Flag, if day light saving is applied, applies if necessary, re/sets Flag if necessary
 *  @param flag_only: functions only alters the flag and not the actual time value inside the RTC
 *  @info: If DLS applies, the hour byte in the RTC is increased by one and the change is signalized by turning the
 *  ALARM 1 SECONDS Byte to 1 - this bit is used for the DLS FLAG (1 = DLS is applied, 0 = DLS is not applied)
 */
void dls_check(bool flag_only){
	static bool dls_needed = false;
	static bool dls_active = false;
	uint8_t offset = 0;			//add hysterese behaviour for fall, when clock switches from 03:00 to 02:00 - code will reactivate dls, since dls is deactivated altough it should be activated based on time+date

	//read DLS bit
	dls_active = read_i2c_single(ADDR_A1SECONDS);
	if(dls_active==0x1){
		offset = 1;
		data_from_RTC.summer_time = true;
	}
	else{
		offset = 0;
		data_from_RTC.summer_time = false;
	}
	//check date + time: Should DLS be active?
	if((data_from_RTC.month == 3 && data_from_RTC.date>=25 && data_from_RTC.day==7 && data_from_RTC.hours>=2 )	//March, last sunday, later than 02:00
	 || (data_from_RTC.month == 3 && data_from_RTC.day<=6 && (data_from_RTC.date-data_from_RTC.day)>=25 )		//March after last sunday
	 || (data_from_RTC.month >= 4 && data_from_RTC.month <=9)													//April - September
	 || (data_from_RTC.month == 10 && ( (((data_from_RTC.date-data_from_RTC.day)<=24)&& data_from_RTC.day<=6) || (data_from_RTC.day==7 && data_from_RTC.date<=24)))
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	//October till saturday befor last sunday
	 || (data_from_RTC.month == 10 && data_from_RTC.day == 7 && data_from_RTC.date >=25 && data_from_RTC.hours<=(1+offset))){
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	//October last sunday till 02:00
		dls_needed = true;
	}
	else{
		dls_needed = false;
	}

	//check if DLS bit should be applied: is DLS active?
	if(dls_active == false && dls_needed == true){
		if(flag_only==false){
			write_i2c_single(ADDR_HOURS, data_from_RTC.hours+1);
			data_from_RTC.hours++;		//alter local variable, so no new read is required before new value is displayed
			//increase RTC by one hour - should only happen when summer is reached, not through time set
		}
		write_i2c_single(ADDR_A1SECONDS, 0x1);	//set DLS FLAG
	}
	if(dls_active == true && dls_needed == false){
		if(flag_only == false){
			write_i2c_single(ADDR_HOURS, data_from_RTC.hours-1);
			data_from_RTC.hours--;
			//decrease RTC by one hour - should only happen when winter is reached, not through time set
		}
		write_i2c_single(ADDR_A1SECONDS, 0x0);	//reset DLS FLAG
	}
}

/**
 * @brief write single register
 * @param cmd: register address to which the data should be written
 * @param data: 8bit data which should be written to the register
 */
void write_i2c_single(uint8_t cmd, uint8_t data){
	uint8_t data_t[2];
	data_t[0] = cmd;
	data_t[1] = data;
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

