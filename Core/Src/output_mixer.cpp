/*
 * output_mixer.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: class for managing all outgoing signals. Generates commands for the output handler and led driver
 */

#include <output_mixer.hpp>


/**
 * @brief Processes all data and generates commands for Output Handler and LED driver
 *
 * gets called every ms
 * gets data directly from respective .hpp files
 */
void run_output_mixer(uint8_t input){
	// get all data
	// compare if something changed
	// handle led animation state?
	// make sure that a minimum of tubes or points is active!

	// just some demo code! - move led from left to right - change color on press
	static uint8_t led_position = 1;
	if(input==1)
		led_position++;
	if(input==2)
		led_position--;
	if(led_position==board_size)
		led_position=1;
	if(led_position==0)
		led_position=board_size-1;

	// switch HV-enable pin by push of button - change led to indicate state (RED = ON, GREEN = OFF)
	static bool tmp = false;
	static uint32_t tube_data = 0;
	if(input==0x1){
		if(tube_data == 999999)
			tube_data = 0;
		else
			tube_data += 111111;
	}
	if(input==0x2){
		if(tube_data == 0)
			tube_data = 999999;
		else
			tube_data -= 111111;
	}

	if(timeout(output_mixer_tube_timer)){
		output_mixer_tube_timer = start_timer_ms(TUBE_REFRESH_RATE_MS);
		set_number(0, (tube_data/100000) %10);
		set_number(1, (tube_data/10000) %10);
		set_point(0, tube_data &0xf);
		set_point(1, tube_data &0xf);
		set_point(2, tube_data &0xf);
		set_point(3, tube_data &0xf);

		set_number(2, data_from_RTC.minutes/10);
		set_number(3, data_from_RTC.minutes%10);
		set_number(4, data_from_RTC.seconds/10);
		set_number(5, data_from_RTC.seconds%10);

		set_output();
	}

	if(input==0x4){
		flyback_state = !flyback_state;
		tmp = set_flyback_state(flyback_state);
	}
	if(tmp == true)
		set_color(0, RED, 25);
	else
		set_color(0, GREEN , 25);

	//	Time LED-Output
	if(timeout(output_mixer_led_timer)){
		output_mixer_led_timer = start_timer_ms(LED_REFRESH_RATE_MS);
		for(uint8_t i=1;i<6;i++){
			set_color(i,0x0,25);
		}
		set_color(led_position,YELLOW,25);
		send_data(true);
	}



}


