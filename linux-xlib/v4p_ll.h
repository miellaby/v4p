#ifndef V4P_LL
#define V4P_LL
#include <stdint.h>
typedef int       Boolean;
typedef uint8_t   UInt8;
typedef uint16_t  UInt16;
typedef uint32_t  UInt32;
typedef int16_t   Int16;
typedef int32_t   Int32;

typedef int32_t   Coord;
typedef uint8_t    Color;
typedef struct stuffing_s* Stuffing;

#define true (~0)
#define false 0
#define failure -1
#define success 0
#endif
