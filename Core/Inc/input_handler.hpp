/*
 * input-handler.h
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#include <gpio.h>
#include <main.hpp>

#ifndef INC_INPUT_HANDLER_H_
#define INC_INPUT_HANDLER_H_

// inputs are sampled every 1ms; they have to hold a state for 3 ms to be registered as input and further processed
#define DEBOUNCE_TIME	5
#define LONG_PUSH_TIME	2000
#define SHORT_PUSH_TIME	50

#define DECODE_FILTER	0b0011
#define PUSH_FILTER		0b1100

#define DECODER_CW	0x1
#define DECODER_CCW	0x2
#define SHORT_PUSH		0x4
#define LONG_PUSH		0x8


uint8_t run_input_handler(void);
uint8_t debouncer(void);
uint8_t decoder(uint8_t);
uint8_t push_handler(uint8_t);


#endif /* INC_INPUT_HANDLER_H_ */
