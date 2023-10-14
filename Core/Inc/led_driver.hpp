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
//#define WHITE 		0xffffff	//color 1
#define RED 		0xff0000	//color 2
//#define ORANGE 		0xffa500	//color 3
//#define YELLOW 		0xffff00	//color 4
//#define CHARTREUSE	0x7fff00	//color 5
#define GREEN 		0x00ff00	//color 6
//#define TURQUOISE 	0x00ff7f	//color 7
#define CYAN 		0x00ffff	//color 8
//#define AZURE 		0x007fff	//color 9
//#define BLUE 		0x0000ff	//color 10
//#define VIOLET 		0xee82ee	//color 11
//#define MAGENTA 	0xff00ff	//color 12
//#define ROSE 		0xff007f	//color 13
#define OFF			0x000000	//color 0

typedef enum {white, red, orange, yellow, chartreuse, green, turquoise, cyan, azure, blue, violet, magenta, rose} Colors;

extern uint32_t colors_hex[14];

void set_color(uint8_t, uint32_t, uint8_t);
void send_data(bool);

static uint8_t brightness_controller = 0;
static uint16_t index=0;
static uint32_t led_data[MAX_LED]={0};
static uint16_t pwm_data[(MAX_LED*NUM_BITS)+LED_RESET]={0};
static bool dma_running = false;

#endif /* INC_LED_DRIVER_C_ */
