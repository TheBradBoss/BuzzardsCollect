/*
 * lcd.h
 *
 *  Created on: Apr 22, 2024
 *      Author: bradl
 *  Notes:
 *  	Please just replace values, trying to include more header files
 *  	breaks the compiler.
 *  	Includes for lcd.c in lcd.h to avoid compiler errors.
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l496xx.h"
//#include "stm32l4xx_hal_gpio.h"	//would be useful but breaks compiler
#include <stdint.h>
#include <stdio.h>
#include <string.h>



//MUST GO AND CHANGE CLOCK ENABLE LINE IN LCD.C INIT MANUALLY
#define LCD_PORT		GPIOA
#define LCD_PORT_CLOCK	RCC_AHB2ENR_GPIOAEN
#define LCD_DATA_BITS	((uint16_t)0x000F)	//(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LCD_NUM			6
#define LCD_PINS		{0, 1, 2, 3, 4, 5}	//GPIO A pins 0 though 5
#define LCD_EN			((uint16_t)0x0010)	//GPIO_PIN_4
#define LCD_RS			((uint16_t)0x0020)	//GPIO_PIN_5
//Only ever writing to LCD so RW pin (6) stays low (is grounded)


//Commands
#define CLEAR_HOME		0x01
#define LINE_TWO		0xC0	//Move cursor to line two
#define RETURN_HOME		0x02	//Move cursor to line one

//Initialize lcd functions
void LCD_init			( void );
void LCD_pulse_ENA		( void ) ;
void LCD_4b_command		( uint8_t command ) ;
void LCD_command		( uint8_t command ) ;
void LCD_write_char		( uint8_t letter );
void LCD_write_string	( uint8_t sentence[] );



#endif /* INC_LCD_H_ */
