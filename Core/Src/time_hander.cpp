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
 * @brief: funcitons updates counter for 1ms time base
 */
void counter_update_it(){
	my_counter++;
}

/**
 * @brief function starts a timer for the given class
 * offset is set to acutal tick
 * @param 32bit value in ms for how long the timer should run
 */
void Timer::start_timer_ms(uint32_t new_timer_value){
	my_timer = my_counter + new_timer_value;
}

/**
 * @brief checks if the timer set by start_timer_ms for a given class is expired
 * @return if present tick counter is higher then timer value set by start_timer_ms
 * the timer is expired and the function returns true
 */
bool Timer::timeout(){
	if(my_counter>my_timer)
		return true;
	else
		return false;
}


#endif /* SRC_TIME_HANDLER_C_ */
