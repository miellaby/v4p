#ifndef V4PSERIAL_H
#define V4PSERIAL_H

#include "v4p.h"

char* v4p_encodePoints(V4pPolygonP p, int scale);
V4pPolygonP v4p_decodePoints(V4pPolygonP p, const char* s, int scale);
V4pPolygonP v4p_quickPolygon(V4pProps t, V4pColor col, V4pLayer pz, const char* s, int scale);
V4pPolygonP v4p_decodePolygon(const char* s, int scale);
char* v4p_encodePolygon(V4pPolygonP p, int scale);
V4pPolygonP v4p_decodeSVGPath(V4pPolygonP p, const char* path, float scale);

#endif
