#ifndef __API_DEBOUNCE_H
#define __API_DEBOUNCE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef bool bool_t;

void debounceFSM_init(void);
void debounceFSM_update(void);

bool_t readKey();

#endif
