#ifndef QFONT_H
#define QFONT_H

#include "v4p.h"

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 5

PolygonP qfontDefinePolygonFromChar(char c,
                                    PolygonP poly,
                                    Coord x,
                                    Coord y,
                                    Coord width,
                                    Coord height);

PolygonP qfontDefinePolygonFromString(char* s,
                                      PolygonP poly,
                                      Coord x,
                                      Coord y,
                                      Coord width,
                                      Coord height,
                                      Coord interleave);

#endif  // QFONT_H