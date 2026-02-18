#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "v4p.h"
#include "v4pi.h"
#include "v4pserial.h"
#include "quick/lowmath.h"

// Transform hexa char ('0-9,A-F') to int
int v4p_parseHexDigit(char c) {
    int o, r;
    o = (int) c;
    r = o - (int) '0';
    if (r >= 0 && r <= 9) {
        return r;
    } else {
        r = o - (int) 'A';
        if (r >= 0 && r <= 5) {
            return 10 + r;
        } else {
            r = o - (int) 'a';
            return (r >= 0 && r <= 5 ? 10 + r : 0);
        }
    }
}

// add points to a polygon with coordinates decoded from a c-string
V4pPolygonP v4p_decodePoints(V4pPolygonP p, char* s, int scale) {
    int j;
    long xs, ys, xs1, ys1;
    Boolean sep, psep;
    char c;

    // Pre-compute integer scaling factors for the scale using quotient-remainder technique
    // This prevents 16-bit overflow by decomposing scale/256 into whole + remainder parts
    V4pCoord scale_whole = scale / 256;
    V4pCoord scale_rem = scale % 256;

    psep = false;
    for (j = 0; s[j]; j++) {
        c = s[j];
        if (c == ' ')
            continue;

        if (c == '.') {
            sep = true;
            if (psep)
                // Apply integer scaling technique with sign-aware rounding
                v4p_addPoint(p, 
                    xs1 * scale_whole + ((xs1 * scale_rem) + SIGN(xs1) * (256 / 2)) / 256,
                    ys1 * scale_whole + ((ys1 * scale_rem) + SIGN(ys1) * (256 / 2)) / 256);
            continue;
        }

        sep = false;

        xs = (v4p_parseHexDigit(c) << 4) + v4p_parseHexDigit(s[++j]);
        ys = v4p_parseHexDigit(s[++j]) << 4;
        ys += v4p_parseHexDigit(s[++j]);

        // Apply integer scaling technique with sign-aware rounding
        v4p_addPoint(p, 
            xs * scale_whole + ((xs * scale_rem) + SIGN(xs) * (256 / 2)) / 256,
            ys * scale_whole + ((ys * scale_rem) + SIGN(ys) * (256 / 2)) / 256);

        if (sep) {
            xs1 = xs;
            ys1 = ys;
            psep = true;
        }
    }
    if (psep)
        // Apply integer scaling technique with sign-aware rounding
        v4p_addPoint(p, 
            xs1 * scale_whole + ((xs1 * scale_rem) + SIGN(xs1) * (256 / 2)) / 256,
            ys1 * scale_whole + ((ys1 * scale_rem) + SIGN(ys1) * (256 / 2)) / 256);

    return p;
}

// create a polygon by adding all points encoded in a c-string
V4pPolygonP v4p_quickPolygon(V4pProps t, V4pColor col, V4pLayer z, char* s, int scale) {
    return v4p_decodePoints(v4p_new(t, col, z), s, scale);
}

// encode every point of a polygon into a single c-string
char* v4p_encodePoints(V4pPolygonP p, int scale) {
    static char* t = "0123456789ABCDEF";
    char* s;
    V4pPointP s1, m, pm;
    int i, l;
    V4pCoord v;

    s = (char*) malloc(32 * sizeof(char));
    l = 0;
    s1 = v4p_getPoints(p);
    m = s1;
    pm = NULL;
    while (m) {
        if (pm && m == s1) {
            s[l++] = '.';
            pm = NULL;
            s1 = m->next;
            m = s1;
        } else {
            for (i = 0; i <= 1; i++) {
                if (! i)
                    v = m->x;
                else
                    v = m->y;
                v = v * 256 / scale;
                s[l++] = t[v & 15];
                s[l++] = t[(v >> 4) & 15];
            }
            pm = m;
            m = m->next;
        }
        if (l % 32 >= 28) {
            s = (char*) realloc(s, (64 + l - l % 32) * sizeof(char));
            if (! s) {
                v4p_error("full Heap");
                return NULL;
            }
        }
    }
    s[l] = '\0';
    return s;
}

V4pPolygonP v4p_decodePolygon(char* s, int scale) {
    V4pLayer z;
    V4pProps t;
    V4pColor col;
    V4pPolygonP p;
    int i = 0;
    if (strlen(s) < 6)
        return NULL;
    t = (v4p_parseHexDigit(s[i]) << 4) + v4p_parseHexDigit(s[i + 1]);
    i = 2;
    col = (v4p_parseHexDigit(s[i]) << 4) + v4p_parseHexDigit(s[i + 1]);
    i = 4;
    z = (v4p_parseHexDigit(s[i]) << 4) + v4p_parseHexDigit(s[i + 1]);
    p = v4p_new(t, col, z);
    return v4p_decodePoints(p, s + 6, scale);
}

char* v4p_encodePolygon(V4pPolygonP p, int scale) {
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
            v = v4p_getLayer(p);
        *ss = t[v & 15];
        ss++;
        v = v >> 4;
        *ss = t[v & 15];
        ss++;
    }
    *ss = '\0';

    sss = v4p_encodePoints(p, scale);
    if (! sss) {
        free(s);
        return NULL;
    } else {
        s = (char*) realloc(s, 7 + strlen(sss));
        strcat(s, sss);
        free(sss);
        return s;
    }
}

// add points to a polygon with coordinates decoded from a c-string
V4pPolygonP v4p_decodeSVGPath(V4pPolygonP p, char* s, float scale) {
    int j;
    Boolean toBeClosed = false, knowFirstPoint = false, nextIsRelative = false;
    char c;
    enum e_status { INIT, MOVE, LINE, NEXT } status = INIT;
    float param_1, xs = 0, xs1;
    float param_2, ys = 0, ys1;
    int offset;

    // Use float scaling for better precision with SVG coordinates

    for (j = 0; s[j]; j++) {
        c = s[j];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
            continue;

        switch (status) {
            case NEXT:  // way to add more points
                if (c >= '0' && c <= '9') {
                    if (sscanf(&s[j], "%f,%f%n", &param_1, &param_2, &offset) >= 2) {
                        j += offset - 1;
                        if (nextIsRelative) {
                            xs += param_1;
                            ys += param_2;
                        } else {
                            xs = param_1;
                            ys = param_2;
                        }
                        // Apply float scaling with rounding
                        v4p_addPoint(p, 
                            (V4pCoord)roundf(xs * scale),
                            (V4pCoord)roundf(ys * scale));
                        if (! knowFirstPoint) {
                            xs1 = xs;
                            ys1 = ys;
                            knowFirstPoint = true;
                        }
                        continue;  // iterate for loop
                    }
                }
                // there is no break here, NEXT case also handled hereafter
                __attribute__((fallthrough));
            case INIT:
                if ((c == 'M' || c == 'm') && knowFirstPoint)
                    v4p_addJump(p);
                if (c == 'L' || c == 'l') {
                    status = LINE;
                } else if (c == 'M' || c == 'm') {
                    status = MOVE;
                } else if (c == 'C' || c == 'c' || c == 'Q' || c == 'q') {
                    status = LINE;  // Curves are not supported
                } else if (c == 'z' || c == 'Z') {
                    toBeClosed = true;
                    status = NEXT;
                }
                nextIsRelative = (c == 'm' || c == 'l');
                break;
            case LINE:
            case MOVE:
                if (sscanf(&s[j], "%f,%f%n", &param_1, &param_2, &offset) >= 2) {
                    j += offset - 1;
                    if (toBeClosed && knowFirstPoint) {
                        // Apply float scaling with rounding
                        v4p_addPoint(p, 
                            (V4pCoord)roundf(xs1 * scale),
                            (V4pCoord)roundf(ys1 * scale));
                        toBeClosed = knowFirstPoint = false;
                    }
                    if (nextIsRelative) {
                        xs += param_1;
                        ys += param_2;
                    } else {
                        xs = param_1;
                        ys = param_2;
                    }
                    // Apply float scaling with rounding
                    v4p_addPoint(p, 
                        (V4pCoord)roundf(xs * scale),
                        (V4pCoord)roundf(ys * scale));
                    if (! knowFirstPoint) {
                        xs1 = xs;
                        ys1 = ys;
                        knowFirstPoint = true;
                    }
                    status = NEXT;
                }
                break;
        }  // switch status
    }  // j
    if (toBeClosed && knowFirstPoint) {
        // Apply float scaling with rounding
        v4p_addPoint(p, 
            (V4pCoord)roundf(xs1 * scale),
            (V4pCoord)roundf(ys1 * scale));
        toBeClosed = knowFirstPoint = false;
    }
    return p;
}
