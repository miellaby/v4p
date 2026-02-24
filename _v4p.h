#ifndef _V4P_H
#define _V4P_H
// V4P internal definitions (not to be included by users)
// Exception: debug addon may include this for debugging purposes
#ifndef _V4P_C
    #ifndef V4P_DEBUG_ADDON
        #warning "This file should only be included by v4p.c or debug addon"
    #endif
#endif
#include "lowmath.h"
#include "quickheap.h"
#include "sortable.h"
#include "quicktable.h"
#include "v4p.h"
#include "v4pi.h"

// Forward declarations
V4pPolygonP v4p_computeLimits(V4pPolygonP p);

static V4pCollisionCallback collisionCallback = NULL;

#define YHASH_SIZE 512
#define YHASH_MASK 511

// Polygon type
typedef struct v4p_polygon_s {
    V4pProps props;  // Property flags
    V4pPointP point1;  // List of points (only 1 for disk)
    V4pColor color;  // V4pColor (any data needed by the drawing function)
    V4pCoord radius;  // Point radius (disk = 1 point with positive radius)
    V4pLayer z;  // Depth
    V4pCollisionLayer collisionMask;  // Collision mask
    V4pPolygonP sub1;  // Subs list
    V4pPolygonP next;  // Subs list link
    V4pPolygonP parent;  // Parent polygon reference (for clones)
    V4pCoord anchor_x, anchor_y;  // Rotation anchor point (default: 0,0)
    V4pCoord minx, maxx, miny, maxy;  // Bounding box
    V4pCoord minyv, maxyv;  // Vertical boundaries in view coordinates
    List ActiveEdge1;  // ActiveEdges list
    UInt32 id;  // Unique polygon ID
} Polygon;

// ActiveEdge type
typedef struct activeEdge_s {
    V4pPolygonP p;  // Parent polygon
    V4pCoord x0, y0, x1, y1;  // Vector coordinates; absolute or relative depending
                              // on the belonging list
    V4pCoord x0v, y0v, x1v, y1v;  // Vector coordinates in view
    // Bresenham algorithm variables
    V4pCoord x;  // Current x
    V4pCoord o1;  // Offset when accumulator under limit
    V4pCoord o2;  // Offset when accumulator cross the limit
    V4pCoord s;  // Accumulator
    V4pCoord h;  // Height of the edge (scanline count)
    V4pCoord r1;  // Remaining 1
    V4pCoord r2;  // r1 - dy
    Boolean circle;  // Right or left edge of a circle (coordinates are its
                     // bounding box)
} ActiveEdge;

typedef struct activeEdge_s* ActiveEdgeP;

// Balanced BST node for depth-sorted polygons
typedef struct depth_tree_node_s {
    V4pPolygonP polygon;
    V4pLayer depth;  // Full UInt32 depth support
    int height;  // Height for AVL balancing
    struct depth_tree_node_s* left;
    struct depth_tree_node_s* right;
} DepthTreeNode;

typedef struct depth_tree_node_s* DepthTreeNodeP;

// V4P context
typedef struct v4p_context_s {
    V4piContextP display;
    V4pSceneP scene;  // Scene = a polygon set
    // View coordinate system (absolute coordinates)
    V4pCoord viewMinX, viewMinY;  // Top-left corner of view (minimum coordinates)
    V4pCoord viewMaxX, viewMaxY;  // Bottom-right corner of view (maximum coordinates)
    Polygon dummyBgPoly;  // Just for color
    int debug1;
    QuickHeap pointHeap, polygonHeap, activeEdgeHeap, depthTreeNodeHeap;
    List openedAEList;  // ActiveEdge lists
    QuickTable openableAETable;  // ActiveEdge Hash Table
    DepthTreeNodeP openedPolygons;  // AVL tree of active polygons sorted by depth
    V4pCoord viewWidth, viewHeight;  // View dimensions (viewMaxX - viewMinX, viewMaxY - viewMinY)
    // Integer scaling factors for coordinate transformations
    // Uses quotient-remainder technique to avoid overflow (see integer_scaling.md)
    V4pCoord screenToView_wholeX, screenToView_remX;  // Screen-to-View scaling (zoom out, X axis)
    V4pCoord screenToView_wholeY, screenToView_remY;  // Screen-to-View scaling (zoom out, Y axis)
    V4pCoord viewToScreen_wholeX, viewToScreen_remX;  // View-to-Screen scaling (zoom in, X axis)
    V4pCoord viewToScreen_wholeY, viewToScreen_remY;  // View-to-Screen scaling (zoom in, Y axis)
    Boolean scaling;  // Is scaling necessary?
    UInt32 changes;
    UInt32 nextId;
} V4pContext;

/**
 * About screen vs view ratios:
 * divyvu == 0 when view bigger than screen (zoom out)
 * divyvub == 0 when screen bigger than view (zoom in)
 * when zoom in : yline++ ==> y-absolute may be unchanged
 * so when divyvu == 0, y-absolute comparison is avoided
 * when zoom out : yline++ ==>  y-absolute+=x where x>0
 */
#define V4P_CHANGED_ABSOLUTE 1
#define V4P_CHANGED_RELATIVE 2
#define V4P_CHANGED_VIEW 4
#endif