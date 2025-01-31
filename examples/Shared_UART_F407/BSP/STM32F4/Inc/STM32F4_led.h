/******************************************************************************
 * @file    	STM32F4_led.h
 *
 * @Description Header file of STM32F4_led.c
 *
 *
 * @date    	2024-04-25
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/

#ifndef STM32F4_LED_H
#define STM32F4_LED_H



#include "boardConfig.h"

typedef enum{
	GREEN=12,
	ORANGE,
	RED,
	BLUE,

}LedColor;


void leds_init(void);

void led_on(LedColor);
void led_off(LedColor);
void led_toggle(LedColor);



#endif
