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

//////////////// SPI-Subroutine ////////////////

/*
 * @brief: function to handle write commands via SPI - calls HAL
 */
void spi_write(void){
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, spi_data, 4, 100);
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_SET);
}

//////////////// Tube control ////////////////

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
	spi_data[0] = (uint8_t)(data >> 24);
	spi_data[1] = (uint8_t)(data >> 16);
	spi_data[2] = (uint8_t)(data >> 8);
	spi_data[3] = (uint8_t)(data);
}


/*
 * @brief: function to read tube data from output handler
 */
uint32_t get_tube_data(void){
	return tube_data_input;
}

//////////////// Flyback Converter ////////////////

/*
 * @brief function to set the state of the flayback converter
 * @param true will activate the converter; false will deactivate the converter - after new param, cooldown is performed
 * @return states how the output is set - use to check if function is not in cooldown mode where new values are discarded
 *
 * @info if a new param is written, a cooldown of 500ms is applied, before the converter can get a new state
 * the written value is discarded if the cooldown is still acitve. If the return value
 */
bool set_flyback(bool new_state){
	if(flyback_state!=new_state){
		if(timeout(flyback_timer)){
			flyback_state = new_state;
			flyback_timer = start_timer_ms(FLYBACK_COOLDOWN);						//new value gets applied
			if(flyback_state == true)
				HAL_GPIO_WritePin(enable_hv_GPIO_Port, enable_hv_Pin, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(enable_hv_GPIO_Port, enable_hv_Pin, GPIO_PIN_RESET);
		}
	}
	return flyback_state;
}

/*
 * @brief: function to check if flyback converter is activated or not
 * @return: true: converter is set to ON, false: converter is OFF
 * returns same value as set_flyback but without writing a value
 */
bool get_flyback_state(void){
	return flyback_state;
}


#endif /* SRC_OUTPUT_HANDLER_C_ */
