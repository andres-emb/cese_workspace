/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "API_uart.h"
#include "stm32f4xx_hal.h"  		/* <- HAL include */
#include "stm32f4xx_nucleo_144.h" 	/* <- BSP include */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUFFER_MAX_LEN 255
#define TX_RX_TIMEOUT_MS 100

/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef UartHandle;
static char internalBuffer[BUFFER_MAX_LEN];


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Turn on the LED2 if an error has been identified.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  while (1)
  {
  }
}

/**
  * @brief  Validate if the pointer argument is valid
  * @param  None
  * @retval None
  */
static void validateUint8Ptr(uint8_t * ptr)
{
	if (ptr == NULL) {
		Error_Handler();
	}
}

/**
  * @brief  Validate if the size is lower than the max supported one
  * @param  None
  * @retval None
  */
static void validateSize(const uint16_t size)
{
	if (size >= BUFFER_MAX_LEN) {
		Error_Handler();
	}
}

/**
  * @brief  Initialize the UART interface with fixed parameters
  * 		and send that parameters
  * @param  None
  * @retval None
  */
bool_t uartInit() {
	UartHandle.Instance        = USART3;
	UartHandle.Init.BaudRate   = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_ODD;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&UartHandle) != HAL_OK) {
		return false;
	}

	uint16_t writeBytes = 0;

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "******************************************************\r\n");
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	uartSendString((uint8_t *)"**\t UART configuration details \t**\r\n");

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "BaudRate: \t%lu\r\n", UartHandle.Init.BaudRate);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "WordLength: \t%lu\r\n", UartHandle.Init.WordLength);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "StopBits: \t%lu\r\n", UartHandle.Init.StopBits);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "Parity: \t%lu\r\n", UartHandle.Init.Parity);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "HwFlowCtl: \t%lu\r\n", UartHandle.Init.HwFlowCtl);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "Mode: \t\t%lu\r\n", UartHandle.Init.Mode);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "OverSampling: \t%lu\r\n", UartHandle.Init.OverSampling);
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	writeBytes = snprintf(internalBuffer, BUFFER_MAX_LEN, "******************************************************\r\n");
	uartSendStringSize((uint8_t *)internalBuffer, writeBytes);

	return true;
}

/**
  * @brief  Send a string using the uart interface
  * @param  pstring: pointer to the string that will be sent
  * @retval None
  */
void uartSendString(uint8_t * pstring)
{
	validateUint8Ptr(pstring);

	int stringLen = strnlen((char *)pstring, BUFFER_MAX_LEN);
	uartSendStringSize(pstring, stringLen);
}

/**
  * @brief  Send a string specifying its size using the uart interface
  * @param  pstring: pointer to the string that will be sent
  * 		size: size of the desired string
  * @retval None
  */
void uartSendStringSize(uint8_t * pstring, uint16_t size)
{

	validateUint8Ptr(pstring);
	validateSize(size);

	HAL_StatusTypeDef transmitResult = HAL_UART_Transmit(
		&UartHandle,
		pstring,
		size,
		TX_RX_TIMEOUT_MS
	);

	if (transmitResult != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief  Receives a string of the specified size using the
  * 		uart interface
  * @param  pstring: pointer to the buffer where the data will be stored
  * 		size: size of the data
  * @retval TRUE if the uart receives data within the selected timeout
  */
bool uartReceiveStringSize(uint8_t * pstring, uint16_t size) {
	validateUint8Ptr(pstring);
	validateSize(size);

	HAL_StatusTypeDef receiveResult = HAL_UART_Receive(
		&UartHandle,
		pstring,
		size,
		TX_RX_TIMEOUT_MS
	);

	return receiveResult == HAL_OK;
}
