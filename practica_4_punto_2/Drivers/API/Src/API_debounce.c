#include "API_debounce.h"
#include "API_delay.h"
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */

#define DEBOUNCE_TIME_MS 40

typedef enum {
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING
} debounceState_t;


static debounceState_t debounceState;
static bool_t buttonHasBeenPressed;
static delay_t debounceDelay;

void debounceFSM_init(void) {
	debounceState = BUTTON_UP;
	buttonHasBeenPressed = false;
	delayInit(&debounceDelay, DEBOUNCE_TIME_MS);
}

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

bool_t readKey() {
	bool_t result = buttonHasBeenPressed ? true : false;
	buttonHasBeenPressed = false;
	return result;
}
