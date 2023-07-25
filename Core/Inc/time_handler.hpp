/*
 * time_handler.hpp
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#ifndef INC_TIME_HANDLER_HPP_
#define INC_TIME_HANDLER_HPP_

#include <main.hpp>

//functions to use systemwide
uint32_t start_timer_ms(uint32_t);
//bool timeout(uint32_t);

//functions which managing the 1ms time base
void counter_update_it();
static uint32_t sys_counter=0;

#endif /* INC_TIME_HANDLER_HPP_ */
