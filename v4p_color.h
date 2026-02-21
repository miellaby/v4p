/**
 * V4P 256-Color Palette
 * 
 * A shared 256-color palette and common color constants
 */

#ifndef V4P_PALETTE_H
#define V4P_PALETTE_H

// v4p_ll.h type definitions
#include "v4p_ll.h"

// V4p color (palette index)
typedef UInt8 V4pColor;

// Shared 256-colors RGB palette
// This palette is inspired by old Palm Computing Devices and provides a consistent color reference across all V4P backends.

extern const UInt8 v4p_palette[256][3];

#define V4P_WHITE 0
#define V4P_SILVER 225
#define V4P_GRAY 226
#define V4P_DARK 215
#define V4P_BLACK 231
#define V4P_RED 125
#define V4P_DARK_RED 161
#define V4P_ORANGE 123
#define V4P_YELLOW 120
#define V4P_LIMEGREEN 210
#define V4P_GREEN 229
#define V4P_DARK_GREEN 213
#define V4P_OLIVE 214
#define V4P_CYAN 90
#define V4P_BLUE 95
#define V4P_LIGHT_BLUE 74
#define V4P_TEAL 230
#define V4P_NAVY 209
#define V4P_PURPLE 228
#define V4P_PINK 9
#define V4P_BROWN 160
#define V4P_MAROON 227

// Color index to RGB triplet
#define V4P_PALETTE_RGB(index) ((const UInt8*)v4p_palette[index])

// Individual component access
#define V4P_PALETTE_R(index) (v4p_palette[index][0])
#define V4P_PALETTE_G(index) (v4p_palette[index][1])
#define V4P_PALETTE_B(index) (v4p_palette[index][2])

// Function to compute palette index from RGB values
V4pColor v4p_rgb_to_palette_index(UInt8 r, UInt8 g, UInt8 b);

#endif // V4P_PALETTE_H