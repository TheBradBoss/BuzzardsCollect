/*
 * keypad.c
 *
 *  Created on: Apr 17, 2024
 *      Author: bradl
 */

#include "keypad.h"

#include <stdint.h>
#include <stdio.h>
#include "stm32l496xx.h"


//Initialize functions
uint16_t Keypad_CheckKeyPressed (uint8_t iKey);

void 	Keypad_Config (void);

int 	Keypad_IsAnyKeyPressed (void);

int Keypad_WhichKeyIsPressed(void);

uint8_t debounce_state = 0;

void Keypad_Config (void) {//(GPIO_TypeDef *port, uint32_t *pins, uint32_t count, char moder, char otype, char ospeed, char pupdr) {
	//pins must be an array with pin numbers.
	//count(number of pins being used) must be entered manually
	//DOES NOT ENABLE PERIPHERAL CLOCK
	//To use:
	//uint32_t LED_pins[] = {0, 1, 2, 3};
	//Configure_GPIO_Pins(GPIOC, LED_pins, 4, 0x1 /*mode*/, 0x0 /*otype*/, 0x3 /*ospeed*/, 0x0 /*pupdr*/);

	RCC->AHB2ENR |= (KEYPAD_PORT_CLOCK);	//CHANGE ME PLEASE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	uint32_t col_pins[] = COL_PINS;
	//Set columns to outputs
	for (uint32_t i = 0; i < NUM_COLS; i++) {
		uint32_t pin = col_pins[i];
		KEYPAD_PORT->MODER &= ~(0x3 << (pin * 2));
		KEYPAD_PORT->MODER |= (0x1 << (pin * 2));
		KEYPAD_PORT->OTYPER &= ~(0x1 << pin);
		KEYPAD_PORT->OTYPER |= (0x0 << pin);
		KEYPAD_PORT->OSPEEDR &= ~(0x3 << (pin * 2));
		KEYPAD_PORT->OSPEEDR |= (0x3 << (pin * 2));
		KEYPAD_PORT->PUPDR &= ~(0x3 << (pin * 2));
		KEYPAD_PORT->PUPDR |= (0x0 << (pin * 2));
	}

	uint32_t row_pins[] = ROW_PINS;
	//Set rows to inputs
	for (uint32_t i = 0; i < NUM_ROWS; i++) {
		uint32_t pin = row_pins[i];
		KEYPAD_PORT->MODER &= ~(0x3 << (pin * 2));
		KEYPAD_PORT->MODER |= (0x0 << (pin * 2));
		KEYPAD_PORT->OTYPER &= ~(0x1 << pin);
		KEYPAD_PORT->OTYPER |= (0x0 << pin);
		KEYPAD_PORT->OSPEEDR &= ~(0x3 << (pin * 2));
		KEYPAD_PORT->OSPEEDR |= (0x3 << (pin * 2));
		KEYPAD_PORT->PUPDR &= ~(0x3 << (pin * 2));
		KEYPAD_PORT->PUPDR |= (0x2 << (pin * 2));
	}
}


// -----------------------------------------------------------------------------
int Keypad_IsAnyKeyPressed(void) {
// drive all COLUMNS HI; see if any ROWS are HI
// return true if a key is pressed, false if not
// currently no debounce here - just looking for a key twitch
		uint16_t SETTLE = 1000;



	if ( debounce_state == 0 ) {
	   KEYPAD_PORT->BSRR = COL_IO;         	      // set all columns HI
	   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let it settle
		  ;
	   if ((KEYPAD_PORT->IDR & ROW_IO) != 0 ) {        // got a keypress!
		   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let the debouncing settle
		   		  ;	//do nothing
		   if ((KEYPAD_PORT->IDR & ROW_IO) != 0 ) {	//still reading a press
			   debounce_state = 1;
			   return( 1 );
		   }
	   }
	   else
		  return( 0 );                          // nope.
	}

	if (debounce_state == 1 ) {
	   KEYPAD_PORT->BSRR = COL_IO;         	      // set all columns HI
	   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let it settle
		  ;
	   if ((KEYPAD_PORT->IDR & ROW_IO) == 0 ) {		// key unpressed!
		   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let the debouncing settle
		   		  ;	//do nothing
		   if ((KEYPAD_PORT->IDR & ROW_IO) == 0 ) {	//still reading unpressed key
			   debounce_state = 0;
			   return( 0 );
		   }
	   }
	}
	else
		return( 1 );
}


int Keypad_WhichKeyIsPressed(void) {
// detect and encode a pressed key at {row,col}
// assumes a previous call to Keypad_IsAnyKeyPressed() returned TRUE
// verifies the Keypad_IsAnyKeyPressed() result (no debounce here),
// determines which key is pressed and returns the encoded key ID

   int8_t iRow=0, iCol=0, iKey=0;  // keypad row & col index, key ID result
   int8_t bGotKey = 0;             // bool for keypress, 0 = no press
   uint16_t SETTLE = 1000;


   uint32_t col_pins[] = COL_PINS; //{0, 1, 2, 3}
   uint32_t row_pins[] = ROW_PINS; //{4, 5, 6, 8}
   for ( iCol = 0; iCol < NUM_COLS; iCol++ ) {
	   uint32_t pinC = col_pins[iCol];
	   KEYPAD_PORT->ODR = (0x1 << pinC);
	   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let it settle
	   	         ;
	   for( iRow = 0; iRow < NUM_ROWS; iRow++ ){
		   uint32_t pinR = row_pins[iRow];
		   if ( KEYPAD_PORT->IDR & (0x1 << pinR) ) {
			   bGotKey = 1;
			   break;
		   }
	   }
	   if ( bGotKey )
		   break;
   }
   if ( bGotKey ) {
      iKey = ( iRow * NUM_COLS ) + iCol + 1;  // Gives pad a location 1-16
      uint16_t LED_out = Keypad_CheckKeyPressed(iKey);
 	return( LED_out );                         // return encoded keypress
   }
   return( -1 );                     // unable to verify keypress

}

uint16_t Keypad_CheckKeyPressed (uint8_t iKey){ //outputs what LEDs to turn on
	uint16_t var;

	switch (iKey) {
	  case 0x01: //Column 1, Row 1
		  var = (1);
		  break;
	  case 0x02: //Column 2, Row 1
		  var = (2);
		  break;

	  case 0x03: //Column 3, Row 1
		  var = (3);
		  break;

	  case 0x04: //Column 1, Row 2
		  var = (10);
		  break;

	  case 0x05: //Column 2, Row 2
		  var = (4);
		  break;

	  case 0x06: //Column 3, Row 2
		  var = (5);
		  break;

	  case 0x07: //Column 1, Row 3
		  var = (6);
		  break;

	  case 0x08: //Column 2, Row 3
		  var = (11); //B
		  break;

	  case 0x09: //Column 2, Row 3
		  var = (7);
		  break;

	  case 0x0A: //Column 1, Row 4
		  var = (8);
		  break;

	  case 0x0B: //Column 2, Row 4
		  var = (9);
		  break;

	  case 0x0C: //Column 3, Row 4
		  var = (12);
		  break;

	  case 0x0D: //Column 3, Row 4
		  var = (14);
		  break;

	  case 0x0E: //Column 3, Row 4
		  var = (0);
		  break;

	  case 0x0F: //Column 3, Row 4
		  var = (15);
		  break;

	  case 0x10: //Column 3, Row 4
		  var = (13);
		  break;

	  default:
		  var = (15); //period for no press
		  break;

	}

	return var;
}
