/*
 * timer_hander.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: class for handling communication with real time clock
 */

#include <time_handler.hpp>

#ifndef SRC_TIME_HANDLER_C_
#define SRC_TIME_HANDLER_C_

/**
 * @brief: funcitons updates counter for 1ms time base - triggered by timer hardware module
 */
void counter_update_it(){
	sys_counter++;
}

/**
 * @brief function to calculate end-time of timer event
 * @param ms: amount of time, the timer should run
 * @return systemtick value, at which the timer run out
 * @usage: call function and insert return value into timeout() function, to check if timeout occured
 */
uint32_t start_timer_ms(uint32_t new_timer_value){
	return new_timer_value += sys_counter;
}

/**
 * @brief checks if the timer set by start_timer_ms for a given class is expired
 * @return if present tick counter is higher then timer value set by start_timer_ms
 * the timer is expired and the function returns true
 */
bool timeout(uint32_t end_time){
	if(end_time>sys_counter)
		return false;
	else
		return true;
}

#endif /* SRC_TIME_HANDLER_C_ */
