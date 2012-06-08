/* Name: LowMath
 Content: Innacurate integer only Math routines for game authoring
*/
#include "lowmath.h"
int cosa = 255 ;
int sina = 0 ;
static UInt16 lastAngle = 0 ;

const int tabCos[128] = {
 255, 255, 255, 255, 255, 255, 255, 254,
   254, 254, 253, 253, 252, 252, 251, 251,
   250, 249, 249, 248, 247, 246, 245, 244,
   244, 243, 242, 241, 239, 238, 237, 236,
   235, 234, 232, 231, 230, 228, 227, 225,
   224, 222, 221, 219, 217, 216, 214, 212,
   211, 209, 207, 205, 203, 201, 199, 197,
   195, 193, 191, 189, 187, 185, 183, 181,
   178, 176, 174, 171, 169, 167, 164, 162,
   159, 157, 155, 152, 149, 147, 144, 142,
   139, 136, 134, 131, 128, 126, 123, 120,
   117, 115, 112, 109, 106, 103, 100, 97,
   95, 92, 89, 86, 83, 80, 77, 74,
   71, 68, 65, 62, 59, 56, 53, 49,
   46, 43, 40, 37, 34, 31, 28, 25,
   21, 18, 15, 12,  9,  6,  3, 0
} ;

int floorLog2(UInt16 v) {
    return floorLog232(v);
}

int floorLog232(UInt32 v) {
  //  Find the log base 2 of an N-bit integer in O(lg(N)) operations with multiply and lookup
  // Credits: http://graphics.stanford.edu/~seander/bithacks.htm

  static const int MultiplyDeBruijnBitPosition[32] =  {
     0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
     8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };
  if (!v) return -1;

  v |= v >> 1; // first round down to one less than a power of 2
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;

  return MultiplyDeBruijnBitPosition[(UInt32)(v * 0x07C4ACDDU) >> 27];
}

Boolean computeCosSin(UInt16 angle) {
   int tb, tr ;
   UInt16 b;
   angle = (angle & (UInt16)0x1FF);
   if (angle == lastAngle)
      return success ;

   lastAngle = angle ;
   b = angle & (UInt16)127;//b=a mod90deg
   if (!(angle & (UInt16)256))
      { // First 1/2 circle
      tb = tabCos[b];
      tr = tabCos[127-b] ;
      }
   else
      {
      tb = -tabCos[b];
      tr = -tabCos[127-b] ;
      }

   if (!(angle & (UInt16)128))
      { // 1st or 3d 1/4 circle
      cosa = tb;
      sina = tr;
      }
   else
      {
      cosa = -tr;
      sina = tb;
      }
   return success ;
}

Boolean straighten(Coord x, Coord y, Coord *xn, Coord *yn) {
   if (!lastAngle) {
      *xn = x ; *yn = y ;
   } else {
      *xn = (x * cosa - y * sina) >> 8;
      *yn = (x * sina + y * cosa) >> 8;
   }
   return success ;
}

UInt16 isqrt(UInt16 v) { // Jim Henry isqrt
  UInt16 t,g=0,b=0x80,s=7;
  while (b>0) {
    t =((g << 1) + b) << s ;
    if (v >= t) { g += b ; v -= t; }
    s--; b = (b >> 1);
  }
  return g;
}

static UInt16 tabAtanFloorLog2[8] = {
   // 19, 37, 64, 90, 108, 117, 122, 125
   64, 63, 60, 50, 37, 28, 19, 7
} ;

UInt16 iatan(Coord x, Coord y) {
   UInt16 m, a ;
   Coord t ;
   Boolean op1, op2, op3 ;
   if (y<0) { // y inverted
      y = -y ;
      op3 = true ;
   } else op3 = false ;

   if (x<0)  { // x inverted
     x = -x ;
     op2 = true ;
   } else op2 = false ;

   if (x<y) { // swap x y
      t = x ; x = y ; y = t ; op1 = true ;
   } else op1 = false ;

   if (y == 0)
      a = 0 ;
   else {
      m = (x << 3) / y ;
      if (!m)
         a = 128 ;
      else if (m > 255)
         a = 0 ;
      else
         a =tabAtanFloorLog2[floorLog2(m)] ;
   }
   if (op1)
      a = 128 - a ;
   if (op2)
      a = 256 - a ;
   if (op3)
      a = 512 - a ;
   return a ;
}

UInt16 iatan2p(Coord x1, Coord y1, Coord x0, Coord y0) {
   return iatan(x1 - x0, y1 - y0) ;
}

int angleCmp(UInt16 a1, UInt16 a0) {
  int d = a1 - a0 ;
  if (!(d & (UInt16)0x1ff))
    return 0 ;
  else if (d & (UInt16)0x100)
    return (d | (UInt16)0xfffffe00) ;
  else
    return (d & (UInt16)0x1ff);
}

Coord iabs(Coord i) {
 int const mask = i >> (sizeof(int) * 8 - 1);
 return (i + mask) ^ mask;
}

// distance estimation (inaccurate but quick)
Coord gaugeDist(Coord x, Coord y) {
  if (x<0)
     x = -x ;

  if (y<0)
     y = -y ;

  // min.(sqrt2-1)+max
  return ((imin(x, y) * 424) >> 10) + imax(x, y) ;
}
