#ifndef LOWMATH_H
#define LOWMATH_H
#include "v4p_ll.h"

// These variables store the current angle and related values
extern UInt16 lwmAngle ;
extern int    lwmCosa ;
extern int    lwmSina ;

// compute floor(log2(short))
// floorLog2(10) = floor(log2(10)) = 3
int floorLog2(UInt16 v) ;

// floorLog2 on int
int floorLog232(UInt32 v) ;

// compute cos/sin and upate angle, cosa, sina vars
Boolean computeCosSin(UInt16 angle) ;

// macro
#define imin(A,B) ((A)<(B)?(A):(B))

// macro
#define imax(A,B) ((A)>(B)?(A):(B))

// macro
#define ilimit(A,B,C) ((A)<(B)?(B):((A)>(C)?(C):(A)))

// rotate (x,y) vector according to current transformation
Boolean straighten(Coord x, Coord y, Coord *xn, Coord *yn) ;

// Jim Henry's isqrt
UInt16 isqrt(UInt16 v) ;

// compute the angle of (0-0)->(x,y) vector
UInt16 iatan(Coord x, Coord y) ;

// compute the angle of (x1,y1)->(x2,y2) vector
UInt16 iatan2p(Coord x1, Coord y1, Coord x0, Coord y0) ;

// compare two angle in do-what-I-mean way
int angleCmp(UInt16 a1, UInt16 a0) ;

// compute approximative length of (0,0)-(x,y)
Coord gaugeDist(Coord x, Coord y) ;

// absolute
Coord iabs(Coord x) ;

#endif
