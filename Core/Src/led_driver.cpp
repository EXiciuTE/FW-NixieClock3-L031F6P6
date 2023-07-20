/*
 * led_driver.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: class for generating the output buffer for the dma output
 */

#include <led_driver.hpp>


/*
 * @brief function to write new color to output queue
 * @param led_number number to which led the color should be applied
 * @param hex_code	enter RGB Hex code of color or use predefined values
 * @param brightness choose brightness level between 1 and 100
 */
void set_color(uint8_t led_number, uint32_t hex_code, uint8_t brightness){
	brightness_controller = 0;
	led_data[led_number]=0;
	if(led_number>=MAX_LED)
		led_number=MAX_LED-1;
	if(brightness>100)
		brightness = 100;
	if(brightness<1)
		brightness = 1;
	//apply brightness to each color segment and reshuffle bits as required by the LED: green-red-blue
	//red
	brightness_controller = (uint16_t)((hex_code&0xff0000) >> 16)*brightness/100;
	led_data[led_number] |= (uint16_t)brightness_controller << 8;
	//green
	brightness_controller = (uint16_t)((hex_code&0xff00) >> 8)*brightness/100;
	led_data[led_number] |= (uint32_t)brightness_controller << 16;
	//blue
	brightness_controller = (uint16_t)(hex_code&0xff)*brightness/100;
	led_data[led_number] |= brightness_controller;
}

/*
 * @brief: calling this command will prepare the data and perform a dma request to output the Data
 * also used to turn led on or off
 * @param state true: actual data is send to led; false: 0 is send to turn LED off
 */
void send_data(bool state){
//try these timings, since they are different from normal WS2812b: https://led-stuebchen.de/download/WS2812-2020_V1.1_EN.pdf
//T0H = 220ns~380nS
//T1H = 580ns~1us	->
//T0L = 580ns~1us
//T1L = 220ns~420ns
//RES = >280us
// 1 period = 40 cycles = 1,25us = 800kHz

	index = 0;
	for(uint8_t i=0;i<MAX_LED;i++){
		for(uint8_t j = NUM_BITS-1;j>0; j--){
			if(state == false){
				pwm_data[index] = 12;
			}
			else{
				if(led_data[i]&(1<<j))
					pwm_data[index] = 27;	//one period = 40 --> High bit = 70%
				else
					pwm_data[index] = 13;	//one period = 40 --> High bit = 30%
			}
			index++;
		}
	}
	for(uint8_t i = 0; i<LED_RESET; i++){
		pwm_data[index]=0;
		index++;
	}
	if(dma_running==false){
		HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t*)pwm_data, (MAX_LED*NUM_BITS)+LED_RESET);
		dma_running=true;
	}
}

/*
 * @brief: callback function when transfer is finished to stop the DMA
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_3);
	dma_running=false;
}

/*
 * @brief: start the DMA transfer
 */
void dma_start(void){

}

/*
 * stops the DMA transfer
 */
void dma_stop(void){

}
