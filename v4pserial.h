#ifndef V4PSERIAL_H
#define V4PSERIAL_H

#include "v4p.h"

char*    v4pPolygonEncodePoints(PolygonP p, int scale);
PolygonP v4pPolygonDecodePoints(PolygonP p, char *s, int scale);
PolygonP v4pQuickPolygon(PolygonProps t, Color col, ILayer pz, char* s, int scale);
PolygonP v4pDecodePolygon(char *s, int scale);
char*    v4pEncodePolygon(PolygonP p, int scale);
PolygonP v4pPolygonDecodeSVGPath(PolygonP p, char *path, int scale);

#endif
