# Scanline Ellipse Arc Implementation

Scanline-driven adaptation of McIlroy's incremental ellipse algorithm.
McIlroy 1987: https://www.cs.dartmouth.edu/~doug/155.pdf

Original algorithm traces the ellipse curve freely (E/S/SE steps).
Here y is driven externally by the scanline, so only the EV condition
is needed to advance x — reducing the inner loop to additions only.

## Overview

This implementation replaces the previous `isqrt`-based circle arc rendering with McIlroy's fully incremental integer algorithm for ellipse arcs. The approach eliminates all runtime trigonometric and square root operations, using only integer arithmetic per scanline.

Arcs are encoded as 3 consecutive points `[a, c, b]`
where `c` (center) has (ra, rb) additional info (WIP).

### When an arc edge enters the scanline:

- Compute semi-axes and direction
- Precompute constants
- Jump to actual entry y offset `ey0`

### Scanline Stepping (v4p_render)
For each scanline, the algorithm performs:

- Step y offset and update accumulator
- EV drain: advance x until ellipse is tracked
- Update edge position

## Performance Characteristics

- **Per-scanline operations:** 2 additions, 1 multiplication, 1 comparison (best case)
- **Worst-case:** Multiple x steps per scanline (EV drain loop)
- **No floating point**
- **No trigonometry**

## Limitations

- No support for rotated ellipses (axes must be axis-aligned)

## References

1. McIlroy, M. D. (1987). "A Method for Generating Function Plots". Dartmouth College. https://www.cs.dartmouth.edu/~doug/155.pdf

## Usage Example

```c
V4pPolygonP p = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 1);
v4p_addPoint(p, start_x, start_y);                    // Start point
v4p_addEllipseCenter(p, center_x, center_y, a_radius, b_radius); // Center
v4p_addPoint(p, end_x, end_y);                      // End point
```

## TODO
- gaspillage espace stockage point
- clipping (il faut d'abord rajouter les points de franchissement de quadrant et ensuite on peut calculer des intersections par trigo)
- rotation de l'ellipse? (implique de pouvoir stocker des coordonées en plus, ca devient vraiment compliqué)

