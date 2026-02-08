#include "debug.h"
#define V4P_DEBUG_ADDON  // Define this to allow including _v4p.h
#include "_v4p.h"  // Internal V4P structures (only for debug addon)
#include <stdio.h>

void v4p_debugPolygon(V4pPolygonP poly, const char* name) {
    if (!poly) {
        v4pi_debug("Polygon '%s' is NULL!\n", name ? name : "unknown");
        return;
    }

    v4pi_debug("=== POLYGON DEBUG INFO: %s ===\n", name ? name : "unknown");
    
    // Basic polygon information
    v4pi_debug("Polygon ID: %d, color: %d, layer: %d\n", v4p_getId(poly), poly->color, poly->z);
    v4pi_debug("Flags: DISABLED=%d, RELATIVE=%d\n", 
              (poly->props & V4P_DISABLED) != 0, 
              (poly->props & V4P_RELATIVE) != 0);
    v4pi_debug("Bounds: minx=%d, maxx=%d, miny=%d, maxy=%d (NIL=%d)", 
              poly->minx, poly->maxx, poly->miny, poly->maxy, V4P_NIL);

    // Check if bounds are valid
    if (poly->miny == V4P_NIL) {
        v4pi_debug("WARNING: Bounds not computed (miny == NIL)\n");
    }
    v4pi_debug("Anchor: ax=%d, ay=%d, radius=%d\n", 
              poly->anchor_x, poly->anchor_y, poly->radius);

    // Display point coordinates
    V4pPointP pt = poly->point1;
    int point_idx = 0;
    while (pt) {
        v4pi_debug("Point %d: x=%d, y=%d (NIL=%d), x==NIL=%d, y==NIL=%d, (x&y)!=NIL=%d\n", point_idx++, pt->x, pt->y,
                   V4P_NIL, pt->x == V4P_NIL, pt->y == V4P_NIL, (pt->x & pt->y) != V4P_NIL);
        pt = pt->next;
    }

    // Check visibility
    Boolean visible = v4p_isVisible(poly);
    v4pi_debug("Visibility: %s\n", visible ? "VISIBLE" : "NOT VISIBLE");

    // Check if polygon has active edges
    v4pi_debug("Active edges: %s\n", poly->ActiveEdge1 ? "YES" : "NO");

    // Debug: Loop through and log active edges
    if (poly->ActiveEdge1) {
        List edge_list = poly->ActiveEdge1;
        int edge_count = 0;
        v4pi_debug("=== ACTIVE EDGES ===\n");
        while (edge_list) {
            ActiveEdgeP edge = (ActiveEdgeP)edge_list->data;
            v4pi_debug("Edge %d: y0=%d, y1=%d, x=%d, h=%d, next=%p\n",
                      edge_count++,
                      edge->y0, edge->y1, edge->x, edge->h, (void*)edge_list->next);
            edge_list = edge_list->next;
        }
        v4pi_debug("Total edges: %d\n", edge_count);
        v4pi_debug("=== END ACTIVE EDGES ===\n");
    }

    // Display view and display information
    v4pi_debug("Display: width=%d, height=%d\n", v4p_displayWidth, v4p_displayHeight);

    // Debug sub-polygons recursively
    if (poly->sub1) {
        v4pi_debug("ENTERING SUBS of %ld\n", v4p_getId(poly));
        V4pPolygonP sub = poly->sub1;
        int sub_idx = 0;
        while (sub) {
            char sub_name[50];
            snprintf(sub_name, sizeof(sub_name), "%s_sub_%d", name ? name : "poly", sub_idx++);
            v4p_debugPolygon(sub, sub_name);
            sub = sub->next;
        }
        v4pi_debug("LEAVING SUBS of %ld\n", v4p_getId(poly));
    }

    v4pi_debug("=== END DEBUG INFO ===\n");
}