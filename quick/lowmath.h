#ifndef LOWMATH_H
#define LOWMATH_H
#include "v4p_ll.h"

// These variables store the current angle and related values
extern UInt16 lwmAngle;
extern int lwmCosa;
extern int lwmSina;

// compute floor(log2(int))
// floorLog2(10) = floor(log2(10)) = 3
int floorLog2(UInt32 v);

// floorLog2 on int
int floorLog232(UInt32 v);

// compute cos/sin and upate lwmCosa, lwmSina (1 / 255 unit)
Boolean computeCosSin(UInt16 angle);

// macro
#define imin(A, B) ((A) < (B) ? (A) : (B))

// macro
#define imax(A, B) ((A) > (B) ? (A) : (B))

// macro
#define ilimit(A, B, C) ((A) < (B) ? (B) : ((A) > (C) ? (C) : (A)))

// rotate (x,y) vector according to current transformation (computeCosSin must have been called before)
void straighten(V4pCoord x, V4pCoord y, V4pCoord* xn, V4pCoord* yn);

// Jim Henry's isqrt
UInt16 isqrt(UInt16 v);

// compute the angle of (0-0)->(x,y) vector
UInt16 iatan(V4pCoord x, V4pCoord y);

// compute the angle of (x1,y1)->(x2,y2) vector
UInt16 iatan2p(V4pCoord x1, V4pCoord y1, V4pCoord x0, V4pCoord y0);

// compare two angle in do-what-I-mean way
int angleCmp(UInt16 a1, UInt16 a0);

// compute approximative length of (0,0)-(x,y)
V4pCoord gaugeDist(V4pCoord x, V4pCoord y);

// absolute
V4pCoord iabs(V4pCoord x);

#endif
