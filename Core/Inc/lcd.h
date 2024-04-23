/*
 * lcd.h
 *
 *  Created on: Apr 22, 2024
 *      Author: bradl
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l496xx.h"
//#include "stm32l4xx_hal_gpio.h"
#include <stdint.h>

//#define LCD_PORT	GPIOA
//#define LCD_DATA_BITS	(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
//#define LCD_PINS	{0, 1, 2, 3, 4, 5, 6}
//#define LCD_EN		GPIO_PIN_4
//#define LCD_RS		GPIO_PIN_5

#define LCD_PORT	GPIOA
#define LCD_DATA_BITS	((uint16_t)0x000F)	//(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LCD_PINS	{0, 1, 2, 3, 4, 5}
#define LCD_EN		((uint16_t)0x0010)	//GPIO_PIN_4
#define LCD_RS		((uint16_t)0x0020)	//GPIO_PIN_5


void LCD_init( void );
void LCD_pulse_ENA( void ) ;
void LCD_4b_command( uint8_t command ) ;
void LCD_command( uint8_t command ) ;
void LCD_write_char( uint8_t letter );



#endif /* INC_LCD_H_ */
