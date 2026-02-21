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

#define V4P_SILVER 224
#define V4P_GRAY 225
#define V4P_MAROON 226
#define V4P_PURPLE 227
#define V4P_GREEN 228
#define V4P_CYAN 229
#define V4P_BLACK 215
#define V4P_RED 125
#define V4P_BLUE 95
#define V4P_YELLOW 120
#define V4P_DARK 217
#define V4P_OLIVE 58
#define V4P_FLUO 48
#define V4P_WHITE 0

// Color index to RGB triplet
#define V4P_PALETTE_RGB(index) ((const UInt8*)v4p_palette[index])

// Individual component access
#define V4P_PALETTE_R(index) (v4p_palette[index][0])
#define V4P_PALETTE_G(index) (v4p_palette[index][1])
#define V4P_PALETTE_B(index) (v4p_palette[index][2])

#endif // V4P_PALETTE_H