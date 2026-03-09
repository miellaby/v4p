#pragma once
#include "v4p_ll.h"  // For types
#include <PalmOS.h>
#define v4p_malloc malloc
#define v4p_free free
void v4p_memset(uint8_t* pdst, uint32_t numBytes, uint8_t value);
#define v4p_assert(expression, message) assert(expression)
#define v4p_getTicks() TimGetTicks()
void v4p_delay(int32_t d);
