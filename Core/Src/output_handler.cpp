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
// lower points are not populated due to later design decisions - uns lh and rh only

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
//	//check for correct input data
//	if(board_size == 4){
//		tube_data&=0xffff00;
//		point_data&=0x03;
//	}
//	else
//		point_data &= 0x0f;	//mask unused bits


	// annoying bit shiftery... 123456 --> 214365
	// lower 4 bit per byte represent the left tube ;(
	uint8_t low_end = 0;
	low_end = (tube_data & 0xff) >> 0;
	spi_data[0] = low_end << 4 | low_end >> 4;
	low_end = (tube_data & 0xff00) >> 8;
	spi_data[2] = low_end << 4 | low_end >> 4;
	low_end = (tube_data & 0xff0000) >> 16;
	spi_data[3] = low_end << 4 | low_end >> 4;

//	spi_data[1] = point_data;

}

/**
 * @brief writes a value to the register of the specified number
 * @param tube_number is the selected tube. 0 is the left tube, 5 the most right one (if 6 tubes are installed)
 * @param value is the value to be displayed - values other than 0 to 9 will turn the tube off
 * @note: values are stored in their variables so that they can easily be copied into the spi_data
 */
void set_number(uint8_t tube_number, uint8_t value){
	uint8_t temp = 0;

	if(value>0xf)
		value=0xf;

	if(tube_number%2 == 1)
		value = value << 4;

	switch(tube_number){
		case 0: temp = houres; houres = (temp & 0xf0) | value; break;
		case 1: temp = houres; houres = (temp & 0x0f) | value; break;	//number already shifted 4 bites
		case 2: temp = minutes; minutes = (temp & 0xf0) | value; break;
		case 3: temp = minutes; minutes = (temp & 0x0f) | value; break;
		case 4: temp = seconds; seconds = (temp & 0xf0) | value; break;
		case 5: temp = seconds; seconds = (temp & 0x0f) | value; break;
		default: break;
	}

//	switch(tube_number){
//		case 0: houres = (houres & 0x0f) | ((value & 0x0f) << 4); break;
//		case 1: houres = (houres & 0xf0) | (value & 0x0f); break;
//		case 2: minutes = (minutes & 0x0f) | ((value & 0x0f) << 4); break;
//		case 3: minutes = (minutes & 0xf0) | (value & 0x0f); break;
//		case 4: seconds = (seconds & 0x0f) | ((value & 0x0f) << 4); break;
//		case 5: seconds = (seconds & 0xf0) | (value & 0x0f); break;
//		default: break;
//	}
}

/**
 * @brief turns a specific point on or off
 * @param point_number is the selected point starting from left - 0: lh, 1: ll, 2: rh, 3: rl
 * @param state true -> point is on; false -> point is off
 */
void set_point(uint8_t point_number, bool state){
	switch(point_number){
		case 0: if(state == true) points |= 0x1; else points &= !0x1; break;
		case 1: if(state == true) points |= 0x2; else points &= !0x2; break;
		case 2: if(state == true) points |= 0x4; else points &= !0x4; break;
		case 3: if(state == true) points |= 0x8; else points &= !0x8; break;
		default: break;
	}
}

/**
 * @brief function to send data to output
 */
void set_output(void){
	if(board_size == 6)
		spi_data[0] = seconds;
	else
		spi_data[0] = 0x00;
	spi_data[1] = points;
	spi_data[2] = minutes;
	spi_data[3] = houres;

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
