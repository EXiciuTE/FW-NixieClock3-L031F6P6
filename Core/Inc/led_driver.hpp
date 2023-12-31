/*
 * led_driver.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 */


#include <dma.h>
#include <gpio.h>
#include <main.hpp>
#include <tim.h>


#ifndef INC_LED_DRIVER_C_
#define INC_LED_DRIVER_C_

//defines
#define MAX_LED 6
#define NUM_BITS 24
#define HIGH_CLOCKS 27	//27 - 13: if last LED is green, second last shimmers blue too; 24-16 flicker
#define LOW_CLOCKS 13
// 25-15 - flicker in menu
// 26-14 - flicker in menu
// 27-13 - flicker off - with 120 reset
// 27-13 - flicker in menu - with 240
#define LED_RESET 240	// theoretically >280us; 1 step=1,25us --> 240/1,25 = 300us


//define default colors
//#define WHITE 		0xffffff
//#define RED 			0xff0000
//#define ORANGE 		0xffa500
//#define YELLOW 		0xffff00
//#define CHARTREUSE	0x7fff00
//#define GREEN 		0x00ff00
//#define TURQUOISE 	0x00ff7f
//#define CYAN 			0x00ffff
//#define AZURE 		0x007fff
//#define BLUE 			0x0000ff
//#define VIOLET 		0xee82ee
//#define MAGENTA 		0xff00ff
//#define ROSE 			0xff007f
//#define OFF			0x000000

extern uint32_t colors_hex[14];

typedef enum {led_off, white, red, orange, yellow, chartreuse, green, turquoise, cyan, azure, blue, violet, magenta, rose} Colors;


void set_color(uint8_t, uint32_t, uint8_t);
void send_data(bool);

static uint8_t brightness_controller = 0;
static uint16_t index=0;
static uint32_t led_data[MAX_LED]={0};
static uint16_t pwm_data[(MAX_LED*NUM_BITS)+LED_RESET]={0};
static bool dma_running = false;

#endif /* INC_LED_DRIVER_C_ */
