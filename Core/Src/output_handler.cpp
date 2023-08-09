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
//MSB first
/*
 * @brief: function to handle write commands via SPI - calls HAL
 */
void spi_write(void){
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, spi_data, 4, 100);
	HAL_GPIO_WritePin(sr_shift_output_GPIO_Port, sr_shift_output_Pin, GPIO_PIN_SET);
}

//////////////// Tube control ////////////////
// Multiplexer arrangement: data -> hh -> 	mm -> 	point (lh, ll, rh, rl) -> 	ss
//								 a-d e-h	a-d e-h			a,  b,  c,  d		a-d e-h
// MSB first
// lower 4 bit per byte represent the left tube

/*
 * @brief: function to write new tube data to output stage, tube data and point data - does performe output update
 * @param tube_data hex values represent the figure to be displayed on the tube; values above 9 mean OFF
 * 0x00yy0000 = houres (tube 1,2)
 * 0x0000yy00 = minutes (tube 3,4)
 * 0x000000yy = seconds (tube 5,6)
 * @param point_data b0 - b4 symbolizes the state of the points
 * b0 - left high, b1 - left low, b3 - right high, b4 - right low
 */

void set_tube_data(uint32_t tube_data, uint8_t point_data){
	//check for correct input data
	if(board_size == 4){
		tube_data&=0xffff00;
		point_data&=0x03;
	}
	else
		point_data &= 0x0f;	//mask unused bits


	// annoying bit shiftery... 123456 --> 214365
	// lower 4 bit per byte represent the left tube ;(
	uint8_t low_end = 0;
	low_end = (tube_data & 0xff) >> 0;
	spi_data[0] = low_end << 4 | low_end >> 4;
	low_end = (tube_data & 0xff00) >> 8;
	spi_data[2] = low_end << 4 | low_end >> 4;
	low_end = (tube_data & 0xff0000) >> 16;
	spi_data[3] = low_end << 4 | low_end >> 4;

	spi_data[1] = point_data;
	spi_write();
}


/*
 * @brief: function to read tube data from output handler
 */
uint32_t get_tube_data(void){
	return tube_data;
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
bool set_flyback_state(bool new_state){
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
