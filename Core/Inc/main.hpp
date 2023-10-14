/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stdbool.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define rot_b_Pin GPIO_PIN_0
#define rot_b_GPIO_Port GPIOA
#define rot_a_Pin GPIO_PIN_1
#define rot_a_GPIO_Port GPIOA
#define sens_size_Pin GPIO_PIN_3
#define sens_size_GPIO_Port GPIOA
#define rot_push_Pin GPIO_PIN_4
#define rot_push_GPIO_Port GPIOA
#define sr_shift_output_Pin GPIO_PIN_6
#define sr_shift_output_GPIO_Port GPIOA
#define enable_hv_Pin GPIO_PIN_1
#define enable_hv_GPIO_Port GPIOB

#define MAIN_TIMER 1
#define UP_FLASH_ADDR 0x08080000
#define WORDS_IN_FLASH	13	//8*6+3 / 4


/* USER CODE BEGIN Private defines */
//basic global variables
extern uint8_t board_size;
static uint32_t main_timer = 0;
extern uint8_t on_time[8][6];	//8: time areas, 6 values per time area (start/end day, start/end time hh + mm)
extern uint8_t misc_setting[3];	//led setting + brightness + point setting - for LED color definition see led_driver.hpp

//variables for runtime handling
static uint32_t sys_counter=0;			//counts system time in 1ms increments
uint32_t start_timer_ms(uint32_t);		//calculates the end-time relative to the system time at which the timer runs out
bool timeout(uint32_t);					//compares system time with parameter - input return value of start_timer_ms() here
void counter_update_it();				//interrupt triggered function to increase sys_counter

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
