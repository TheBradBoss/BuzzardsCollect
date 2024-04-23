/* USER CODE BEGIN Header */
/**
 *
 *
 *
 * Sources:
 * How would I extract each character from a string as a character literal to write to an LCD with an STM32 MCU board and c programming
 * (ChatGPT, 4/23/2024)
 *
 *
 *
 *
 *
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "delay.h"
#include "lcd.h"
#include "keypad.h"


//#include <stdio.h>
#include <stdint.h>


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void MX_GPIO_Init(void);


int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize LCD pins and LCD for 4-bit operation */
  LCD_init();
  Keypad_Config();

  /* Initialize LED pins (Port D, Pins 0-3) */
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN);						//Enable clock for each peripheral	//RCC->AHB2ENR |= 1; //another option
	GPIOD->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);		//Clears mode
	GPIOD->MODER   |=  (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0);	//Sets mode to 01
	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);		//Sets type to 0 (push/pull)
	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);		//Sets to 00(No PU or PD)
	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |			//Very High Speed
					  (3 << GPIO_OSPEEDR_OSPEED1_Pos));
	GPIOD->BRR = (GPIO_PIN_0 | GPIO_PIN_1); // preset PC0, PC1 to 0

	// configure GPIO pins PC2, PC3 for:
	// output mode, push-pull, no pull up or pull down, high speed
	GPIOD->MODER   &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOD->MODER   |=  (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
	GPIOD->OTYPER  &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
	GPIOD->PUPDR   &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	GPIOD->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED2_Pos) |
					  (3 << GPIO_OSPEEDR_OSPEED3_Pos));
	GPIOD->BRR = (GPIO_PIN_2 | GPIO_PIN_3); // preset PC2, PC3 to 0

	///////////////////////////////////////////
//	//Clear display sometimes breaks other commands or will not let you write to first position on line 1
//	LCD_command(CLEAR_HOME);	//BE CAREFUL
//	delay_us( 1000 );	//include delay after whenever clear home is called
	///////////////////////////////////////////


	//////////////////////////////
//	//Temporary code to replicate CLEAR_HOME command (if desired)
//	LCD_command(RETURN_HOME);
//	LCD_write_string("                ");
//	LCD_command(LINE_TWO);
//	LCD_write_string("                ");
//	LCD_command(RETURN_HOME);
	///////////////////////////////


//	LCD_command(0x02);	//Returns cursor to start
//	LCD_write_char( 'B' ); //
//	LCD_write_char( 'R' ); //A
//	LCD_write_char( 'A' ); //A
//	LCD_write_char( 'D' ); //A
//	LCD_write_string( "California Burrito" );
//	LCD_command(LINE_TWO);		//Moves Cursor to the second line
//	LCD_write_char( 'K' );
//	//LCD_command(CLEAR_HOME);	//clears display, moves cursor to start


	LCD_command(CLEAR_HOME);	//0x01
	delay_us (1000);
	LCD_write_string("EE 329 A3 Timer");;
	LCD_command(LINE_TWO);
	LCD_write_string("*=SET #=GO 00:00");


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if(Keypad_IsAnyKeyPressed() == 1){
		  //GPIOD->BSRR = 0x0000000F; //(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
		  for(int i = 0; i < 1000; i++){
				  //do nothing
			  }
		  int red = Keypad_WhichKeyIsPressed();
		  if (red != -1)
			  GPIOD->ODR = red;
		  int goal = KEYPAD_PORT->IDR;
		  printf("Returning 1 %d \n", goal);
		  printf("Got key %d \n", red);

	  }
	  else{
		  //GPIOD->BRR = 0x0000000F;//(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
		  for(int i = 0; i < 1000; i++){
		  				  //do nothing
		  	  }

		  int bad = KEYPAD_PORT->IDR;
		  printf("Returning 0 %d \n", bad);
	  }




//	  for ( uint16_t i = 0; i < 15; i++) {	//Make LEDs constantly count to show while reached
//		  GPIOD->ODR &= 0x0;
//		  GPIOD->ODR |= i;
//		  delay_us( 200000 );
//	  }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
