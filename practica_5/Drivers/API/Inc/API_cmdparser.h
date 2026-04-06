#ifndef __API_CMDPARSER_H
#define __API_CMDPARSER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef bool bool_t;

#define CMD_MAX_LINE	64
#define CMD_MAX_TOKENS	3

typedef enum {
	CMD_OK = 0,
	CMD_ERR_OVERFLOW,
	CMD_ERR_SYNTAX,
	CMD_ERR_UNKOWN,
	CMD_ERR_ARG,
} cmdStatus_t;


bool cmdParserInit(void);
void cmdPoll(void);
void cmdPrintHelp(void);


#endif
