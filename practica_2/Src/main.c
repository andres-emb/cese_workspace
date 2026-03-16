 /**
  ******************************************************************************
  * @file    UART/UART_Printf/Src/main.c
  * @author  MCD Application Team
  * @brief   This example shows how to retarget the C library printf function
  *          to the UART.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup UART_Printf
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define DELAY_TIMER_MS 100
#define MAX_VARIABLE_DELAYS 3
/* Private variables ---------------------------------------------------------*/
/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* Private function prototypes -----------------------------------------------*/

static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

void validateDelay(delay_t * delay) {
	if (delay == NULL) {
		Error_Handler();
	}
}

void validateVariableSpec(variableSpec_t * spec) {
	if (spec == NULL) {
		Error_Handler();
	}
}

void delayInit(delay_t * delay, tick_t duration)
{
	validateDelay(delay);

	delay->startTime = 0;
	delay->duration = duration;
	delay->running = false;
}

bool_t delayRead(delay_t * delay)
{
	validateDelay(delay);

	if (!delay->running) {
		delay->running = true;
		delay->startTime = HAL_GetTick();
		return false;
	}

	tick_t currentTime = HAL_GetTick();

	if ((currentTime - delay->startTime) >= delay->duration) {
		delay->running = false;
		return true;
	}

	return false;
}

void delayWrite(delay_t * delay, tick_t duration)
{
	validateDelay(delay);

	delay->duration = duration;
}

void variableDelayInit(delay_t * delay, variableSpec_t * variableSpec, bool_t isOnState)
{
	validateDelay(delay);
	validateVariableSpec(variableSpec);

	double timeFactor = isOnState ? variableSpec->dutyCycle / 100.0 : 1 - variableSpec->dutyCycle / 100.0;
	tick_t duration = variableSpec->periodMs * timeFactor;
	delayInit(delay, duration);
}

uint32_t getNextVariableDelayIdx(uint32_t currentIdx)
{
	return (++currentIdx < MAX_VARIABLE_DELAYS) ? currentIdx : 0;
}

bool_t shouldUpdateVariableSpec(uint32_t currentIterations, uint32_t maxIterations)
{
	return currentIterations >= maxIterations;
}

void normalizeVariableSpecIdx(uint32_t * currentSpecIdx)
{
	if (*currentSpecIdx >= MAX_VARIABLE_DELAYS) {
		*currentSpecIdx = 0;
	}
}

int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 180 MHz */
  SystemClock_Config();

  /* Initialize BSP Led for LED1 and LED2 */
  BSP_LED_Init(LED1); // Used for fixed delay
  BSP_LED_Init(LED2); // Used for variable delay

  delay_t delay;
  delayInit(&delay, DELAY_TIMER_MS);

  delay_t variableDelay;
  variableSpec_t variableSpec[MAX_VARIABLE_DELAYS] = {
  	{1000, 50, 5},
  	{200, 50, 5},
  	{100, 50 ,5},
  };

  BSP_LED_On(LED2);
  bool_t isOnState = true;
  uint32_t currentVariableSpecIdx = 0;
  uint32_t variableSpecIterations = 0;

  variableSpec_t * currentVariableSpec = variableSpec;
  variableDelayInit(&variableDelay, currentVariableSpec, isOnState);

  /* Infinite loop */
  while (1) {

	  if (delayRead(&delay)) {
		  BSP_LED_Toggle(LED1);
		  delayInit(&delay, DELAY_TIMER_MS);
	  }

	  if (delayRead(&variableDelay)) {
		  BSP_LED_Toggle(LED2);
		  isOnState = !isOnState;

		  if (isOnState) {
			  // Cycle complete has been detected
			  variableSpecIterations++;
		  }

		  if (shouldUpdateVariableSpec(variableSpecIterations, currentVariableSpec->repetitions)) {
			  currentVariableSpecIdx++;
			  normalizeVariableSpecIdx(&currentVariableSpecIdx);
			  currentVariableSpec = &variableSpec[currentVariableSpecIdx];
			  variableSpecIterations = 0;
		  }

		  variableDelayInit(&variableDelay, currentVariableSpec, isOnState);
	  }
  }
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED3);
  while (1)
  {
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
