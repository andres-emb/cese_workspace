#ifndef __API_UART_H
#define __API_UART_H

#include <stdbool.h>
#include <stdint.h>

typedef bool bool_t;

bool_t uartInit(void);
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
bool uartReceiveStringSize(uint8_t * pstring, uint16_t size);

#endif
