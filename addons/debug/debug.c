#include "debug.h"
#define V4P_DEBUG_ADDON  // Define this to allow including _v4p.h
#include "_v4p.h"  // Internal V4P structures (only for debug addon)
#include <stdio.h>

void v4p_debugPolygon(V4pPolygonP poly, const char* name) {
    if (!poly) {
        v4p_debug("Polygon '%s' is NULL!\n", name ? name : "unknown");
        return;
    }

    v4p_debug("=== POLYGON DEBUG INFO: %s ===\n", name ? name : "unknown");
    
    // Basic polygon information
    v4p_debug("Polygon ID: %d, color: %d, layer: %d\n", v4p_getId(poly), poly->color, poly->z);
    v4p_debug("Flags: DISABLED=%d, RELATIVE=%d\n", 
              (poly->props & V4P_DISABLED) != 0, 
              (poly->props & V4P_RELATIVE) != 0);

    // Check if bounds are valid
    if (poly->miny == V4P_NIL) {
        v4p_debug("WARNING: Bounds not computed (miny == NIL)\n");
    } else {
        v4p_debug("Bounds: minx=%d, maxx=%d, miny=%d, maxy=%d\n", poly->minx, poly->maxx, poly->miny, poly->maxy);
    }
    v4p_debug("Anchor: ax=%d, ay=%d\n", poly->anchor_x, poly->anchor_y);

    // Display point coordinates
    V4pPointP pt = poly->point1;
    int point_idx = 0;
    while (pt) {
        v4p_debug("Point %d: (%d,%d) center=%d\n", point_idx++, pt->x, pt->y, V4P_IS_ARC_CENTER(pt));
        pt = pt->next;
    }

    // Check visibility
    bool visible = v4p_isVisible(poly);
    v4p_debug("Visibility: %s\n", visible ? "VISIBLE" : "NOT VISIBLE");

    // Check if polygon has active edges
    v4p_debug("Active edges: %s\n", poly->ActiveEdge1 ? "YES" : "NO");

    // Debug: Loop through and log active edges
    if (poly->ActiveEdge1) {
        List edge_list = poly->ActiveEdge1;
        int edge_count = 0;
        v4p_debug("=== ACTIVE EDGES ===\n");
        while (edge_list) {
            ActiveEdgeP edge = (ActiveEdgeP)edge_list->data;
            v4p_debug("Edge %d: y0=%d, y1=%d, x=%d, h=%d, next=%p\n",
                      edge_count++,
                      edge->ay, edge->by, edge->x, edge->h, (void*)edge_list->next);
            edge_list = edge_list->next;
        }
        v4p_debug("Total edges: %d\n", edge_count);
        v4p_debug("=== END ACTIVE EDGES ===\n");
    }

    // Display view and display information
    v4p_debug("Display: width=%d, height=%d\n", v4p_displayWidth, v4p_displayHeight);

    // Debug sub-polygons recursively
    if (poly->sub1) {
        v4p_debug("ENTERING SUBS of %ld\n", v4p_getId(poly));
        V4pPolygonP sub = poly->sub1;
        int sub_idx = 0;
        while (sub) {
            char sub_name[50];
            snprintf(sub_name, sizeof(sub_name), "%s_sub_%d", name ? name : "poly", sub_idx++);
            v4p_debugPolygon(sub, sub_name);
            sub = sub->next;
        }
        v4p_debug("LEAVING SUBS of %ld\n", v4p_getId(poly));
    }

    v4p_debug("=== END DEBUG INFO ===\n");
}