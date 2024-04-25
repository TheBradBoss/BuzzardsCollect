
/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE 329 A3 Parallel LCD
 *******************************************************************************
 * @file           : main.c
 * @brief          : Use keypad to enter and display countdown on Parallel LCD
 * project         : EE 329 S'24 Assignment 3
 * authors         : Bradley Buzzini - Logan Tom
 * version         : 0.2
 * date            : 240424
 * compiler        : STM32CubeIDE v.1.15.0 Build: 20695_20240315_1429 (UTC)
 * target          : NUCLEO-L496ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2024 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * REVISION HISTORY
 * 0.1 240422 BB  Printed first character, no keypad
 * 0.2 240424 BB  Includes requirements for A3
 *******************************************************************************
 * TODO
 *
 *******************************************************************************
 * 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-234567 */
/* USER CODE END Header */

/* Includes */
#include "main.h"
#include "delay.h"
#include "lcd.h"
#include "keypad.h"


//#include <stdio.h>
#include <stdint.h>
#include <stdio.h>


/* Function initialization */
void SystemClock_Config(void);


int main( void )
{
	/* Initialize */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize Keypad and LCD for 4-bit operation */
	LCD_init();
	Keypad_Config();

	// Initialize LED pins (GPIO Port D, Pins 0-3) for:
	// output mode, push-pull, no pull up or pull down, high speed
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);
	GPIOD->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);
	GPIOD->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0);
	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);
	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);
	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED1_Pos));
	GPIOD->BRR = (GPIO_PIN_0 | GPIO_PIN_1); // preset PD0, PD1 to 0

	GPIOD->MODER   &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOD->MODER   |=  (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED2_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED3_Pos));
	GPIOD->BRR = (GPIO_PIN_2 | GPIO_PIN_3); // preset PD2, PD3 to 0


	uint8_t complete = 1;	// LCD reset state (skips while loops when true)

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

		if (complete == 1) {
			// Initialize prompt at beginning or reset triggered.
			GPIOD->ODR = 0x00;				// LEDs off
			LCD_command( 0x10 );			// Shift cursor to the left
			LCD_command( CURSOR_ON );		// Display, cursor, position on
			LCD_command( CURSOR_RIGHT );	// Cursor moves right, no shift
			LCD_command(CLEAR_HOME);
			delay_us (1000);
			LCD_write_string("EE 329 A3 Timer");
			LCD_command(LINE_TWO);
			LCD_write_string("*=SET #=GO 00:00");
			complete = 0;
		}

		if( (Keypad_IsAnyKeyPressed() == 1) ) {
			if ( Keypad_WhichKeyIsPressed() == '*' ) {
				// Begin waiting for count
				LCD_command(0xCB);		// Cursor to first M in MM:SS
				uint8_t gotCount = 1;	// Clear when 4 keys entered
				uint8_t Tpoint = 0;		// Which point  in setting the timer
				uint32_t countdown = 0;	// countdown saved as decimal: MMSS

				while ( gotCount ){
					if( (Keypad_IsAnyKeyPressed() == 1) ) {
						// Record key pressed
						uint8_t first = Keypad_WhichKeyIsPressed();
						if ( ( first  <= '9') && ( first  >= '0') ) {
							// Valid key?
							if ( (Tpoint % 2 == 0) && ( first  > '5')) {
								// Does digit need limited?
								LCD_write_char( '5' );
								countdown = countdown * 10;	// Move M/S left
								countdown += ('5' - '0');	// Add M/S to countdown
							}
							else {
								LCD_write_char( first );	// Save key pressed
								countdown = countdown * 10;
								countdown += ( first - '0' );
							}
							Tpoint++;
							if ( Tpoint == 2 )	LCD_write_char(':');
							// Add colon in between M/S
							if ( Tpoint == 4 )	gotCount = 0;
							// Four digits entered
						}
						else if ( first == '*' ) {			// Reset triggered?
							complete = 1;
							break;
						}
					}

				}

				GPIOD->ODR = 0x03;	// LEDs show timer is ready to start

				//Convert countdown (MMSS) in terms of 50ms
				uint32_t totalIterations = 0;
				totalIterations = (((countdown/100)*60)+(countdown % 100))*20;

				while( 1 && (complete != 1)) { // Do nothing and wait for '#' press
					if ( Keypad_IsAnyKeyPressed() == 1 ){
						uint8_t  second  = Keypad_WhichKeyIsPressed();
						if (  second  == '#' ) {
							break;
						}
						else if (  second  == '*' ) {		// Reset triggered?
							complete = 1;
							break;
						}
					}
				}

				while ( (totalIterations != 0) && (complete != 1) ) {
					// Count down begins
					totalIterations --;
					delay_us(48000);	// ~50ms (includes function delays)
					if ((totalIterations % 20) == 0) {
						// Every second, write new time to LCD
						LCD_write_time(totalIterations / 20);

					}
					if ( (Keypad_IsAnyKeyPressed() == 1) ) {// Reset triggered?
						if ( Keypad_WhichKeyIsPressed() == '*') {
							complete = 1;
							break;
						}
					}
				}
				complete = 1;	// Countdown reached its end, reset
			}
		}

	}

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

int _write(int file, char *ptr, int len)
{
	(void)file;
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
	return len;
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */



















/* Past iterations of countdown in main()
 * 		// if (complete == 1) {
		//		  GPIOD->ODR = 0x00;
		//	   LCD_command( 0x10 );		//Shift cursor to the left
		//	   LCD_command( CURSOR_ON );		//Display, cursor, cursor position on
		//	   LCD_command( CURSOR_RIGHT );		//Cursor moves right, no shift
		//		LCD_command(CLEAR_HOME);	//0x01
		//		delay_us (1000);
		//		LCD_write_string("EE 329 A3 Timer");
		//		LCD_command(LINE_TWO);
		//		LCD_write_string("*=SET #=GO 00:00");
		//		complete = 0;
		//	  }
		//	  if( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '*') ) {
		//			  //SET has been pressed
		//			  //GPIOD->ODR = 0x01;
		//			  LCD_command(0xCB);	//First M in MM:SS
		//			  uint8_t gotCount = 1;	//Clear when 4 keys entered
		//			  uint8_t Tpoint = 0;	//Which point are we in setting the timer
		//			  uint32_t countdown = 0;	//countdown as MMSS
		//			  while ( gotCount ){
		//				if( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() < 0x40)) {	//Key must be a number
		//					if ( (Tpoint % 2 == 0) && (Keypad_WhichKeyIsPressed() > 0x35)) {
		//						LCD_write_char( '5' );
		//						countdown = countdown * 10;	//Move number left
		//						countdown += ('5' - '0');	//Add number
		//					}
		//					else {
		//						LCD_write_char( Keypad_WhichKeyIsPressed() );
		//						countdown = countdown * 10;
		//						countdown += ( Keypad_WhichKeyIsPressed() - '0' );
		//					}
		//					  Tpoint++;
		//					  if ( Tpoint == 2 )	LCD_write_char(':');
		//					  if ( Tpoint == 4 )	gotCount = 0;
		//				  }
		//				else if ( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '*') ) {
		//					complete = 1;
		//					break;
		//				}
		//			  }
		//			  GPIOD->ODR = 0x03;
		//			  uint32_t totalIterations = 0;
		//			  totalIterations = (((countdown / 100) * 60) + (countdown % 100)) * 20;
		//			  //Convert countdown (MMSS) in terms of 50ms
		//			  printf("Converted integer value: %d\n", countdown);
		//			  while( 1 && (complete != 1)) {
		//				  //Do nothing and wait for '#' press
		//				  if ( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '#') ){
		//					  break;
		//				  }
		//					else if ( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '*') ) {
		//						complete = 1;
		//						break;
		//				  }
		//			  }
		//			  //Count down begins
		//			  while ( (totalIterations != 0) && (complete != 1) ) {
		//				  totalIterations --;
		//				  delay_us(48000);	//50ms
		//				  if ((totalIterations % 20) == 0) {
		//					  LCD_write_time(totalIterations / 20);
		//					  //Writes seconds left to LED as MM:SS
		//				  }
		//				  else if ( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '*') ) {
		//						complete = 1;
		//						break;
		//				  }
		//			  }
		//			  complete = 1;
		//	  }




















		//	  if( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '*') ) {
		//			  GPIOD->ODR = 0x01;
		//			  LCD_command(0xCB);	//First M in MM:SS
		//			  uint8_t getCount = 1;	//Clear when 4 keys entered
		//			  uint8_t Tpoint = 0;	//Which point are we in setting the timer
		//			  uint32_t countdown = 0;	//Countdown entered
		//			  while ( getCount ){
		//				if( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() < 0x40)) {	//Key must be a number
		//					if ( (Tpoint % 2 == 0) && (Keypad_WhichKeyIsPressed() > 0x35)) {
		//						LCD_write_char( '5' );
		//						countdown = countdown * 10;	//Move number left
		//						countdown += ('5' - '0');	//Add number
		//					}
		//					else {
		//						LCD_write_char( Keypad_WhichKeyIsPressed() );
		//						countdown = countdown * 10;
		//						countdown += ( Keypad_WhichKeyIsPressed() - '0' );
		//					}
		//					  Tpoint++;
		//					  if ( Tpoint == 2 )	LCD_write_char(':');
		//					  if ( Tpoint == 4 )	getCount = 0;
		//
		//				  }
		//
		//			  }
		//			  GPIOD->ODR = 0x03;
		//			  uint32_t totalIterations = 0;
		//			  totalIterations = (((countdown / 100) * 60) + (countdown % 100)) * 20;
		//			  //Convert countdown in terms of 50ms
		//			  printf("Converted integer value: %d\n", countdown);
		//
		//			  while( 1 ) {
		//				  //Do nothing and wait for '#' press
		//				  if ( (Keypad_IsAnyKeyPressed() == 1) && (Keypad_WhichKeyIsPressed() == '#') ){
		//					  break;
		//				  }
		//			  }
		//
		//			  //Count down begins
		//			  while ( totalIterations != 0 ) {
		//				  totalIterations --;
		//				  delay_us(50000);	//50ms
		//				  if ((totalIterations % 20) == 0) {
		//					  LCD_write_time(totalIterations / 20);
		//				  }
		//
		//			  }
		//
		//	  }











		//Code that only writes countdown without 59:59 bound.
		//	  if( Keypad_IsAnyKeyPressed() == 1 ) {
		//		  if( Keypad_WhichKeyIsPressed() == '*' ){
		//			  GPIOD->ODR = 0x01;
		//			  LCD_command(0xCB);	//First M in MM:SS
		//			  uint8_t getout = 1;
		//			  uint8_t Tpoint = 0;
		//			  while ( getout ){
		//					if( Keypad_IsAnyKeyPressed() == 1 ) {
		//					  if( Keypad_WhichKeyIsPressed() < 0x40 ) {
		//						  LCD_write_char( Keypad_WhichKeyIsPressed());
		//						  Tpoint++;
		//						  if ( Tpoint == 2 )	LCD_write_char(':');
		//						  if ( Tpoint == 4 )	getout = 0;
		//
		//					  }
		//				  }
		//			  }
		//			  GPIOD->ODR = 0x03;
		//
		//
		//		  }
		//	  }


		//	  if( Keypad_IsAnyKeyPressed() == 1 ) {
		//		  if( Keypad_WhichKeyIsPressed() < 0x40 ) {
		//			  LCD_commant()
		//			  printf("Recognized as number \n");
		//			  //delay_us (100000);
		//		  }
		//		  else {
		//			  printf("Not a number \n");
		//			  //delay_us (100000);
		//		  }
		//
		//	  }
		//	  printf("Nothing happening \n");
		//	  delay_us (100000);








		//	  if(Keypad_IsAnyKeyPressed() == 1){
		//		  //GPIOD->BSRR = 0x0000000F; //(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
		//		  for(int i = 0; i < 1000; i++){
		//				  //do nothing
		//			  }
		//		  int red = Keypad_WhichKeyIsPressed();
		//		  if (red != -1)
		//			  GPIOD->ODR = red;
		//		  int goal = KEYPAD_PORT->IDR;
		//		  printf("Returning 1 %d \n", goal);
		//		  printf("Got key %d \n", red);
		//
		//	  }
		//	  else{
		//		  //GPIOD->BRR = 0x0000000F;//(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
		//		  for(int i = 0; i < 1000; i++){
		//		  				  //do nothing
		//		  	  }
		//
		//		  int bad = KEYPAD_PORT->IDR;
		//		  printf("Returning 0 %d \n", bad);
		//	  }
 */










//Iteration 1:
//#include "main.h"
//#include "delay.h"
//#include "lcd.h"
//
//
////#include <stdio.h>
//#include <stdint.h>
//
//
///* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
////static void MX_GPIO_Init(void);
//
//
//int main(void)
//{
//	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//  HAL_Init();
//
//  /* Configure the system clock */
//  SystemClock_Config();
//  LCD_init();
//
//	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);						//Enable clock for each peripheral	//RCC->AHB2ENR |= 1; //another option
//	GPIOD->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);		//Clears mode
//	GPIOD->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0);	//Sets mode to 01
//	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);		//Sets type to 0 (push/pull)
//	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);		//Sets to 00(No PU or PD)
//	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |			//Very High Speed
//					  (3 << GPIO_OSPEEDR_OSPEED1_Pos));
//	GPIOD->BRR = (GPIO_PIN_0 | GPIO_PIN_1); // preset PC0, PC1 to 0
//
//	// configure GPIO pins PC2, PC3 for:
//	// output mode, push-pull, no pull up or pull down, high speed
//	GPIOD->MODER   &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
//	GPIOD->MODER   |=  (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
//	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
//	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
//	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED2_Pos) |
//					  (3 << GPIO_OSPEEDR_OSPEED3_Pos));
//	GPIOD->BRR = (GPIO_PIN_2 | GPIO_PIN_3); // preset PC2, PC3 to 0
//
//
//
////	LCD_command(0x01);	//Clears display DOES NOT WORK AS FIRST COMMAND?
//	LCD_command(0x02);	//Returns cursor to start
//
//
//	LCD_write_char( 'B' ); //
//	LCD_write_char( 'R' ); //A
//	LCD_write_char( 'A' ); //A
//	LCD_write_char( 'D' ); //A
//	LCD_write_string( "California Burrito" );
//	LCD_command(0xC0);		//Moves Cursor to the second line
//	LCD_write_char( 'K' );
//
//	LCD_command(CLEAR_HOME);	//clears display
//  /* Infinite loop */
//  /* USER CODE BEGIN WHILE */
//  while (1)
//  {
//    /* USER CODE END WHILE */
//	  for ( uint16_t i = 0; i < 15; i++) {
//		  GPIOD->ODR &= 0x0;
//		  GPIOD->ODR |= i;
//		  delay_us( 200000 );
////		  for (int wait = 0; wait < 200000; wait ++) {
////			  //do nothing
////		  }
//	  }
//	  //LCD_write_char( 0x41 ); //A
//    /* USER CODE BEGIN 3 */
//  }
//  /* USER CODE END 3 */
//}
//
///**
//  * @brief System Clock Configuration
//  * @retval None
//  */
//void SystemClock_Config(void)
//{
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//  /** Configure the main internal regulator output voltage
//  */
//  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Initializes the RCC Oscillators according to the specified parameters
//  * in the RCC_OscInitTypeDef structure.
//  */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
//  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
//  RCC_OscInitStruct.MSICalibrationValue = 0;
//  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Initializes the CPU, AHB and APB buses clocks
//  */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}
//
///**
//  * @brief GPIO Initialization Function
//  * @param None
//  * @retval None
//  */
//
///* USER CODE BEGIN 4 */
//
///* USER CODE END 4 */
//
///**
//  * @brief  This function is executed in case of error occurrence.
//  * @retval None
//  */
//void Error_Handler(void)
//{
//  /* USER CODE BEGIN Error_Handler_Debug */
//  /* User can add his own implementation to report the HAL error return state */
//  __disable_irq();
//  while (1)
//  {
//  }
//  /* USER CODE END Error_Handler_Debug */
//}
//
//int _write(int file, char *ptr, int len)
//{
//  (void)file;
//  int DataIdx;
//
//  for (DataIdx = 0; DataIdx < len; DataIdx++)
//  {
//    ITM_SendChar(*ptr++);
//  }
//  return len;
//}
//
//#ifdef  USE_FULL_ASSERT
///**
//  * @brief  Reports the name of the source file and the source line number
//  *         where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t *file, uint32_t line)
//{
//  /* USER CODE BEGIN 6 */
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//  /* USER CODE END 6 */
//}
//#endif /* USE_FULL_ASSERT */
