/**
 * V4P Implementation for Palm-OS
 */
#include <stdlib.h>
#include <stdarg.h>

#include "v4pi.h"
#include "lowmath.h"
#include "v4p.h"

// 0..215 combination of
//    ff  cc  99  66  33  00
// B : 0   6  12 108 114 120
// R : 0  18  36  54  72  90
// G : 0   1   2   3  4    5
// e.g  12+54+5 = $99 B + $66 R +$0 G
// 216..224 = V4P_GRAY 22 44 55 77 88 aa bb dd
// 225..229 = specials: light V4P_GRAY,V4P_MAROON,V4P_PURPLE,V4P_GREEN,V4P_CYAN
// 230..255 = unused (V4P_BLACK)
V4pColor V4P_GRAY = 225, V4P_MAROON = 226, V4P_PURPLE = 227, V4P_GREEN = 228, V4P_CYAN = 229, V4P_BLACK = 215, V4P_RED = 125,
      V4P_BLUE = 120, V4P_YELLOW = 95, V4P_DARK = 217, V4P_OLIVE = 58, V4P_FLUO = 48, V4P_WHITE = 0;
V4pCoord screenWidth = 160, screenHeight = 160, marginX = 8, marginY = 8, bytesBetweenLines = 2 * 8,
      lineWidth = 160 - 2 * 8, lineNb = 160 - 2 * 8;

V4pColor bgColor = 0;

static char* buffer = NULL;

static int iBuffer;



void v4pi_debug(char* formatString, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}

Boolean v4pi_error(char* formatString, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}



static UInt32 t1;
static UInt32 laps[4] = { 0, 0, 0, 0 }, tlaps = 0;

Boolean v4pi_start() {
    int i;
    t1 = TimGetTicks();
    iBuffer = marginY * screenWidth + marginX;


    return success;
}

Boolean v4pi_end() {
    int i;
    static int j = 0;
    UInt32 t2 = TimGetTicks();
    tlaps -= laps[j % 4];
    tlaps += laps[j % 4] = t2 - t1;
    j++;
    idebug(tlaps);


    return success;
}

static void myMemSet(UInt8* pdst, UInt32 numBytes, UInt8 value) {
    UInt32 a4;
    Int32 n;
    void* dst;
    n = numBytes;
    dst = pdst;
    if (n > 6) {
        while ((Int32) dst & 3) {
            *((UInt8*) dst)++ = value;
            n--;
        }
        a4 = value;
        a4 = (a4 << 8) | a4;
        a4 = (a4 << 16) | a4;
        n -= 16;
        while (n > 0) {
            *((UInt32*) dst)++ = a4;
            *((UInt32*) dst)++ = a4;
            *((UInt32*) dst)++ = a4;
            *((UInt32*) dst)++ = a4;
            n -= 16;
        }
        n += 12;
        while (n > 0) {
            *((UInt32*) dst)++ = a4;
            n -= 4;
        }
        n += 4;
    }
    while (n > 0) {
        *((UInt8*) dst)++ = value;
        n--;
    }
}

Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l;
    if (x1 <= x0)
        return success;
    l = x1 - x0;
    // WinSetForeColor((IndexedColorType)c);
    // WinDrawLine(x0 + 10, y+10, x1+9, y+10);
    myMemSet(&buffer[iBuffer], l, (char) c);
    iBuffer += l;
    if (x1 == lineWidth)
        iBuffer += bytesBetweenLines;

    return success;
}

extern int v4p_parseHexDigit(char c);
V4pPolygonP v4pDecodePolygon(char* s) {
    V4pLayer z;
    V4pProps t;
    V4pColor col;
    V4pPolygonP p;
    int i = 0;
    if (strlen(s) < 6)
        return NULL;
    t = v4p_parseHexDigit(s[i]) << 4 + v4p_parseHexDigit(s[i + 1]);
    i = 2;
    col = v4p_parseHexDigit(s[i]) << 4 + v4p_parseHexDigit(s[i + 1]);
    i = 4;
    z = v4p_parseHexDigit(s[i]) << 4 + v4p_parseHexDigit(s[i + 1]);
    p = v4p_new(t, col, z);
    return v4pPolygonDecodePoints(p, s + 6);
}

char* v4pEncodePolygon(V4pPolygonP p) {
    const char* t = "0123456789ABCDEF";
    UInt16 i, v;
    char *s, *ss, *sss;

    s = (char*) malloc(7);
    ss = s;
    for (i = 0; i <= 2; i++) {
        if (i == 0)
            v = v4p_putProp(p, 0);
        else if (i == 1)
            v = v4p_getColor(p);
        else if (i == 2)
            v = v4p_getZ(p);
        *ss = t[v & 15];
        ss++;
        v = v >> 4;
        *ss = t[v & 15];
        ss++;
    }
    *ss = '\0';

    sss = v4pPolygonEncodePoints(p);
    if (! sss) {
        free(s);
        return NULL;
    } else {
        s = (char*) realloc(s, 7 + strlen(sss));
        StrCopy(ss, sss);
        free(sss);
        return s;
    }
}

Boolean v4pDisplayInit(int quality, V4pColor background) {
    bgColor = background;
    buffer = BmpGetBits(WinGetBitmap(WinGetDisplayWindow()));
}

void v4pi_quit() {
}
