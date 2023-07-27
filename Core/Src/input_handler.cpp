/*
 * input-handler.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: class for handling state of input signals
 */

#include <input_handler.hpp>

//debouncer
typedef struct input_debounce_timer{
	uint8_t rotary_a=0;
	uint8_t rotary_b=0;
	uint8_t rotary_push=0;
};


/**
 * @brief function to handle all input related actions - gets called every ms
 * @return current input actions contained in one byte
 * 	b0 HIGH: decoder turned right one click
 * 	b1 HIGH: decoder turned left one click
 * 	b2 HIGH: push button short pushed and released	>50ms
 * 	b3 HIGH: push button long pushed and NOT released >2000ms
 */
uint8_t run_input_handler(void){
	static uint8_t test_byte=0;
	uint8_t calculated_input_byte=0;
	static uint8_t input_data=0;
	input_data=debouncer();											//debouncing
	calculated_input_byte |= decoder(input_data&DECODE_FILTER);		//rotary decoding
	calculated_input_byte |= push_handler(input_data&PUSH_FILTER);	//handling long press for push button

	if((calculated_input_byte&DECODE_FILTER) == DECODER_CW){
		test_byte++;
	}
	if((calculated_input_byte&DECODE_FILTER) == DECODER_CCW){
		test_byte--;
	}

	return calculated_input_byte;
}

/**
 * @brief read physical GPIO state and debounces it
 * @return debounced input states in one byte
 * 	b0: HIGH: rotary_a high
 * 	b1: HIGH: rotary_b high
 * 	b2: HIGH: rotary_push high
 */
uint8_t debouncer(void){
	static uint8_t debounced_data=0;
	static input_debounce_timer debounce_timer;
	//let's debounce some inputs
	if(HAL_GPIO_ReadPin(rot_a_GPIO_Port, rot_a_Pin)==true){
		if(debounce_timer.rotary_a<DEBOUNCE_TIME)
			debounce_timer.rotary_a++;
	}
	else{
		if(debounce_timer.rotary_a!=0)
			debounce_timer.rotary_a--;
	}
	if(HAL_GPIO_ReadPin(rot_b_GPIO_Port, rot_b_Pin)==true){
		if(debounce_timer.rotary_b<DEBOUNCE_TIME)
			debounce_timer.rotary_b++;
	}
	else{
		if(debounce_timer.rotary_b!=0)
			debounce_timer.rotary_b--;
	}
	if(HAL_GPIO_ReadPin(rot_push_GPIO_Port, rot_push_Pin)==true){
		if(debounce_timer.rotary_push<DEBOUNCE_TIME)
			debounce_timer.rotary_push++;
	}
	else{
		if(debounce_timer.rotary_push!=0)
			debounce_timer.rotary_push--;
	}

	//now lets check the inputs
	if(debounce_timer.rotary_a==DEBOUNCE_TIME)
		debounced_data |= 0x1;
	if(debounce_timer.rotary_a==0)
		debounced_data &= ~0x1;
	if(debounce_timer.rotary_b==DEBOUNCE_TIME)
		debounced_data |= 0x2;
	if(debounce_timer.rotary_b==0)
		debounced_data &= ~0x2;
	if(debounce_timer.rotary_push==DEBOUNCE_TIME)
		debounced_data |= 0x4;
	if(debounce_timer.rotary_push==0)
		debounced_data &= ~0x4;
	return debounced_data;
}

/**
 * @brief functions which calculates the spinning direction of a rotary decoder
 * @param input debounced data - see debouncer() [0b = a; b1 = b]
 * @return information, if decoder turned left, right or not
 *  b0 HIGH: did turn right - CW
 *  b1 HIGH: did turn left - CCW
 */
uint8_t decoder(uint8_t new_state){
	static uint8_t last_state=0;
	uint8_t decoded_data=0;
	if(last_state == 0x00){
		if(new_state==0x01)
			decoded_data =DECODER_CW;
		if(new_state==0x10)
			decoded_data =DECODER_CCW;
	}
	if(last_state == 0x01){
		if(new_state==0x11)
			decoded_data =DECODER_CW;
		if(new_state==0x00)
			decoded_data =DECODER_CCW;
	}
	if(last_state == 0x11){
		if(new_state==0x10)
			decoded_data =DECODER_CW;
		if(new_state==0x01)
			decoded_data =DECODER_CCW;
	}
	if(last_state == 0x10){
		if(new_state==0x00)
			decoded_data =DECODER_CW;
		if(new_state==0x11)
			decoded_data =DECODER_CCW;
	}
	last_state = new_state;
	return decoded_data;
}

/**
 * @brief functions to decide if push button is short, long or not pushed
 * @param input debounced data - see debouncer()
 * @return wether input was pressed short, long or not at all
 * 	b2 HIGH: push button short pushed and released	>50ms
 * 	b3 HIGH: push button long pushed and NOT released >2000ms
 */
uint8_t push_handler(uint8_t push_button){
	push_button = push_button >> 2;
	static uint16_t input_hold_timer=0;
	uint8_t push_state=0;

	if(push_button == true && input_hold_timer<=LONG_PUSH_TIME+2){
		input_hold_timer++;
	}

	if(push_button == false){
		if(input_hold_timer>=SHORT_PUSH_TIME && input_hold_timer<=LONG_PUSH_TIME)
			push_state = SHORT_PUSH;
		input_hold_timer=0;
	}

	if(input_hold_timer==LONG_PUSH_TIME){
		push_state = LONG_PUSH;
	}
	return push_state;
}

