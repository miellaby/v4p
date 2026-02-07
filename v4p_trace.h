/**
 * V4P Trace System - Compile-time tag filtering
 *
 * Usage:
 *   v4p_trace(SHIFT, "Shifting edge %p by %d\n", edge, amount);
 *
 * Compilation:
 *   make TRACE="SHIFT,OPEN"  # Enable specific trace tags
 */

#ifndef V4P_TRACE_H
#define V4P_TRACE_H

#include "v4p_ll.h"

// Forward declaration
void v4pi_trace(const char* tag, const char* format, ...);

#ifndef TRACE_SCAN
    #define TRACE_SCAN 0
#endif
#ifndef TRACE_SHIFT
    #define TRACE_SHIFT 0
#endif
#ifndef TRACE_OPEN
    #define TRACE_OPEN 0
#endif
#ifndef TRACE_EDGE
    #define TRACE_EDGE 0
#endif
#ifndef TRACE_POLYGON
    #define TRACE_POLYGON 0
#endif
#ifndef TRACE_CIRCLE
    #define TRACE_CIRCLE 0
#endif
#ifndef TRACE_COLLISION
    #define TRACE_COLLISION 0
#endif
#ifndef TRACE_RENDER
    #define TRACE_RENDER 0
#endif
#ifndef TRACE_TRANSFORM
    #define TRACE_TRANSFORM 0
#endif

#define TRACE_ENABLED_1(TAG, FMT, ...) v4pi_trace(#TAG, FMT, ##__VA_ARGS__)
#define TRACE_ENABLED_0(TAG, FMT, ...) ((void) 0)

// This glue macro joins "TRACE_ENABLED_" with the 1 or 0 defined for the tag
#define V4P_GLUE(A, B) A##B
#define V4P_EVAL(A, B) V4P_GLUE(A, B)

#define v4p_trace(TAG, FMT, ...) \
    V4P_EVAL(TRACE_ENABLED_, TRACE_##TAG)(TAG, FMT, ##__VA_ARGS__)                                                    

#endif // V4P_TRACE_H