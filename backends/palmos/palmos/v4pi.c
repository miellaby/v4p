/**
 * V4P Implementation for Palm-OS
 */
#include <stdlib.h>
#include <stdarg.h>

#include "v4pi.h"
#include "quick/imath.h"
#include "v4p.h"

V4pCoord screenWidth = 160, screenHeight = 160, marginX = 8, marginY = 8, bytesBetweenLines = 2 * 8,
      lineWidth = 160 - 2 * 8, lineNb = 160 - 2 * 8;

V4pColor bgColor = 0;

static char* buffer = NULL;

static int iBuffer;

// Metrics stuff
static int32_t t1, laps[4] = { 0, 0, 0, 0 };

int v4pi_start() {
    int i;
    t1 = TimGetTicks();
    iBuffer = marginY * screenWidth + marginX;


    return success;
}

int v4pi_end() {
    int i;
    // Get end time and compute average rendering time
    static int j = 0;
    int32_t t2 = TimGetTicks();
    laps[j++ % 4] = t2 - t1;
    if (! (j % 100)) v4p_trace(RENDER, "render time = %.1fms\n", (laps[0] + laps[1] + laps[2] + laps[3]) / 4.0);

    return success;
}

int v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l;
    if (x1 <= x0)
        return success;
    l = x1 - x0;
    // WinSetForeColor((IndexedColorType)c);
    // WinDrawLine(x0 + 10, y+10, x1+9, y+10);
    v4p_memset(&buffer[iBuffer], l, (char) c);
    iBuffer += l;
    if (x1 == lineWidth)
        iBuffer += bytesBetweenLines;

    return success;
}

int v4pi_init(int quality, V4pColor background) {
    bgColor = background;
    buffer = BmpGetBits(WinGetBitmap(WinGetDisplayWindow()));
}

void v4pi_quit() {
}
