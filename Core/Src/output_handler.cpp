/*
 * output_handler.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: functions for managing all output signals like the state of the flyback converter or the shift register signals
 */

#include <output_handler.hpp>

#ifndef SRC_OUTPUT_HANDLER_C_
#define SRC_OUTPUT_HANDLER_C_


/**
 * @brief main function of output handler - gets called every ms and manages outgoing signals
 */
void run_output_handler(uint8_t input){//TODO: remove input value
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
	for(uint8_t i=0;i<6;i++){
		set_color(i,0x0,25);
	}
	set_color(led_position,color,25);
	send_data(true);


}

/*
 * @brief: function to handle write commands via SPI - calls HAL
 */
void write_spi(void){
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, spi_data, 4, 100);
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_SET);
}

/*
 * @brief: applies set output data to acutal periphery; should be called every ms
 */
//void run_output_handler(void){
//	//waits for output-switch-cooldown to get zero - the most recent command is than applied to the output (if the state changes)
//	if(flyback_change_cooldown==0){
//		if(flyback_state_hardware != flyback_state_software){
//			flyback_state_hardware = flyback_state_software;
//			HAL_GPIO_WritePin(enable_hv_GPIO_Port, enable_hv_Pin, GPIO_PIN_SET);	//change Pin
//			flyback_change_cooldown=250;
//		}
//	}
//
//	//write displayed data to shiftregisters via SPI
//
//	if(spi_refresh==0){
//		write_spi();
//		spi_refresh = 100;
//	}
//	else{
//		spi_refresh--;
//	}
//}

/*
 * @name: set_tube_input(uint32_t)
 * @brief: function to write new tube data to output stage; does not output data
 * @param: hex values represent the figure to be displayed on the tube
 * 0xyy000000 = houres (tube 1,2)
 * 0x00yy0000 = minutes (tube 3,4)
 * 0x0000y000 = pints (point 1,2,3,4) - (A - left high, B - left low, C - right high, D - right low)
 * 0x000000yy = seconds (tube 5,6)
 */

void set_tube_input(uint32_t data){
	//check for correct input data
	if(board_size == 4){
		data&=0xffffC000;
	}
	point_data = (uint8_t)(data >> 8);
	spi_data[0] = (uint8_t)(data >> 24);
	spi_data[1] = (uint8_t)(data >> 16);
	spi_data[2] = (uint8_t)(data >> 8);
	spi_data[3] = (uint8_t)(data);

	tube_data_input = data;
	spi_refresh = 0;			//when new data is available, refresh delay is set to 0
}


/*
 * @brief: function to read tube data from output handler
 */
uint32_t get_tube_data(void){
	return tube_data_input;
}

/*
 * @brief: function to set flyback converter bit
 * @param: true will activate, false will deactivate the flyback converter
 * output is set with next call of run();
 * after change, output will be blocked for 250ms (if run is called every ms)
 * when cooldown is complete, run will apply the last value send to this functions
 * cooldown is reset in the run() when the state of the output pin changes
 * @return: whether the input will be applied on the next run command or not
 * 0: data will be applied on next call of run();
 * x: data will be applied in x ms
 *
 */
uint16_t set_flyback(bool data){
	flyback_state_software = data;
	return flyback_change_cooldown;
}

/*
 * @brief: function to read the state of the flyback converter
 * @return: returns the actual state of the output; true: flyback is on; false: flyback is off
 * does not care about cooldown value or changed value that awaits to be send to output
 */
bool get_flyback_state(void){
	return flyback_state_hardware;
}


#endif /* SRC_OUTPUT_HANDLER_C_ */
