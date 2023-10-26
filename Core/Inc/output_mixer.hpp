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

void submenu_0_display_time(void);
void submenu_1_set_time(uint8_t, bool);
void submenu_2_set_date(uint8_t, bool);
void submenu_3_set_onoff(uint8_t, bool);
void submenu_4_set_led_color(uint8_t, bool);
void submenu_5_set_points(uint8_t);
void submeun_6_random_animation(void);
bool submenu_9_menu_select(uint8_t);
bool write_flash_new_data(void);
void animation_random(void);
void animation_shifting(void);

static uint32_t output_mixer_led_timer = 0;
static uint32_t output_mixer_tube_timer = 0;

// menu variables
static uint8_t current_menu = 0;
static uint8_t selected_menu = 0;	//used for menu navigator
static bool new_selected_menu = false;
static uint8_t old_state = 0;
static bool	flash_write = false;

// misc variables
static bool flyback_status = false;

//debug
static uint8_t brightness = 0;


#endif /* INC_OUTPUT_MIXER_H_ */
