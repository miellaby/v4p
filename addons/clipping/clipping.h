// Clipping addon header for V4P
// Contains function declarations for clipping operations

#ifndef V4P_CLIPPING_H
#define V4P_CLIPPING_H

#include "v4p.h"

// Clip a polygon against a rectangle
V4pPolygonP v4p_clip(V4pPolygonP p, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1);

// Clip a clone c of a polygon p against a rectangle
V4pPolygonP v4p_clipClone(V4pPolygonP p, V4pPolygonP c, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1);

// Clip a polygon against a rectangle using Sutherland-Hodgman algorithm
V4pPolygonP v4p_recPolygonClipClone(bool estSub, V4pPolygonP p, V4pPolygonP c, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1);

#endif // V4P_CLIPPING_H