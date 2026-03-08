#pragma once
#include "v4p_ll.h"  // For types
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define v4p_malloc malloc
#define v4p_free free
#define v4p_memset memset
#define v4p_assert(expression, message) assert(expression)
