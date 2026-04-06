/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "API_cmdparser.h"
#include "API_uart.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"

/* Private typedef -----------------------------------------------------------*/

/* Store the cmdParser state */
typedef enum {
	CMD_IDLE,
	CMD_RECEIVING,
	CMD_PROCESS,
	CMD_EXEC,
	CMD_ERROR
} cmdParserState_t;

/* Store the action identified by the cmdParser*/
typedef enum {
	UNKOWN,
	IGNORE,
	HELP,
	STATUS,
	CONTROL_LED
} cmdAction_t;

/* Store the tokens identified by the cmdParser */
typedef struct {
	cmdAction_t action;
	uint8_t arg1[CMD_MAX_LINE];
	uint8_t arg2[CMD_MAX_LINE];
	cmdStatus_t status;
} cmdTokens_t;

/* Private define ------------------------------------------------------------*/
#define LED 1

/* Definitions of the supported commands */
#define LED_STR		"LED"
#define HELP_STR	"HELP"
#define	STATUS_STR	"STATUS"

/* Definitions of the supported arguments for LED operation */
#define LED_ON		"ON"
#define LED_OFF		"OFF"
#define	LED_TOGGLE	"TOGGLE"

/* Definitions of the case insensitive operation */
#define MIN_UPPERCASE_VALUE 97
#define MAX_UPPERCASE_VALUE 122
#define UPPERCASE_OFFSET 32

/* Private variables ---------------------------------------------------------*/

static cmdParserState_t state;
static cmdStatus_t error;
static cmdTokens_t tokens;

/* Used to store the users input */
static uint8_t buffer[CMD_MAX_LINE];
static uint16_t idx;

/* Track the status of the LED */
static bool ledStatus;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Clears the input buffer.
  * @param  None
  * @retval None
  */
static void clearBuffer(void)
{
	idx = 0;
	buffer[idx] = '\0';
}

/**
  * @brief  Clear the identified tokens structure.
  * @param  None
  * @retval None
  */
static void clearTokens(void)
{
	tokens.action = UNKOWN;
	tokens.arg1[0] = '\0';
	tokens.arg2[0] = '\0';
	tokens.status = CMD_OK;
}

/**
  * @brief  Add one element to the input buffer, transform lower case to upper case
  * 		and echo the value.
  * @param  element: new element
  * @retval None
  */
static void addBufferElement(uint8_t element)
{
	if (element >= MIN_UPPERCASE_VALUE && element <= MAX_UPPERCASE_VALUE) {
		element -= UPPERCASE_OFFSET;
	}

	uartSendStringSize((uint8_t *)&element, 1);

	buffer[idx] = element;
	idx++;
}

/**
  * @brief  Identify if the desired element is one of the command terminators
  * @param  element: element to be inspected
  * @retval True if the element is one of the command terminators
  */
static bool isCmdTerminator(const uint8_t element)
{
	return element == '\r' || element == '\n';
}

/**
  * @brief  Identify if initial element of the input is one of the comment identifiers
  * @param  element: element to be inspected
  * @retval True if the element is one of the comment identifiers
  */
static bool shouldIgnoreCommand()
{
	return buffer[0] == '#' || buffer[0] == '/';
}

/**
  * @brief  Analyze the input buffer to identify the desired action and arguments,
  * 		then update the tokens structure to further execution
  * @param  None
  * @retval None
  */
static bool populateTokens()
{
	uint8_t commandBuffer[CMD_MAX_LINE];

	int identifiedTokens = sscanf(
		(char *)buffer,
		"%s %s %s",
		(char *)commandBuffer,
		tokens.arg1,
		tokens.arg2
	);

	if (identifiedTokens == 0) {
		tokens.status = CMD_ERR_SYNTAX;
		return false;
	}

	if (strcmp((char *)commandBuffer, HELP_STR) == 0) {
		tokens.action = HELP;
		tokens.status = CMD_OK;
		return true;
	}

	if (strcmp((char *)commandBuffer, STATUS_STR) == 0) {
		tokens.action = STATUS;
		tokens.status = CMD_OK;
		return true;
	}

	if (strcmp((char *)commandBuffer, LED_STR) == 0) {
		tokens.action = CONTROL_LED;

		if (identifiedTokens != 2) {
			tokens.status = CMD_ERR_ARG;
			return false;
		}

		if (strcmp((char *)tokens.arg1, LED_ON) == 0) {
			tokens.status = CMD_OK;
			return true;
		}

		if (strcmp((char *)tokens.arg1, LED_OFF) == 0) {
			tokens.status = CMD_OK;
			return true;
		}

		if (strcmp((char *)tokens.arg1, LED_TOGGLE) == 0) {
			tokens.status = CMD_OK;
			return true;
		}

	}

	tokens.status = CMD_ERR_SYNTAX;
	return false;
}

/**
  * @brief  Turn on the led and update the tracking variable
  * @param  None
  * @retval None
  */
static void turnLedOff(void)
{
	ledStatus = false;
	BSP_LED_Off(LED);
}

/**
  * @brief  Turn off the led and update the tracking variable
  * @param  None
  * @retval None
  */
static void turnLedOn(void)
{
	ledStatus = true;
	BSP_LED_On(LED);
}

/**
  * @brief  Toggle the led and update the tracking variable
  * @param  None
  * @retval None
  */
static void ledToggle(void)
{
	ledStatus = !ledStatus;
	BSP_LED_Toggle(LED);
}

/**
  * @brief  Execute the HELP command
  * @param  None
  * @retval None
  */
void cmdHelp()
{
	uartSendString((uint8_t *)"====================================================\r\n");
	uartSendString((uint8_t *)"HELP command invoked\r\n");
	uartSendString((uint8_t *)"Available commands:\r\n");
	uartSendString((uint8_t *)"HELP: Show the current information\r\n");
	uartSendString((uint8_t *)"STATUS: Show the current status of the LED\r\n");
	uartSendString((uint8_t *)"LED: Args \"ON\" / \"OFF\" / \" TOGGLE \" change the status of the LED\r\n");
	uartSendString((uint8_t *)"====================================================\r\n");
}

/**
  * @brief  Execute the STATUS command
  * @param  None
  * @retval None
  */
void cmdStatus()
{
	uartSendString((uint8_t *)"====================================================\r\n");
	uartSendString((uint8_t *)"STATUS command invoked\r\n");

	if (ledStatus) {
		uartSendString((uint8_t *)"LED is ON\r\n");
	}else {
		uartSendString((uint8_t *)"LED is OFF\r\n");
	}

	uartSendString((uint8_t *)"====================================================\r\n");
}

/**
  * @brief  Execute the LED command
  * @param  None
  * @retval None
  */
void cmdLed()
{
	uartSendString((uint8_t *)"====================================================\r\n");
	uartSendString((uint8_t *)"LED command invoked\r\n");

	if (strcmp((char *)tokens.arg1, LED_ON) == 0) {
		turnLedOn();
		uartSendString((uint8_t *)"LED turned ON\r\n");
	} else if (strcmp((char *)tokens.arg1, LED_OFF) == 0) {
		turnLedOff();
		uartSendString((uint8_t *)"LED turned OFF\r\n");
	} else if (strcmp((char *)tokens.arg1, LED_TOGGLE) == 0) {
		ledToggle();
		uartSendString((uint8_t *)"LED toggled\r\n");
	} else {
		uartSendString((uint8_t *)"Unsupported action\r\n");
	}
		
	uartSendString((uint8_t *)"====================================================\r\n");
}

/**
  * @brief  Show a clear description of the identified error
  * @param  None
  * @retval None
  */

void cmdError(void)
{
	switch(error) {
	case CMD_ERR_OVERFLOW:
		uartSendString((uint8_t *)"ERROR: Line too long\r\n");
		break;
	case CMD_ERR_SYNTAX:
		uartSendString((uint8_t *)"ERROR: Wrong syntax\r\n");
		break;
	case CMD_ERR_ARG:
		uartSendString((uint8_t *)"ERROR: Bad arguments\r\n");
		break;
	case CMD_ERR_UNKOWN:
		uartSendString((uint8_t *)"ERROR: Unknown error\r\n");
		break;
	case CMD_OK:
	default:
		uartSendString((uint8_t *)"Error: Ignore error\r\n");
		break;
	}
}

/**
  * @brief  Call the proper function based on the identified action
  * @param  None
  * @retval None
  */
bool cmdExecute(void)
{
	switch(tokens.action) {
	case HELP:
		cmdHelp();
		break;
	case STATUS:
		cmdStatus();
		break;
	case CONTROL_LED:
		cmdLed();
		break;
	default:
		return false;
	}

	return true;
}

/**
  * @brief  If the FSM is in the process, exec or error state ignore
  * 		the uart input so the FSM could complete its transitions
  * @param  None
  * @retval True if the state is process, exec or error
  */
static bool shouldIgnoreUart(void)
{
	return
		state == CMD_PROCESS ||
		state == CMD_EXEC ||
		state == CMD_ERROR;
}
/**
  * @brief  Initialize the cmdParser, the initial state is CMD_IDLE,
  * 		turn off the LED, clear the tokens structure and the input buffer
  * @param  None
  * @retval None
  */
bool cmdParserInit(void)
{
	state = CMD_IDLE;
	error = CMD_OK;

	turnLedOff();
	clearTokens();
	clearBuffer();

	uartSendString((uint8_t *)"Input > ");

	return true;
}

/**
  * @brief  Update the FSM of the cmdParser according to the users input
  * @param  None
  * @retval None
  */
void cmdPoll(void)
{
	uint8_t c;
	bool newElementReceived = false;

	if (!shouldIgnoreUart() && uartReceiveStringSize(&c, 1)) {
		addBufferElement(c);
		newElementReceived = true;
	}

	switch (state) {

	case CMD_IDLE:
		/* Initial state of the FSM */
		if (newElementReceived && !isCmdTerminator(c)) {
			state = CMD_RECEIVING;
		}
		break;

	case CMD_RECEIVING:
		/* Second state, identifies if the line is too long
		 * and transit notify the CMD_ERR_OVERFLOW
		 * or if the received element is a terminator, if so
		 * transit to CMD_PROCESS
		 */
		if (idx > CMD_MAX_LINE) {
			state = CMD_ERROR;
			error = CMD_ERR_OVERFLOW;
			uartSendString((uint8_t *)"\r\n");

		}

		if (newElementReceived) {
			if (isCmdTerminator(c)) {
				state = CMD_PROCESS;
				uartSendString((uint8_t *)"\r\n");
			}

		}

		break;

	case CMD_PROCESS:
		/* Validate the input buffer, identify if the input should
		 * be ignored (/ or #) thus transit to CMD_IDLE.
		 * Build the tokens structure based on the input buffer.
		 * Identify syntax errors, wrong arguments and transit to CMD_ERROR
		 * If everything is okay transit to CMD_EXEC
		 */
		buffer[idx] = '\0';

		if (shouldIgnoreCommand()) {
			uartSendString((uint8_t *)"Input will be ignored\r\n");
			clearTokens();
			clearBuffer();
			uartSendString((uint8_t *)"Input > ");
			state = CMD_IDLE;
			break;
		}

		populateTokens();

		if (tokens.status != CMD_OK) {
			state = CMD_ERROR;
			error = tokens.status;
			break;
		}

		state = CMD_EXEC;
		break;

	case CMD_EXEC:
		/* Execute the supported command, if everything was okay transit
		 * to CMD_IDLE.
		 * If there is an error transit to CMD_ERROR
		 */
		if (cmdExecute()) {
			clearTokens();
			clearBuffer();
			uartSendString((uint8_t *)"Input > ");
			state = CMD_IDLE;
		} else {
			error = CMD_ERR_UNKOWN;
			state = CMD_ERROR;
		}
		break;

	case CMD_ERROR:
		/* Based on the identified error show the corresponding
		 * message to the user.
		 * At the end transit to CMD_IDLE
		 */
		cmdError();
		clearTokens();
		clearBuffer();
		uartSendString((uint8_t *)"Input > ");
		state = CMD_IDLE;
		break;

	default:
		cmdParserInit();
		break;
	}
}
