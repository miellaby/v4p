/**
 * Name: LowMath
 * Content: Inaccurate integer only Math routines for game authoring
 */
#include "quick/imath.h"
int lwmCosa = 255;
int lwmSina = 0;
static uint16_t lastAngle = 0;
const int tabCos[129] = {
    255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 253, 252, 252, 251, 251, 250, 249, 249, 248, 247, 247,
    246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 234, 233, 232, 231, 229, 228, 226, 225, 223, 222, 220,
    219, 217, 215, 214, 212, 210, 208, 207, 205, 203, 201, 199, 197, 195, 193, 191, 189, 187, 185, 183, 180, 178,
    176, 174, 171, 169, 167, 164, 162, 159, 157, 154, 152, 149, 147, 144, 142, 139, 136, 134, 131, 128, 126, 123,
    120, 117, 115, 112, 109, 106, 103, 100, 98,  95,  92,  89,  86,  83,  80,  77,  74,  71,  68,  65,  62,  59,
    56,  53,  50,  47,  44,  41,  37,  34,  31,  28,  25,  22,  19,  16,  13,  9,   6,   3,
    0  // The 129th entry (Index 128)
};

int floorLog2(uint32_t v) {
    if (! v) return -1;
    #if defined(__GNUC__) || defined(__clang__)
        return 31 - __builtin_clz(v);
    #else
        return floorLog232(v);
    #endif
}

int floorLog232(uint32_t v) {
    //  Find the log base 2 of an N-bit integer in O(lg(N)) operations with
    //  multiply and lookup
    // Credits: http://graphics.stanford.edu/~seander/bithacks.htm

    static const int MultiplyDeBruijnBitPosition[32]
        = { 0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
            8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31 };
    if (! v)
        return -1;

    v |= v >> 1;  // first round down to one less than a power of 2
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return MultiplyDeBruijnBitPosition[(uint32_t) (v * 0x07C4ACDDU) >> 27];
}

/** Compute cosine and sine values lwmCosa and lwmSina for a given angle
 *  in the range [0, 512) where 512 represents a full circle (360 degrees).
 */
int computeCosSin(uint16_t angle) {
    // Use a lookup table for cosine values and derives sine from it
    int tb, tr;
    uint16_t b;
    angle = (angle & (uint16_t) 0x1FF);
    if (angle == lastAngle) return success;

    lastAngle = angle;
    b = angle & (uint16_t) 127; // b is 0 to 127

    if (! (angle & (uint16_t) 256)) {  // First 1/2 circle
        tb = tabCos[b];
        tr = tabCos[128 - b];        // Changed from 127 to 128
    } else {
        tb = -tabCos[b];
        tr = -tabCos[128 - b];       // Changed from 127 to 128
    }

    if (! (angle & (uint16_t) 128)) {  // 1st or 3d 1/4 circle
        lwmCosa = tb;
        lwmSina = tr;
    } else {
        lwmCosa = -tr;
        lwmSina = tb;
    }
    return success;
}

void straighten(V4pCoord x, V4pCoord y, V4pCoord* xn, V4pCoord* yn) {
    if (! lastAngle) {
        *xn = x;
        *yn = y;
    } else {
        *xn = (x * lwmCosa - y * lwmSina) >> 8;
        *yn = (x * lwmSina + y * lwmCosa) >> 8;
    }
}

uint16_t isqrt(uint16_t v) {  // Jim Henry isqrt
    uint16_t t, g = 0, b = 0x80, s = 7;
    while (b > 0) {
        t = ((g << 1) + b) << s;
        if (v >= t) {
            g += b;
            v -= t;
        }
        s--;
        b = (b >> 1);
    }
    return g;
}

uint16_t isqrt32(uint32_t v) {
    uint32_t t;
    uint16_t g = 0;
    uint16_t b = 0x100;  // 9 bits of search (result fits uint16_t)
    int s = 15;
    while (b > 0) {
        t = ((uint32_t) (g << 1) + b) << s;
        if (v >= t) {
            g += b;
            v -= t;
        }
        s--;
        b >>= 1;
    }
    return g;
}


// 64 entries, index = y*64/x (0..63), value = angle in [0..64]
static const uint8_t tabAtan64[65] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20,
    20, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34,
    34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44,
    64  // sentinel for index==64 (x==y case)
};

uint16_t iatan(V4pCoord x, V4pCoord y) {
    uint16_t a;
    V4pCoord t;
    bool op1, op2, op3;

    if (y < 0) {
        y = -y;
        op3 = true;
    } else
        op3 = false;
    if (x < 0) {
        x = -x;
        op2 = true;
    } else
        op2 = false;
    if (x < y) {
        t = x;
        x = y;
        y = t;
        op1 = true;
    } else
        op1 = false;

    if (y == 0) {
        a = 0;
    } else {
        int idx = (int) ((long) y * 64 / x);  // 0..64, x>=y so no overflow
        a = tabAtan64[idx];
    }

    if (op1) a = 128 - a;
    if (op2) a = 256 - a;
    if (op3) a = (uint16_t) (512 - a);
    return a % 512;
}

int angleCmp(uint16_t a1, uint16_t a0) {
    int d = a1 - a0;
    if (! (d & (uint16_t) 0x1ff))
        return 0;
    else if (d & (uint16_t) 0x100)
        return (d | (uint16_t) 0xfffffe00);
    else
        return (d & (uint16_t) 0x1ff);
}

V4pCoord iabs(V4pCoord i) {
    int const mask = i >> (sizeof(int) * 8 - 1);
    return (i + mask) ^ mask;
}

// distance estimation (inaccurate but quick)
V4pCoord gaugeDist(V4pCoord x, V4pCoord y) {
    if (x < 0) x = -x;

    if (y < 0) y = -y;

    // min.(sqrt2-1)+max
    return ((IMIN(x, y) * 424) >> 10) + IMAX(x, y);
}
