#ifndef V4P_DEBUG_H
#define V4P_DEBUG_H

#include "v4p.h"

/**
 * Debug function to dump polygon information to console
 * 
 * @param poly The polygon to debug
 * @param name Optional name/identifier for the polygon (e.g., "ship")
 */
void v4p_debugPolygon(V4pPolygonP poly, const char* name);

#endif // V4P_DEBUG_H