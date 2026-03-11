# Scanline Ellipse Arc Implementation

Scanline-driven adaptation of McIlroy's incremental ellipse algorithm.
McIlroy 1987: https://www.cs.dartmouth.edu/~doug/155.pdf

Original algorithm traces the ellipse curve freely (E/S/SE steps).
Here y is driven externally by the scanline, so only the EV condition
is needed to advance x — reducing the inner loop to additions only.

## Overview

This implementation replaces the previous `isqrt`-based circle arc rendering with McIlroy's fully incremental integer algorithm for ellipse arcs. The approach eliminates all runtime trigonometric and square root operations, using only integer arithmetic per scanline.

Arcs are encoded as 3 consecutive points `[a, c, b]`
where `c` (center) is flagged.

## ActiveEdge Structure

The `ActiveEdge` struct has been updated with McIlroy algorithm fields:

```c
struct {
    V4pCoord cvx, cvy;  // center in view coordinates
    V4pCoord a, b;       // semi-axes in view
    V4pCoord a2, b2;     // a² and b², precomputed
    V4pCoord ea;         // ceil(a²/4), precomputed
    V4pCoord t;          // McIlroy accumulator
    V4pCoord ex;         // x offset from center (always >= 0)
    V4pCoord ey;         // y offset from center (always >= 0)
    int8_t   xdir;       // +1 = right side, -1 = left side
    int8_t   ydir;       // -1 = top-to-bottom, +1 = bottom-to-top
} arc;
```

## Algorithm Flow

When an arc edge enters the scanline:

1. **Compute semi-axes and direction:**
   ```c
   ae->as.arc.a = abs(avx - cvx);  // Semi-axis x
   ae->as.arc.b = abs(avy - cvy);  // Semi-axis y
   ae->as.arc.xdir = (avx >= cvx) ? 1 : -1;
   ae->as.arc.ydir = (avy >= cvy) ? 1 : -1;
   ```

2. **Precompute constants:**
   ```c
   long a2 = (long)ae->as.arc.a * ae->as.arc.a;
   long b2 = (long)ae->as.arc.b * ae->as.arc.b;
   ae->as.arc.a2 = (V4pCoord)a2;
   ae->as.arc.b2 = (V4pCoord)b2;
   ae->as.arc.ea = (V4pCoord)((a2 + 3) / 4);  // ceil(a²/4)
   ```

3. **Closed-form initialization:**
   - Start from (ex=0, ey=b) position
   - Jump to actual entry y offset `ey0` using closed-form calculation
   - Advance `ex` until ellipse condition is satisfied

### 3. Scanline Stepping (v4p_render)
For each scanline, the algorithm performs:

```c
// Step y offset and update accumulator
 ae->as.arc.ey--;
 ae->as.arc.t += ae->as.arc.a2 * (-2 * ae->as.arc.ey);

// EV drain: advance x until ellipse is tracked
 while (ae->as.arc.t + ae->as.arc.b2 * ae->as.arc.ex
        <= -(ae->as.arc.ea + ae->as.arc.b2)) {
     ae->as.arc.ex++;
     ae->as.arc.t += ae->as.arc.b2 * (2 * ae->as.arc.ex);
 }

// Update edge position
 ae->x = ae->as.arc.cvx + ae->as.arc.xdir * ae->as.arc.ex;
```

## Performance Characteristics

- **Per-scanline operations:** 2 additions, 1 multiplication, 1 comparison (best case)
- **Worst-case:** Multiple x steps per scanline (EV drain loop)
- **No floating point**
- **No trigonometry**

## Limitations

- Arcs must be convex (standard ellipse segments)
- Center point must be between start and end points
- Semi-axes are computed from start point only
- No support for rotated ellipses (axes must be axis-aligned)

## Future Enhancements

Possible improvements:
- Support for rotated ellipses via coordinate transformation
- Adaptive step size for better performance on shallow arcs
- Arc quality metrics for automatic segment selection
- Serialization format for arc-encoded polygons

## References

1. McIlroy, M. D. (1987). "A Method for Generating Function Plots". Dartmouth College. https://www.cs.dartmouth.edu/~doug/155.pdf

## Usage Example

```c
V4pPolygonP p = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 1);
v4p_addPoint(p, start_x, start_y);                    // Start point
v4p_addPointFlag(p, center_x, center_y, V4P_ARC_CENTER_FLAG); // Center (flagged)
v4p_addPoint(p, end_x, end_y);                      // End point
```
