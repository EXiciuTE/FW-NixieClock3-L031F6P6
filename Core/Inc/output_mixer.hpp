/*
 * output_mixer.h
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#ifndef INC_OUTPUT_MIXER_H_
#define INC_OUTPUT_MIXER_H_

#include <led_driver.hpp>
#include <main.hpp>
#include <output_handler.hpp>
#include <time_handler.hpp>

#define LED_REFRESH_RATE	30	//Hz
#define TUBE_REFRESH_RATE	30	//Hz

#define LED_REFRESH_RATE_MS	1000/LED_REFRESH_RATE
#define TUBE_REFRESH_RATE_MS	1000/LED_REFRESH_RATE

void run_output_mixer(uint8_t);	//processes data and call output
static uint32_t output_mixer_led_timer = 0;
static uint32_t output_mixer_tube_timer = 0;
static uint32_t blink_1_timer = 0;
static uint32_t blink_2_timer = 0;

// menu variables
static uint8_t current_menu = 0;
static uint8_t new_menu = 0;
static uint8_t current_state = 0;
static uint8_t old_state = 0;

// misc variables
static uint32_t active_color=GREEN;

//debug
static uint8_t brightness = 0;
static uint8_t number_value = 0;
static bool blink_state_1 = false;

#endif /* INC_OUTPUT_MIXER_H_ */
