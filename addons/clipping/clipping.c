// Clipping addon for V4P
// Contains all clipping-related functionality

#include "clipping.h"
#include "_v4p.h"

// Simplified clipEdge for axis-aligned clipping
static V4pPointP clipEdge(V4pPointP subject, bool isVertical, V4pCoord clipCoord, bool isMinEdge) {
    V4pPointP result = NULL;
    V4pPointP prev = NULL;
    V4pPointP current = subject;
    V4pPointP subStart = subject;

    // We clip sub-path by sub-path because of JUMP points that separates multiple sub-paths.
    // Nested loop closes every sub-path.

    // For each sub-path we need: its last real point and its inside state.
    // We do two things:
    //   1. Scan ahead to find sub-path tail for closure init.
    //   2. Process points normally, resetting on JUMP.

    while (subStart != NULL) {
        // Find the last real point of this sub-path (before next JUMP or end)
        V4pPointP lastPoint = NULL;
        V4pPointP scan = subStart;
        int pointCount = 0;

        // Track arc information
        V4pPointP arcCenter = NULL;  // Current arc center point
        V4pPointP ultimateCenter = NULL;  // center to be put at the result list end
        while (scan != NULL) {
            if (scan->x == V4P_NIL && scan->y == V4P_NIL) {
                break;  // JUMP
            }

            if (V4P_IS_ARC_CENTER(scan)) {  // Center isn't a last point for loop closing
                arcCenter = scan;
            } else {
                lastPoint = scan;
                arcCenter = NULL;
            }
            pointCount++;
            scan = scan->next;
        }

        // Compute prevPoint/prevInside from this sub-path's last point
        V4pPointP prevPoint = lastPoint;
        bool prevInside = false;
        if (prevPoint != NULL) {
            V4pCoord lx = prevPoint->x;
            V4pCoord ly = prevPoint->y;
            if (isVertical) {
                prevInside = isMinEdge ? (lx >= clipCoord) : (lx < clipCoord);
            } else {
                prevInside = isMinEdge ? (ly >= clipCoord) : (ly < clipCoord);
            }
        }

        // Process this sub-path's points
        current = subStart;
        while (current != NULL) {
            // --- JUMP point: emit it and break to next sub-path ---
            if (current->x == V4P_NIL && current->y == V4P_NIL) {
                // Only emit JUMP if output is non-empty (avoids leading JUMPs)
                if (result != NULL) {
                    V4pPointP jump = v4p_newPoint(V4P_NIL, V4P_NIL, 0, 0);
                    prev->next = jump;
                    prev = jump;
                }
                subStart = current->next;  // Advance outer loop past JUMP
                break;
            }

            if (V4P_IS_ARC_CENTER(current)) {
                // Store arc center information for potential arc intersection
                arcCenter = current;
                current = current->next;
                // skip to the next point. Note prevPoint/prevInside is kept as is, corresponding to the actual previous
                // point
                continue;
            }

            V4pCoord x = current->x;
            V4pCoord y = current->y;
            bool currentInside;

            if (isVertical) {
                currentInside = isMinEdge ? (x >= clipCoord) : (x < clipCoord);
            } else {
                currentInside = isMinEdge ? (y >= clipCoord) : (y < clipCoord);
            }

            if (prevPoint != NULL && arcCenter != NULL && prevInside && currentInside) {
                // both arc ends are inside, one includes the arcCenter back
                v4p_trace(TRANSFORM, "both arc ends are inside, one includes the arcCenter back\n");

                V4pPointP arcCenterPoint = v4p_newPoint(arcCenter->x, arcCenter->y, arcCenter->a, arcCenter->b);
                if (prevPoint == lastPoint) {
                    // special case: one puts the center at the end of the list, like in the initial list
                    v4p_trace(TRANSFORM,
                              "special case: one puts the center at the end of the list, like in the initial list\n");
                    ultimateCenter = arcCenterPoint;
                } else {
                    if (result == NULL) {
                        result = arcCenterPoint;
                    } else {
                        prev->next = arcCenterPoint;
                    }
                    prev = arcCenterPoint;
                }
            }

            // Intersection on inside/outside transition
            if (prevPoint != NULL && (prevInside != currentInside)) {
                V4pCoord prevX = prevPoint->x;
                V4pCoord prevY = prevPoint->y;
                v4p_trace(TRANSFORM, "Intersection on inside/outside transition (%d,%d)-(%d,%d)\n", prevX, prevY, x, y);

                // Compute intersection with the clip edge
                V4pPointP intersection = v4p_newPoint(0, 0, 0, 0);

                // Check if we have an arc transition (both prev and current are arc-ends)
                bool isArcTransition = (arcCenter != NULL);
                V4pPointP arcCenterPoint = NULL;
                if (isArcTransition) {
                    // For arc transitions, we need to preserve the arc structure
                    // First, add the arc center point to maintain the arc relationship
                    arcCenterPoint = v4p_newPoint(arcCenter->x, arcCenter->y, arcCenter->a, arcCenter->b);
                    v4p_trace(TRANSFORM, "arc intersection with center (%d, %d)\n", arcCenterPoint->x,
                              arcCenterPoint->y);

                    if (prevInside) {
                        // one puts arcCenterPoint between the previous point and the intersection
                        v4p_trace(TRANSFORM, "put arcCenterPoint between the previous point and the intersection\n");

                        if (prevPoint == lastPoint) {
                            // special case: one puts the center at the end of the list, like in the initial list
                            v4p_trace(TRANSFORM,
                                      "special case: one puts the center at the end of the list, like in the initial "
                                      "list\n");

                            ultimateCenter = arcCenterPoint;
                        } else {
                            if (result == NULL) {
                                result = arcCenterPoint;
                            } else {
                                prev->next = arcCenterPoint;
                            }
                        }
                        prev = arcCenterPoint;
                    }

                    // Now compute the intersection point
                    V4pCoord edge = clipCoord - (! isMinEdge);
                    V4pCoord cx = arcCenterPoint->x;
                    V4pCoord cy = arcCenterPoint->y;
                    uint32_t a = (uint32_t) arcCenterPoint->a;
                    uint32_t b = (uint32_t) arcCenterPoint->b;
                    uint32_t a2 = a * a;
                    uint32_t b2 = b * b;
                    if (isVertical) {
                        // Solve: ((edge - cx) / a)² + ((py - cy) / b)² = 1
                        // => (py - cy)² = b² * (1 - ((edge - cx) / a)²)
                        // => (py - cy)² = (b² * (a² - (edge - cx)²)) / a²
                        V4pCoord dx = edge - cx;
                        // Use 64-bit intermediates to avoid overflow
                        int64_t disc = b2 * (a2 - (int64_t) dx * dx);  // = b²*(a² - dx²)

                        // sqrt(disc) / a  — keep fixed-point precision
                        V4pCoord dy = (V4pCoord) isqrt32(disc / (int64_t) a2);
                        // Pick the root closer to the linear-interpolated y
                        V4pCoord yMid = (prevY + y) / 2;
                        V4pCoord y1 = cy + dy;
                        V4pCoord y2 = cy - dy;
                        intersection->x = edge;
                        intersection->y = (abs(y1 - yMid) <= abs(y2 - yMid)) ? y1 : y2;
                        v4p_trace(TRANSFORM,
                                  "isVertical: intersection for cx,cy,a,b,dx,disc,dy,yMid,y1,y2"
                                  "=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d => y=%d\n",
                                  cx, cy, a, b, dx, disc, dy, yMid, y1, y2, intersection->y);
                    } else {
                        // Solve: ((px - cx) / a)² + ((edge - cy) / b)² = 1
                        // => (px - cx)² = a² * (1 - ((edge - cy) / b)²)
                        V4pCoord dy = edge - cy;
                        int64_t disc = a2 * (b2 - (int64_t) dy * dy);  // = a²*(b² - dy²)

                        V4pCoord dx = (V4pCoord) isqrt32(disc / (int64_t) b2);
                        V4pCoord xMid = (prevX + x) / 2;
                        V4pCoord x1 = cx + dx;
                        V4pCoord x2 = cx - dx;
                        intersection->x = (abs(x1 - xMid) <= abs(x2 - xMid)) ? x1 : x2;
                        intersection->y = edge;
                        v4p_trace(TRANSFORM,
                                  "isHorizontal: intersection for cx,cy,a,b,dy,disc,dx,xMid,x1,x2"
                                  "=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d => x=%d\n",
                                  cx, cy, a, b, dy, disc, dx, xMid, x1, x2, intersection->x);
                    }
                } else {
                    // Standard line intersection
                    v4p_trace(TRANSFORM, "standard line intersection\n");
                    if (isVertical) {
                        V4pCoord edge = clipCoord - (! isMinEdge);
                        V4pCoord t = (edge - prevX) * 256 / (x - prevX);
                        intersection->x = edge;
                        intersection->y = prevY + (t * (y - prevY)) / 256;
                    } else {
                        V4pCoord edge = clipCoord - (! isMinEdge);
                        V4pCoord t = (edge - prevY) * 256 / (y - prevY);
                        intersection->x = prevX + (t * (x - prevX)) / 256;
                        intersection->y = edge;
                    }
                }
                v4p_trace(TRANSFORM, "intersection (%d,%d)\n", intersection->x, intersection->y);

                if (result == NULL) {
                    result = intersection;
                } else {
                    prev->next = intersection;
                }
                prev = intersection;

                if (isArcTransition && currentInside) {
                    // one puts arcCenterPoint between the intersection and the current point
                    v4p_trace(TRANSFORM, "arcCenterPoint between the intersection and the current point\n");
                    if (result == NULL) {
                        result = arcCenterPoint;
                    } else {
                        prev->next = arcCenterPoint;
                    }
                    prev = arcCenterPoint;
                }
            }

            // Emit current point if inside
            if (currentInside) {
                V4pPointP newPoint = v4p_newPoint(x, y, 0, 0);

                if (result == NULL) {
                    result = newPoint;
                } else {
                    prev->next = newPoint;
                }
                prev = newPoint;
            }

            prevPoint = current;
            prevInside = currentInside;
            current = current->next;
            arcCenter = NULL;
        }

        if (ultimateCenter != NULL) {
            v4p_trace(TRANSFORM, "Ultimate center (%d, %d)\n", ultimateCenter->x, ultimateCenter->y);
            if (result == NULL) {
                result = ultimateCenter;
            } else {
                prev->next = ultimateCenter;
            }
            prev = ultimateCenter;
            ultimateCenter->next = NULL;
        }

        // If we reached end of list (no JUMP broke us out), exit outer loop
        if (current == NULL) break;
    }

    return result;
}

// Clip a polygon against a rectangle using Sutherland-Hodgman algorithm
V4pPolygonP v4p_recPolygonClipClone(bool estSub, V4pPolygonP p, V4pPolygonP c, V4pCoord x0, V4pCoord y0, V4pCoord x1,
                                    V4pCoord y1) {
    V4pPointP sp, sc;
    V4pPointP clippedPoints = NULL;

    if (p->miny == V4P_NIL) {
        v4p_computeLimits(p);
    }

    // If p is not entirely into the clip region
    if (p->minx < x0 || p->maxx > x1 || p->miny < y0 || p->maxy > y1) {
        // If p is entirely outside the clip region, clear c points and return
        if (p->maxx < x0 || p->minx > x1 || p->maxy < y0 || p->miny > y1) {
            clippedPoints = NULL;  // all points clipped

        } else {
            // Copy points from p to a temporary list
            sp = p->point1;
            V4pPointP* tail = &clippedPoints;  // Pointer to the end of the list
            while (sp) {
                V4pPointP newPoint = v4p_newPoint(sp->x, sp->y, sp->a, sp->b);
                *tail = newPoint;
                tail = &newPoint->next;
                sp = sp->next;
            }

            // Clip against each edge of the rectangle
            // Order: top, right, bottom, left (clockwise)
            v4p_trace(TRANSFORM, "clipping Top edge (horizontal, min y)\n");
            clippedPoints = clipEdge(clippedPoints, false, y0, true);  // Top edge (horizontal, min y)
            v4p_trace(TRANSFORM, "clipping Right edge (vertical, max x)\n");
            clippedPoints = clipEdge(clippedPoints, true, x1, false);  // Right edge (vertical, max x)
            v4p_trace(TRANSFORM, "clipping Bottom edge (horizontal, max y)\n");
            clippedPoints = clipEdge(clippedPoints, false, y1, false);  // Bottom edge (horizontal, max y)
            v4p_trace(TRANSFORM, "clipping Left edge (vertical, min x)\n");
            clippedPoints = clipEdge(clippedPoints, true, x0, true);  // Left edge (vertical, min x)
        }

        // Replace the points in c with the clipped points
        sc = c->point1;
        while (sc) {
            V4pPointP next = sc->next;
            v4p_destroyPoint(sc);
            sc = next;
        }
        c->point1 = clippedPoints;
        c->miny = V4P_NIL;  // Invalidate computed boundaries
        v4p_changed(c);
    }

    if (estSub && p->next) {
        v4p_recPolygonClipClone(true, p->next, c->next, x0, y0, x1, y1);
    }
    if (p->sub1) {
        v4p_recPolygonClipClone(true, p->sub1, c->sub1, x0, y0, x1, y1);
    }

    return c;
}

// Clip a clone c of a polygon p against a rectangle
V4pPolygonP v4p_clipClone(V4pPolygonP p, V4pPolygonP c, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1) {
    return v4p_recPolygonClipClone(false, p, c, x0, y0, x1, y1);
}

// Clip a polygon against a rectangle
V4pPolygonP v4p_clip(V4pPolygonP p, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1) {
    if (p->parent) {  // If this polygon has a parent
        // Clip relatively to parent
        return v4p_clipClone(p->parent, p, x0, y0, x1, y1);
    } else {
        // Otherwise, clip in place
        return v4p_clipClone(p, p, x0, y0, x1, y1);
    }
}
