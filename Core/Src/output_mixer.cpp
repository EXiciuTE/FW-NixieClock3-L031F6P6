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


	// just some demo code!
	static uint8_t led_position = 0;
	static uint16_t color=YELLOW;
	static uint8_t state=0;
	if(input==1)
		led_position++;
	if(input==2)
		led_position--;
	if(led_position==board_size)
		led_position=0;
	if(led_position==255)
		led_position=board_size-1;
	if(input==4){
		switch(state){
		case 0:	color=0xff; break;
		case 1:	color=0xff00; break;
		case 2:	color=0xffff; break;
		case 3:	color=0x00; break;
		default: break;
		}
		state++;
		if(state==4)
			state=0;
	}

	if(timeout(output_mixer_led_timer)){
		output_mixer_led_timer = start_timer_ms(LED_REFRESH_RATE_MS);

		for(uint8_t i=0;i<6;i++){
			set_color(i,0x0,25);
		}
		set_color(led_position,color,25);
		send_data(true);
	}
	if(timeout(output_mixer_tube_timer)){
		output_mixer_tube_timer = start_timer_ms(TUBE_REFRESH_RATE_MS);
		//TODO: call output handler
	}
}


