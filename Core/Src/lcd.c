/*
 * lcd.c
 *
 *  Created on: Apr 22, 2024
 *      Author: bradl
 */





#include "lcd.h"
#include "delay.h"


#include "stm32l496xx.h"


//#include <stdint.h>
//#include <stdio.h>


//#define LCD_PORT	GPIOA
//#define LCD_DATA_BITS	(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
//#define LCD_PINS	{0, 1, 2, 3, 4, 5, 6}
//#define LCD_EN		GPIO_PIN_4
//#define LCD_RS		GPIO_PIN_5


//Only ever writing to LCD so RW pin (6) stays low

//Code given only work if GPIO LCD DB pins are 0,1,2,3

// ------------------------------------------------------ excerpt from lcd.c ---
void LCD_init( void )  {   // RCC & GPIO config removed - leverage A1, A2 code

	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);	//CHANGE ME PLEASE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	uint32_t lcd_pins[] = LCD_PINS;
	//Set columns to outputs
	for (uint32_t i = 0; i < 6; i++) {
		uint32_t pin = lcd_pins[i];
		LCD_PORT->MODER &= ~(0x3 << (pin * 2));
		LCD_PORT->MODER |= (0x1 << (pin * 2));	//Output 01
		LCD_PORT->OTYPER &= ~(0x1 << pin);
		LCD_PORT->OTYPER |= (0x0 << pin);		//0
		LCD_PORT->OSPEEDR &= ~(0x3 << (pin * 2));
		LCD_PORT->OSPEEDR |= (0x3 << (pin * 2));//Highest speed
		LCD_PORT->PUPDR &= ~(0x3 << (pin * 2));
		LCD_PORT->PUPDR |= (0x2 << (pin * 2));	//push/pull and pull down
		LCD_PORT->BRR |= (0x1 << (pin * 1));	//initialize off
	}

	delay_us( 40000 );                     // power-up wait 40 ms
   for ( int idx = 0; idx < 3; idx++ ) {  // wake up 1,2,3: DATA = 0011 XXXX
      LCD_4b_command( 0x30 );             // HI 4b of 8b cmd, low nibble = X
      delay_us( 200 );
   }
   LCD_4b_command( 0x20 ); // fcn set #4: 4b cmd set 4b mode - next 0x28:2-line
   delay_us( 40 );         // remainder of LCD init removed - see LCD datasheets
   LCD_command( 0x28 );
   LCD_command( 0x10 );
   LCD_command( 0x0F );
   LCD_command( 0x06 );

   //LCD_PORT->ODR |= 0x003F;
}


void LCD_pulse_ENA( void )  {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
   LCD_PORT->ODR   |= ( LCD_EN );         	// ENABLE = HI
   delay_us( 50 );                         // TDDR > 320 ns
   LCD_PORT->ODR   &= ~( LCD_EN );        // ENABLE = LOW
   delay_us( 50 );                         // low values flakey, see A3:p.1
}

void LCD_4b_command( uint8_t command )  {
// LCD command using high nibble only - used for 'wake-up' 0x30 commands
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS ); 	// clear DATA bits
   LCD_PORT->ODR   |= ( command >> 4 );   // DATA = command
   delay_us( 50 );
   LCD_pulse_ENA( );
}

void LCD_command( uint8_t command )  {
// send command to LCD in 4-bit instruction mode
// HIGH nibble then LOW nibble, timing sensitive
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( (command>>4) & LCD_DATA_BITS ); // HIGH shifted low
   delay_us( 50 );

   LCD_pulse_ENA( );                                    // latch HIGH NIBBLE

   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( command & LCD_DATA_BITS );      // LOW nibble
   delay_us( 50 );
   LCD_pulse_ENA( );                                    // latch LOW NIBBLE
}

void LCD_write_char( uint8_t letter )  { //0x41 is A
// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
   LCD_PORT->ODR   |= (LCD_RS);       // RS = HI for data to address
   delay_us( 50 );
   LCD_command( letter );             // character to print
   LCD_PORT->ODR   &= ~(LCD_RS);      // RS = LO
}


