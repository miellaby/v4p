#pragma once
#include "v4p_ll.h"  // For types
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define v4p_malloc malloc
#define v4p_free free
#define v4p_memset memset
#define v4p_assert(expression, message) assert(expression)
int32_t v4p_getTicks();
void v4p_delay(int32_t d);
