/*
 * output_handler.h
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

//#include <gpio.h>
#include <spi.h>
#include <main.hpp>

#ifndef INC_OUTPUT_HANDLER_H_
#define INC_OUTPUT_HANDLER_H_

#define FLYBACK_COOLDOWN 500	//ms


//variables
static bool flyback_state = false;
static uint32_t flyback_timer=0;

static uint8_t spi_data[4] = {0};
static uint32_t tube_data_input=0xff0000ff;


////variables for LED-control
//static uint32_t output_handler_timer = 0;
//static uint8_t color_number = 1;
//static uint32_t color=RED;
//static uint8_t active_led=0;

//functions

void set_tube_data(uint32_t);
uint32_t get_tube_data(void);
bool set_flyback_state(bool);
void spi_write(void);
bool get_flyback_state(void);

#endif /* INC_OUTPUT_HANDLER_H_ */
