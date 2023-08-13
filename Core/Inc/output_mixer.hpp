/*
 * output_mixer.h
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#include <led_driver.hpp>
#include <main.hpp>
#include <output_handler.hpp>
//#include <time_handler.hpp>

#ifndef INC_OUTPUT_MIXER_H_
#define INC_OUTPUT_MIXER_H_

#define LED_REFRESH_RATE	30	//Hz
#define TUBE_REFRESH_RATE	30	//Hz

#define LED_REFRESH_RATE_MS	1000/LED_REFRESH_RATE
#define TUBE_REFRESH_RATE_MS	1000/LED_REFRESH_RATE

void run_output_mixer(uint8_t);	//processes data and call output
static uint32_t output_mixer_led_timer = 0;
static uint32_t output_mixer_tube_timer = 0;
//static uint32_t time_handler_timer = 0;


#endif /* INC_OUTPUT_MIXER_H_ */
