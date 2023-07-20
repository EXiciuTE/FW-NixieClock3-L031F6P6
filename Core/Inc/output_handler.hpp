/*
 * output_handler.h
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#include <gpio.hpp>
#include <main.hpp>
#include <spi.hpp>

#ifndef INC_OUTPUT_HANDLER_H_
#define INC_OUTPUT_HANDLER_H_

//functions
void run_output_handler(void);			//should get called every ms
void set_tube_input(uint32_t);
uint32_t get_tube_data(void);
uint16_t set_flyback(bool);
void write_spi(void);
bool get_flyback_state(void);

//variables
static uint32_t tube_data_input=0xff0000ff;
static bool flyback_state_software = false;
static bool flyback_state_hardware = false;
static uint8_t flyback_change_cooldown = 250;
static uint8_t spi_data[4] = {0};
static uint8_t point_data = 0;
static uint8_t spi_refresh = 100;




#endif /* INC_OUTPUT_HANDLER_H_ */
