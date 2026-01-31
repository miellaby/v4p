#ifndef V4PSERIAL_H
#define V4PSERIAL_H

#include "v4p.h"

char* v4pPolygonEncodePoints(V4pPolygonP p, int scale);
V4pPolygonP v4pPolygonDecodePoints(V4pPolygonP p, char* s, int scale);
V4pPolygonP v4pQuickPolygon(V4pProps t, V4pColor col, V4pLayer pz, char* s, int scale);
V4pPolygonP v4pDecodePolygon(char* s, int scale);
char* v4pEncodePolygon(V4pPolygonP p, int scale);
V4pPolygonP v4pPolygonDecodeSVGPath(V4pPolygonP p, char* path, int scale);

#endif
