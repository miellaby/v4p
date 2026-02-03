#ifndef QFONT_H
#define QFONT_H

#include "v4p.h"

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 5

V4pPolygonP qfontDefinePolygonFromChar(char c,
                                    V4pPolygonP poly,
                                    V4pCoord x,
                                    V4pCoord y,
                                    V4pCoord width,
                                    V4pCoord height);

V4pPolygonP qfontDefinePolygonFromString(char* s,
                                      V4pPolygonP poly,
                                      V4pCoord x,
                                      V4pCoord y,
                                      V4pCoord width,
                                      V4pCoord height,
                                      V4pCoord interleave);

// Function to draw an integer as digits
V4pPolygonP qfontDefinePolygonFromInt(int value,
                                     V4pPolygonP poly,
                                     V4pCoord x,
                                     V4pCoord y,
                                     V4pCoord width,
                                     V4pCoord height,
                                     V4pCoord interleave);

#endif  // QFONT_H