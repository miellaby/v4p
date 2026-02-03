#include "v4p.h"
#include "v4pi.h"
#include "qfont.h"

#define AA0 "1111 111  1111 111  1111 1111 1111 1  1 111  1111 1  1 1    "
#define AA1 "1  1 1  1 1    1  1 1    1    1    1  1  1      1 1 1  1    "
#define AA2 "1111 111  1    1  1 111  111  1 11 1111  1      1 11   1    "
#define AA3 "1  1 1  1 1    1  1 1    1    1  1 1  1  1   1  1 1 1  1    "
#define AA4 "1  1 111  1111 111  1111 1    1111 1  1 111   11  1  1 1111 "

#define AB0 "111  1  1 1111 111  1111 111  1111 1111 1  1 1  1 1  1 1  1 "
#define AB1 "1111 11 1 1  1 1  1 1  1 1  1 1      1  1  1 1  1 1 11 1  1 "
#define AB2 "11 1 1 11 1  1 111  1  1 111  1111   1  1  1 1  1 1 11  11  "
#define AB3 "11 1 1  1 1  1 1    1 11 1  1    1   1  1  1 1 1  1111 1  1 "
#define AB4 "11 1 1  1 1111 1    1111 1  1 1111   1  1111  1   111  1  1 "

#define AC0 "1  1 1111 "
#define AC1 "1  1    1 "
#define AC2 " 111  11  "
#define AC3 "   1 1    "
#define AC4 " 11  1111 "

#define NA0 " 1111  1  1111 1111 1  1 1111 1111 1111 1111 1111 "
#define NA1 " 1  1 11     1    1 1  1 1    1       1 1  1 1  1 "
#define NA2 " 1 11  1  1111 1111 1111 1111 1111   1  1111 1111 "
#define NA3 " 11 1  1  1       1    1    1 1  1  1   1  1    1 "
#define NA4 " 1111 111 1111 1111    1 1111 1111  1   1111 1111 "

// !/?,"'.:;&*+-<=>@$
#define PA0 " 11     1  11  1 1    1        11   11   11  1  1   1  "
#define PA1 " 11    11 1  1 1 1    1        11   11  1  1  11    1  "
#define PA2 " 11   11    1  1 1   1                   1   1111  111 "
#define PA3 "     11                   11   11    1  1 1   11    1  "
#define PA4 " 11  1      1             11   11   1    111 1  1   1  "

#define PB0 "       11      11    11   111 "
#define PB1 "      11  1111  11  1  1 111  "
#define PB2 "1111 11          11   11 1111 "
#define PB3 "      11  1111  11   1 1  111 "
#define PB4 "       11      11    11  111  "

#define S0 " " AA0 AB0 AC0 NA0 PA0 PB0
#define S1 " " AA1 AB1 AC1 NA1 PA1 PB1
#define S2 " " AA2 AB2 AC2 NA2 PA2 PB2
#define S3 " " AA3 AB3 AC3 NA3 PA3 PB3
#define S4 " " AA4 AB4 AC4 NA4 PA4 PB4

char* qfont[5] = { S0, S1, S2, S3, S4 };

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 5

static int ichar(char c) {
    int i = 0;
    char* s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!/?,\"'.:;&*+-<=>@$";
    if (! c || c == ' ')
        return -1;

    while (c != s[i] && s[i])
        i++;
    if (! s[i])
        return 45;  // *
    return i;
}

V4pPolygonP qfontDefinePolygonFromChar(char c,
                                    V4pPolygonP poly,
                                    V4pCoord x,
                                    V4pCoord y,
                                    V4pCoord width,
                                    V4pCoord height) {
    int i, j, down, is;
    int ic = ichar(c);
    if (ic == -1)
        return poly;  // V4P_WHITE char, no edge

    v4p_addJump(poly);
    for (i = 0; i <= CHAR_WIDTH; i++) {
        Boolean down = 0;
        // v4p_addJump (poly);
        for (j = 0; j < CHAR_HEIGHT; j++) {
            int is = ic * (1 + CHAR_WIDTH) + i;
            if (! down && qfont[j][is] != qfont[j][1 + is]) {
                v4p_addPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
                down = 1;
            } else if (down && qfont[j][is] == qfont[j][1 + is]) {
                // dubed point to left the pen up
                v4p_addPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
                v4p_addJump(poly);
                down = 0;
            }
        }
        if (down) {
            v4p_addPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
            v4p_addJump(poly);
        }
    }
    return poly;
}

V4pPolygonP qfontDefinePolygonFromString(char* s,
                                      V4pPolygonP poly,
                                      V4pCoord x,
                                      V4pCoord y,
                                      V4pCoord width,
                                      V4pCoord height,
                                      V4pCoord interleave) {
    char c;
    int i;
    for (i = 0; (c = s[i]); i++) {
        qfontDefinePolygonFromChar(c, poly, x, y, width, height);
        x += width + interleave;
    }
    return poly;
}
