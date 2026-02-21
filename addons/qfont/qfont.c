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

#define NA0 "1111   1  1111 1111 1  1 1111 1111 1111 1111 1111 "
#define NA1 "1  1  11     1    1 1  1 1    1       1 1  1 1  1 "
#define NA2 "1 11   1  1111 1111 1111 1111 1111   1  1111 1111 "
#define NA3 "11 1   1  1       1    1    1 1  1  1   1  1    1 "
#define NA4 "1111  111 1111 1111    1 1111 1111  1   1111 1111 "

// !/?,"'.:;&*+-<=>@$
#define PA0 " 11     1  11       1 1    1        11   11   11  1  1   1  "
#define PA1 " 11    11 1  1      1 1    1        11   11  1  1  11    1  "
#define PA2 " 11   11    1       1 1   1                   1   1111  111 "
#define PA3 "     11          1             11   11    1  1 1   11    1  "
#define PA4 " 11  1      1   1              11   11   1    111 1  1   1  "

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
    if (!c || c == ' ')
        return -1;
    
    // Direct index calculation instead of loop
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';  // A=0, B=1, ..., Z=25
    } else if (c >= '0' && c <= '9') {
        return 26 + (c - '0');  // 0=26, 1=27, ..., 9=35
    } else {
        // Handle special characters with direct mapping
        // !/?,"'.:;&*+-<=>@$
        switch (c) {
            case '!': return 36;
            case '/': return 37;
            case '?': return 38;
            case ',': return 39;
            case '"': return 40;
            case '\'': return 41;
            case '.': return 42;
            case ':': return 43;
            case ';': return 44;
            case '&': return 45;
            case '*': return 46;
            case '+': return 47;
            case '-': return 48;
            case '<': return 49;
            case '=': return 50;
            case '>': return 51;
            case '@': return 52;
            case '$': return 53;
            default: return 46;  // * for unknown characters
        }
    }
}

V4pPolygonP qfontDefinePolygonFromChar(char c,
                                    V4pPolygonP poly,
                                    V4pCoord x,
                                    V4pCoord y,
                                    V4pCoord width,
                                    V4pCoord height) {
    int i, j, down = 0;
    int ic = ichar(c);
    if (ic == -1)
        return poly;  // V4P_WHITE char, no edge

    v4p_addJump(poly);
    for (i = 0; i <= CHAR_WIDTH; i++) {
        Boolean down = 0;
        // v4p_addJump (poly);
        for (j = 0; j < CHAR_HEIGHT; j++) {
            int idx = ic * (1 + CHAR_WIDTH) + i;
            if (! down && qfont[j][idx] != qfont[j][1 + idx]) {
                v4p_addPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
                down = 1;
            } else if (down && qfont[j][idx] == qfont[j][1 + idx]) {
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
