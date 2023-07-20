/*
 * time_handler.hpp
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */

#ifndef INC_TIME_HANDLER_H_
#define INC_TIME_HANDLER_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <main.hpp>
#include "stm32l0xx_it.h"

//functions which managing the 1ms time base
void counter_update_it(void);
static uint32_t my_counter=0;

/**
 * @brief Timer class used in all cases of timed events where timing is not that critical
 */

class Timer{
	public:
		Timer();
		void start_timer_ms(uint32_t);
		bool timeout(void);

	private:
		uint32_t my_now;
		uint32_t my_timer;
};

#endif /* INC_TIME_HANDLER_H_ */
