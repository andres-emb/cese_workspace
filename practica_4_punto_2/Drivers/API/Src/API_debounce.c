/**
  ******************************************************************************
  * @file    Drivers/API/Src/main.c
  * @author  Andres Urian Florez
  * @brief   This example shows how to modularize the debounce opeartion of a
  * 		 button using a FSM-
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
#include "API_debounce.h"
#include "API_delay.h"
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING
} debounceState_t;

/* Private macro -------------------------------------------------------------*/
#define DEBOUNCE_TIME_MS 40

/* Private variables ---------------------------------------------------------*/
static debounceState_t debounceState;
static bool_t buttonHasBeenPressed;
static delay_t debounceDelay;

/**
  * @brief  Initialize a FSM to handle the debounce operation
  * @param  None
  * @retval None
  */
void debounceFSM_init(void) {
	debounceState = BUTTON_UP;
	buttonHasBeenPressed = false;
	delayInit(&debounceDelay, DEBOUNCE_TIME_MS);
}

/**
  * @brief  Updates the FSM according to the current state and the current state of
  * 		the button GPIO
  * @param  None
  * @retval None
  */
void debounceFSM_update(void) {

	GPIO_PinState buttonState = BSP_PB_GetState(BUTTON_USER);

	switch (debounceState) {
	case BUTTON_UP:
		if (buttonState == GPIO_PIN_SET) {
			debounceState = BUTTON_FALLING;
		}
		break;
	case BUTTON_FALLING:
		if (delayRead(&debounceDelay)) {
			if (buttonState == GPIO_PIN_SET) {
				debounceState = BUTTON_DOWN;
				buttonHasBeenPressed = true;
			} else {
				debounceState = BUTTON_UP;
			}
		}
		break;
	case BUTTON_DOWN:
		if (buttonState == GPIO_PIN_RESET) {
			debounceState = BUTTON_RISING;
		}
		break;
	case BUTTON_RISING:
		if (delayRead(&debounceDelay)) {
			if (buttonState == GPIO_PIN_RESET) {
				debounceState = BUTTON_UP;
			} else {
				debounceState = BUTTON_DOWN;
			}
		}
		break;
	default:
		debounceFSM_init();
		break;
	};
}

/**
  * @brief  Check if the button has been pressed and reset the private variable that
  *  		tracks this operation
  * @param  None
  * @retval True if the button has been pressed
  */
bool_t readKey() {
	bool_t result = buttonHasBeenPressed ? true : false;
	buttonHasBeenPressed = false;
	return result;
}
