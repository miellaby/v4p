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

// Primary trace macro using token concatenation
#define v4p_trace(TAG, FORMAT, ...) v4p_trace_##TAG(FORMAT, ##__VA_ARGS__)

// Define trace macros for specific tags
#ifdef TRACE_SHIFT
#define v4p_trace_SHIFT(FORMAT, ...) v4pi_trace("SHIFT", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_SHIFT(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_OPEN
#define v4p_trace_OPEN(FORMAT, ...) v4pi_trace("OPEN", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_OPEN(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_EDGE
#define v4p_trace_EDGE(FORMAT, ...) v4pi_trace("EDGE", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_EDGE(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_POLYGON
#define v4p_trace_POLYGON(FORMAT, ...) v4pi_trace("POLYGON", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_POLYGON(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_CIRCLE
#define v4p_trace_CIRCLE(FORMAT, ...) v4pi_trace("CIRCLE", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_CIRCLE(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_COLLISION
#define v4p_trace_COLLISION(FORMAT, ...) v4pi_trace("COLLISION", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_COLLISION(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_RENDER
#define v4p_trace_RENDER(FORMAT, ...) v4pi_trace("RENDER", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_RENDER(FORMAT, ...) ((void)0)
#endif

#ifdef TRACE_TRANSFORM
#define v4p_trace_TRANSFORM(FORMAT, ...) v4pi_trace("TRANSFORM", FORMAT, ##__VA_ARGS__)
#else
#define v4p_trace_TRANSFORM(FORMAT, ...) ((void)0)
#endif

// Backward compatibility with v4pi_debug
#ifdef DEBUG
#define v4p_debug(FORMAT, ...) v4pi_debug(FORMAT, ##__VA_ARGS__)
#else
#define v4p_debug(FORMAT, ...) ((void)0)
#endif

#endif // V4P_TRACE_H