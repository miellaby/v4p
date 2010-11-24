#ifndef V4P_LL
#define V4P_LL
#include "SDL/SDL_types.h"
typedef SDL_bool Boolean;
typedef Uint8    UInt8;
typedef Uint16   UInt16;
typedef Uint32   UInt32;
typedef Sint16   Int16;
typedef Sint32   Int32;

typedef Sint32  Coord;
typedef UInt8    Color;
typedef struct stuffing_s* Stuffing;

#define true SDL_TRUE
#define false SDL_FALSE
#define failure -1
#define success 0
#endif
