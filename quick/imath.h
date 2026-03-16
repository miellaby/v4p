#ifndef QUICKMATH_H
#define QUICKMATH_H
#include "v4p_ll.h"

// These variables store the current angle and related values
extern uint16_t lwmAngle;
extern int lwmCosa;
extern int lwmSina;

// compute floor(log2(int))
// floorLog2(10) = floor(log2(10)) = 3
int floorLog2(uint32_t v);

// floorLog2 on int
int floorLog232(uint32_t v);

// compute cos/sin and upate lwmCosa, lwmSina (1 / 255 unit)
int computeCosSin(uint16_t angle);

// Sign function for proper rounding: returns -1, 0, or 1
#define SIGN(x) (((x) > 0) - ((x) < 0))

// macro
#define IMIN(A, B) ((A) < (B) ? (A) : (B))

// macro
#define IMAX(A, B) ((A) > (B) ? (A) : (B))

// macro
#define ILIMIT(A, B, C) ((A) < (B) ? (B) : ((A) > (C) ? (C) : (A)))

// iabs
#define IABS(A) ((A) < 0 ? -(A) : (A))

// rotate (x,y) vector according to current transformation (computeCosSin must have been called before)
void straighten(V4pCoord x, V4pCoord y, V4pCoord* xn, V4pCoord* yn);

// Jim Henry's isqrt
uint16_t isqrt(uint16_t v);

// compute the angle of (0-0)->(x,y) vector
uint16_t iatan(V4pCoord x, V4pCoord y);

// compare two angle in do-what-I-mean way
int angleCmp(uint16_t a1, uint16_t a0);

// compute approximative length of (0,0)-(x,y)
V4pCoord gaugeDist(V4pCoord x, V4pCoord y);

// absolute
V4pCoord iabs(V4pCoord x);

#endif
