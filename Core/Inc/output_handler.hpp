/*
 * output_handler.h
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#include <main.hpp>
#include <spi.h>

#ifndef INC_OUTPUT_HANDLER_H_
#define INC_OUTPUT_HANDLER_H_

#define FLYBACK_COOLDOWN 500	//ms


//variables
static bool flyback_state = false;
static uint32_t flyback_timer=0;

static uint8_t spi_data[4] = {0};

//functions

void set_number(uint8_t, uint8_t);
void set_point(uint8_t , bool);
void set_output(void);
bool set_flyback_state(bool);
void spi_write(void);
bool get_flyback_state(void);

#endif /* INC_OUTPUT_HANDLER_H_ */
