/**
 * V4P Low-Level Definitions for Emscripten
 */
#ifndef V4P_LL_H
#define V4P_LL_H

#include <stdint.h>
#include <stddef.h>

// Basic type definitions
typedef int Boolean;
typedef uint8_t UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef int16_t Int16;
typedef int32_t Int32;

typedef int32_t V4pCoord;

#define true 1
#define false 0
#define failure true
#define success false

// Debug and trace functions
void v4p_debug(const char* format, ...);
int v4p_error(char* s, ...);

#endif // V4P_LL_H