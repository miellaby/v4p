/**
 * ASCII character map for qfont
 * Based on standard ASCII characters (man ascii)
 */

#include "v4p.h"
#include "v4pi.h"
#include "qfont.h"

// ASCII character map - 4x5 pixel representation for each character
// 5 bytes each byte represents a row of 4 pixels (4 bits used)
const UInt8 char_map[96][5] = {
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
const UInt8* qfont_get_char(char c) {
    
    if (c >= 32 && c < 127) {
        return char_map[c - 32];
    }
    return char_map[127];  // Return the "unknown" character for non-printable characters
}

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 5

V4pPolygonP qfontDefinePolygonFromChar(char c,
                                    V4pPolygonP poly,
                                    V4pCoord x,
                                    V4pCoord y,
                                    V4pCoord width,
                                    V4pCoord height) {
    int i, j, down = 0;
    if (c == 32) {
        return poly;  // V4P_WHITE char, no edge
    }
    const UInt8* qfont = qfont_get_char(c);

    v4p_addJump(poly);
    for (i = 0; i <= CHAR_WIDTH; i++) {
        Boolean down = 0;
        // v4p_addJump (poly);
        for (j = 0; j < CHAR_HEIGHT; j++) {
            UInt8 row = (qfont[j] << 1) >> (CHAR_WIDTH - i);  // Shift left to simplify edge detection
            Boolean edge = (row & 1) ^ ((row & 2) >> 1);
            if (! down && edge) {
                v4p_addPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
                down = 1;
            } else if (down && ! edge) {
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
