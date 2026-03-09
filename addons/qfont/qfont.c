/**
 * ASCII character map for qfont
 * Based on standard ASCII characters (man ascii)
 */

#include "v4p.h"
#include "qfont.h"
#include "string.h"

// ASCII character map - 4x5 pixel representation for each character
// 5 bytes each byte represents a row of 4 pixels (4 bits used)
const uint8_t char_map[96][5] = {
    // 32: space
    {   0b0000,
        0b0000,
        0b0000,
        0b0000,
        0b0000 },
    // 33: !
    {   0b0100,
        0b0100,
        0b0100,
        0b0000,
        0b0100 },
    // 34: "
    {   0b1010,
        0b1010,
        0b0000,
        0b0000,
        0b0000 },
    // 35: #
    {   0b0110,
        0b1111,
        0b0110,
        0b1111,
        0b0110 },
    // 36: $
    {   0b0111,
        0b1010,
        0b0110,
        0b0101,
        0b1110 },
    // 37: %
    {   0b0111,
        0b1110,
        0b0100,
        0b0111,
        0b1110 },
    // 38: &
    {   0b0110,
        0b1001,
        0b0100,
        0b1010,
        0b0111 },
    // 39: '
    {   0b0010,
        0b0010,
        0b0000,
        0b0000,
        0b0000 },
    // 40: (
    {   0b0100,
        0b1000,
        0b1000,
        0b1000,
        0b0100 },
    // 41: )
    {   0b0100,
        0b0010,
        0b0010,
        0b0010,
        0b0100 },
    // 42: *
    {   0b1001,
        0b0110,
        0b1111,
        0b0110,
        0b1001 },
    // 43: +
    {   0b0000,
        0b0010,
        0b0111,
        0b0010,
        0b0000 },
    // 44: ,
    {   0b0000,
        0b0000,
        0b0000,
        0b0010,
        0b0100 },
    // 45: -
    {   0b0000,
        0b0000,
        0b0111,
        0b0000,
        0b0000 },
    // 46: .
    {   0b0000,
        0b0000,
        0b0000,
        0b0000,
        0b0100 },
    // 47: /
    {   0b0001,
        0b0001,
        0b0010,
        0b0100,
        0b1000 },
    // 48: 0
    {   0b0110,
        0b1011,
        0b1111,
        0b1101,
        0b0110 },
    // 49: 1
    {   0b0010,
        0b0110,
        0b0010,
        0b0010,
        0b0010 },
    // 50: 2
    {   0b1110,
        0b0001,
        0b0110,
        0b1000,
        0b1111 },
    // 51: 3
    {   0b1110,
        0b0001,
        0b0110,
        0b0001,
        0b1110 },
    // 52: 4
    {   0b0010,
        0b0110,
        0b1010,
        0b1111,
        0b0010 },
    // 53: 5
    {   0b1111,
        0b1000,
        0b1110,
        0b0001,
        0b1110 },
    // 54: 6
    {   0b0110,
        0b1000,
        0b1110,
        0b1001,
        0b0110 },
    // 55: 7
    {   0b1111,
        0b0001,
        0b0010,
        0b0100,
        0b1000 },
    // 56: 8
    {   0b0110,
        0b1001,
        0b0110,
        0b1001,
        0b0110 },
    // 57: 9
    {   0b0110,
        0b1001,
        0b1111,
        0b0001,
        0b0110 },
    // 58: :
    {   0b0000,
        0b0100,
        0b0000,
        0b0100,
        0b0000 },
    // 59: ;
    {   0b0000,
        0b0100,
        0b0000,
        0b0100,
        0b1000 },
    // 60: <
    {   0b0010,
        0b0100,
        0b1000,
        0b0100,
        0b0010 },
    // 61: =
    {   0b0000,
        0b1111,
        0b0000,
        0b1111,
        0b0000 },
    // 62: >
    {   0b0100,
        0b0010,
        0b0001,
        0b0010,
        0b0100 },
    // 63: ?
    {   0b0110,
        0b1001,
        0b0010,
        0b0000,
        0b0010 },
    // 64: @
    {   0b0110,
        0b1001,
        0b0011,
        0b0101,
        0b0110 },
    // 65: A
    {   0b1111,
        0b1001,
        0b1111,
        0b1001,
        0b1001 },
    // 66: B
    {   0b1110,
        0b1001,
        0b1110,
        0b1001,
        0b1110 },
    // 67: C
    {   0b1111,
        0b1000,
        0b1000,
        0b1000,
        0b1111 },
    // 68: D
    {   0b1110,
        0b1001,
        0b1001,
        0b1001,
        0b1110 },
    // 69: E
    {   0b1111,
        0b1000,
        0b1110,
        0b1000,
        0b1111 },
    // 70: F
    {   0b1111,
        0b1000,
        0b1110,
        0b1000,
        0b1000 },
    // 71: G
    {   0b1111,
        0b1000,
        0b1011,
        0b1001,
        0b1111 },
    // 72: H
    {   0b1001,
        0b1001,
        0b1111,
        0b1001,
        0b1001 },
    // 73: I
    {   0b1110,
        0b0100,
        0b0100,
        0b0100,
        0b1110 },
    // 74: J
    {   0b1111,
        0b0001,
        0b0001,
        0b1001,
        0b0110 },
    // 75: K
    {   0b1001,
        0b1010,
        0b1100,
        0b1010,
        0b1001 },
    // 76: L
    {   0b1000,
        0b1000,
        0b1000,
        0b1000,
        0b1111 },
    // 77: M
    {   0b1110,
        0b1111,
        0b1111,
        0b1101,
        0b1101 },
    // 78: N
    {   0b1001,
        0b1101,
        0b1011,
        0b1001,
        0b1001 },
    // 79: O
    {   0b1111,
        0b1001,
        0b1001,
        0b1001,
        0b1111 },
    // 80: P
    {   0b1110,
        0b1001,
        0b1110,
        0b1000,
        0b1000 },
    // 81: Q
    {   0b1111,
        0b1001,
        0b1001,
        0b1011,
        0b1111 },
    // 82: R
    {   0b1110,
        0b1001,
        0b1110,
        0b1001,
        0b1001 },
    // 83: S
    {   0b0111,
        0b1000,
        0b0110,
        0b0001,
        0b1110 },
    // 84: T
    {   0b1111,
        0b0010,
        0b0010,
        0b0010,
        0b0010 },
    // 85: U
    {   0b1001,
        0b1001,
        0b1001,
        0b1001,
        0b1111 },
    // 86: V
    {   0b1001,
        0b1001,
        0b1001,
        0b1010,
        0b0100 },
    // 87: W
    {   0b1001,
        0b1011,
        0b1011,
        0b1111,
        0b1110 },
    // 88: X
    {   0b1001,
        0b1001,
        0b0110,
        0b1001,
        0b1001 },
    // 89: Y
    {   0b1001,
        0b1001,
        0b0111,
        0b0001,
        0b0110 },
    // 90: Z
    {   0b1111,
        0b0010,
        0b0100,
        0b1000,
        0b1111 },
    // 91: [
    {   0b1110,
        0b1000,
        0b1000,
        0b1000,
        0b1110 },
    // 92: antislash
    {   0b1000,
        0b1000,
        0b0100,
        0b0010,
        0b0001 },
    // 93: ]
    {   0b0111,
        0b0001,
        0b0001,
        0b0001,
        0b0111 },
    // 94: ^
    {   0b0100,
        0b1010,
        0b0000,
        0b0000,
        0b0000 },
    // 95: _
    {   0b0000,
        0b0000,
        0b0000,
        0b0000,
        0b1111 },
    // 96: `
    {   0b0100,
        0b0010,
        0b0000,
        0b0000,
        0b0000 },
    // 97: a
    {   0b0000,
        0b0101,
        0b1011,
        0b1011,
        0b0101 },
    // 98: b
    {   0b1000,
        0b1000,
        0b1110,
        0b1001,
        0b1110 },
    // 99: c
    {   0b0000,
        0b0111,
        0b1000,
        0b1000,
        0b0111 },
    // 100: d
    {   0b0001,
        0b0001,
        0b0111,
        0b1001,
        0b0111 },
    // 101: e
    {   0b0110,
        0b1001,
        0b1111,
        0b1000,
        0b0111 },
    // 102: f
    {   0b0011,
        0b0100,
        0b1110,
        0b0100,
        0b0100 },
    // 103: g
    {   0b0111,
        0b1001,
        0b0111,
        0b0001,
        0b0111 },
    // 104: h
    {   0b1000,
        0b1000,
        0b1110,
        0b1001,
        0b1001 },
    // 105: i
    {   0b0000,
        0b0010,
        0b0000,
        0b0010,
        0b0010 },
    // 106: j
    {   0b0001,
        0b0000,
        0b0001,
        0b0001,
        0b0110 },
    // 107: k
    {   0b1000,
        0b1010,
        0b1100,
        0b1010,
        0b1001 },
    // 108: l
    {   0b1100,
        0b0100,
        0b0100,
        0b0100,
        0b1110 },
    // 109: m
    {   0b0000,
        0b1110,
        0b1101,
        0b1101,
        0b1101 },
    // 110: n
    {   0b0000,
        0b1110,
        0b1001,
        0b1001,
        0b1001 },
    // 111: o
    {   0b0000,
        0b0110,
        0b1001,
        0b1001,
        0b0110 },
    // 112: p
    {   0b0000,
        0b1110,
        0b1001,
        0b1110,
        0b1000 },
    // 113: q
    {   0b0000,
        0b0110,
        0b1001,
        0b0111,
        0b0001 },
    // 114: r
    {   0b0000,
        0b1011,
        0b1100,
        0b1000,
        0b1000 },
    // 115: s
    {   0b0000,
        0b0111,
        0b0100,
        0b0010,
        0b1110 },
    // 116: t
    {   0b0100,
        0b1110,
        0b0100,
        0b0100,
        0b0011 },
    // 117: u
    {   0b0000,
        0b1001,
        0b1001,
        0b1001,
        0b0110 },
    // 118: v
    {   0b0000,
        0b1001,
        0b1001,
        0b0110,
        0b0110 },
    // 119: w
    {   0b0000,
        0b1001,
        0b1111,
        0b1111,
        0b0110 },
    // 120: x
    {   0b0000,
        0b1001,
        0b0110,
        0b0110,
        0b1001 },
    // 121: y
    {   0b0000,
        0b1001,
        0b0111,
        0b0001,
        0b0110 },
    // 122: z
    {   0b0000,
        0b1111,
        0b0010,
        0b0100,
        0b1111 },
    // 123: {
    {   0b0110,
        0b0100,
        0b1100,
        0b0100,
        0b0110 },
    // 124: |
    {   0b0100,
        0b0100,
        0b0100,
        0b0100,
        0b0100 },
    // 125: }
    {   0b1100,
        0b0100,
        0b0110,
        0b0100,
        0b1100 },
    // 126: ~
    {   0b0000,
        0b0000,
        0b0101,
        0b1010,
        0b0000 },
    // 127: DEL (the "unknown" character)
    {   0b1111,
        0b1001,
        0b1001,
        0b1001,
        0b1111 }
};

// Function to get character data
const uint8_t* qfont_get_char(char c) {

    if (c >= 32 && c < 127) {
        return char_map[c - 32];
    }
    return char_map[127 - 32];  // Return the "unknown" character for non-printable characters
}

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 5

// One uses a contour tracer to turn a small array of pixels into a polygon.
// The tracing algorithm is a standard "follow the right-hand wall" approach:
//  - Start on an unvisited boundary edge (where a pixel is set on one side and not on the other)
//  - Move forward along the edge, turning right when as much as possible
// It produces closed pathes of points, which we add to the polygon with jump points in between.
// This part was coded by Claude.

// Turn right / left in the direction table
#define TURN_RIGHT(d) (((d) + 1) & 3)
#define TURN_LEFT(d) (((d) + 3) & 3)

// Is pixel (px,py) a filled glyph pixel? (bounds-safe)
static bool pixel_set(const uint8_t* qfont, int px, int py) {
    if (px < 0 || px >= CHAR_WIDTH || py < 0 || py >= CHAR_HEIGHT) return 0;
    return (qfont[py] >> (CHAR_WIDTH - 1 - px)) & 1;
}

// We track visited edges to avoid retracing the same contour multiple times.
// visited array: one bit per boundary edge, sized generously
// We track edges as (x,y,dir) — here flattened into a small array
#define MAX_EDGES ((CHAR_WIDTH + 1) * (CHAR_HEIGHT + 1) * 4)
static uint32_t visited[MAX_EDGES] = {0};
static uint32_t generation = 0; //< We use a generation counter to avoid clearing the visited array on every call.

static int edge_index(int x, int y, int dir) {
    return (y * (CHAR_WIDTH + 1) + x) * 4 + dir;
}

// quotient-remainder scaling macro: scale c by whole + rem/original, with rounding
#define SCALE(c, whole, rem, original) \
    ((c) * (whole) + ((c) * (rem) + (original) / 2) / (original))

V4pPolygonP qfontDefinePolygonFromChar(char c,
                                    V4pPolygonP poly,
                                    V4pCoord x,
                                    V4pCoord y,
                                    V4pCoord width,
                                    V4pCoord height) {
    if (c == 32) return poly;

    const unsigned char* qfont = qfont_get_char(c);

    // pre-compute quotient-remainder scaling factors
    V4pCoord whole_x = width  / CHAR_WIDTH,  rem_x = width  % CHAR_WIDTH;
    V4pCoord whole_y = height / CHAR_HEIGHT, rem_y = height % CHAR_HEIGHT;

    // advance generation; on wraparound, reset to avoid false hits
    if (++generation == 0) {
        memset(visited, 0, sizeof(visited));
        generation = 1;
    }

    for (int j = 0; j <= CHAR_HEIGHT; j++) {
        for (int i = 0; i <= CHAR_WIDTH; i++) {
            if (!pixel_set(qfont, i, j-1) &&
                 pixel_set(qfont, i, j  ) &&
                visited[edge_index(i, j, 0)] != generation) {

                int sx = i, sy = j, sd = 0;
                int cx = sx, cy = sy, d = sd;

                v4p_addJump(poly);
                do {
                    visited[edge_index(cx, cy, d)] = generation;
                    v4p_addPoint(poly,
                        x + SCALE(cx, whole_x, rem_x, CHAR_WIDTH),
                        y + SCALE(cy, whole_y, rem_y, CHAR_HEIGHT));

                    int nx, ny, nd;
                    switch (d) {
                        case 0: // R: step to (cx+1, cy)
                            nx = cx+1; ny = cy;
                            if      (pixel_set(qfont, nx,   ny-1)) { nd=3; } // NE -> turn U
                            else if (pixel_set(qfont, nx,   ny  )) { nd=0; } // SE -> stay R
                            else                                    { nd=1; } //    -> turn D
                            break;
                        case 1: // D: step to (cx, cy+1)
                            nx = cx; ny = cy+1;
                            if      (pixel_set(qfont, nx,   ny  )) { nd=0; } // SE -> turn R
                            else if (pixel_set(qfont, nx-1, ny  )) { nd=1; } // SW -> stay D
                            else                                    { nd=2; } //    -> turn L
                            break;
                        case 2: // L: step to (cx-1, cy)
                            nx = cx-1; ny = cy;
                            if      (pixel_set(qfont, nx-1, ny  )) { nd=1; } // SW -> turn D
                            else if (pixel_set(qfont, nx-1, ny-1)) { nd=2; } // NW -> stay L
                            else                                    { nd=3; } //    -> turn U
                            break;
                        case 3: // U: step to (cx, cy-1)
                            nx = cx; ny = cy-1;
                            if      (pixel_set(qfont, nx-1, ny-1)) { nd=2; } // NW -> turn L
                            else if (pixel_set(qfont, nx,   ny-1)) { nd=3; } // NE -> stay U
                            else                                    { nd=0; } //    -> turn R
                            break;
                    }
                    cx = nx; cy = ny; d = nd;
                } while (cx != sx || cy != sy || d != sd);

                v4p_addPoint(poly,
                    x + SCALE(sx, whole_x, rem_x, CHAR_WIDTH),
                    y + SCALE(sy, whole_y, rem_y, CHAR_HEIGHT));
            }
        }
    }
    return poly;
}

// Helper function to convert a digit (0-9) to character
static char digitToChar(int digit) {
    return '0' + digit;
}

// Helper function to extract digits from an integer (right to left)
static void extractDigits(int value, char* buffer, int bufferSize) {
    int i = 0;
    int isNegative = 0;

    // Handle negative numbers
    if (value < 0) {
        isNegative = 1;
        value = -value;
    }

    // Extract digits in reverse order
    do {
        int digit = value % 10;
        buffer[i++] = digitToChar(digit);
        value = value / 10;
    } while (value > 0 && i < bufferSize - 1);

    // Add minus sign if needed
    if (isNegative && i < bufferSize - 1) {
        buffer[i++] = '-';
    }

    // Null-terminate
    buffer[i] = '\0';
}

// Function to reverse a string in place
static void reverseString(char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }

    for (int i = 0; i < length / 2; i++) {
        char temp = str[i];
        str[i] = str[length - 1 - i];
        str[length - 1 - i] = temp;
    }
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

// Function to draw an integer as digits
V4pPolygonP qfontDefinePolygonFromInt(int value,
                                     V4pPolygonP poly,
                                     V4pCoord x,
                                     V4pCoord y,
                                     V4pCoord width,
                                     V4pCoord height,
                                     V4pCoord interleave) {
    char buffer[12];  // Enough for 32-bit int: -2147483648

    // Extract digits (in reverse order)
    extractDigits(value, buffer, sizeof(buffer));

    // Reverse to get correct order
    reverseString(buffer);

    // Draw each character
    qfontDefinePolygonFromString(buffer, poly, x, y, width, height, interleave);

    return poly;
}
