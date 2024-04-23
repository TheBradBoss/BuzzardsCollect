/*
 * keypad.h
 *
 *  Created on: Apr 17, 2024
 *      Author: bradl
 *
 *
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include "stm32l496xx.h"
#include <stdint.h>
//Header file for GPIO_PIN_# not included because they break compiler

#define NUM_ROWS			(4)	//Chang depending on your keypad
#define NUM_COLS			(4)
#define KEYPAD_PORT			(GPIOC)
#define KEYPAD_PORT_CLOCK	(RCC_AHB2ENR_GPIOCEN)

#define ROW_PINS	{4, 5, 6, 8}
//#define ROW_IO		(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8)
#define ROW_IO		((uint16_t)0x0170)	//(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8)
#define ROW_0		((uint16_t)0x0010)	//(GPIO_PIN_4)

#define COL_PINS	{0, 1, 2, 3}
//#define COL_IO		(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define COL_IO		((uint16_t)0x000F)	//(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define COL_0		((uint16_t)0x0001)	//GPIO_PIN_0


void 	Keypad_Config 	(void);

int 	Keypad_IsAnyKeyPressed	(void);

int 	Keypad_WhichKeyIsPressed (void);

uint16_t Keypad_CheckKeyPressed (uint8_t iKey);

#endif /* INC_KEYPAD_H_ */

