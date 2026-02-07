#pragma once
#include <stdint.h>
#include <stddef.h>
typedef int Boolean;
typedef uint8_t UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef int16_t Int16;
typedef int32_t Int32;

typedef int32_t V4pCoord;
typedef struct stuffing_s* Stuffing;

#define true 1
#define false 0
#define failure true
#define success false

// Debug and error functions
#if defined(DEBUG) || defined(V4P_LL_C)
    void v4pi_debug(char* formatString, ...);
#else
    #define v4pi_debug(...) ((void)0)
#endif

// Trace function
void v4pi_trace(const char* tag, const char* format, ...);

Boolean v4p_error(char* s, ...);
