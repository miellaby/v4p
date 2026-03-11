#ifndef _V4P_H
#define _V4P_H
// V4P internal definitions (not to be included by users)
// Exception: debug addon may include this for debugging purposes
#ifndef DEBUG
    #ifndef _V4P_C
        #ifndef V4P_DEBUG_ADDON
            #warning "This file should only be included by v4p.c or debug addon"
        #endif
    #endif
#endif
#include "quick/imath.h"
#include "quick/heap.h"
#include "quick/sortable.h"
#include "quick/sorted.h"
#include "quick/table.h"
#include "v4p.h"
#include "v4pi.h"
#include "v4p_trace.h"
#include "v4p_platform.h"

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
    V4pLayer z;  // Depth
    V4pCollisionLayer collisionMask;  // Collision mask
    V4pPolygonP sub1;  // Subs list
    V4pPolygonP next;  // Subs list link
    V4pPolygonP parent;  // Parent polygon reference (for clones)
    V4pCoord anchor_x, anchor_y;  // Rotation anchor point (default: 0,0)
    V4pCoord minx, maxx, miny, maxy;  // Bounding box
    V4pCoord minyv, maxyv;  // Vertical boundaries in view coordinates
    List ActiveEdge1;  // ActiveEdges list
    uint32_t id;  // Unique polygon ID
    uint32_t stroke;  // Stroke width (1 = 1px stroke, 0 = filled)
} Polygon;

// ActiveEdge type
typedef struct activeEdge_s {
    V4pPolygonP p;  // Parent polygon
    V4pCoord ax, ay, bx, by;  // (a->b) Vector coordinates in scene reference
    V4pCoord avx, avy, bvx, bvy;  // Vector coordinates in view
    V4pCoord h;  // Remaining scanlines to process
    V4pCoord x;  // Current x coordinate (in view) at y=scanline
    bool isArc;  // ellipse arc edge
    union {
        struct { // Straight edge (Bresenham)
            V4pCoord o1;  // Offset when accumulator under limit
            V4pCoord o2;  // Offset when accumulator cross the limit
            V4pCoord s;  // Accumulator
            V4pCoord r1;  // Remaining 1
            V4pCoord r2;  // r1 - dy
        } straight;
        struct { // Ellipse arc edge (McIlroy algorithm)
            V4pCoord cx, cy;     // center in screen coordinates
            V4pCoord cvx, cvy;   // center in view coordinates
            V4pCoord a, b;       // semi-axes in view
            V4pCoord a2, b2;     // a² and b², precomputed
            V4pCoord ea;         // ceil(a²/4), precomputed
            V4pCoord t;          // McIlroy accumulator
            V4pCoord ex;         // x offset from center (always >= 0)
            V4pCoord ey;         // y offset from center (always >= 0)
            int8_t   xdir;       // +1 = right side, -1 = left side
            int8_t   ydir;       // -1 = top-to-bottom (usual), +1 = bottom-to-top
        } arc;
    } as;
    bool isStroke;  // If true: plot 1px per scanline, don't toggle fill
} ActiveEdge;

typedef struct activeEdge_s* ActiveEdgeP;

// Forward declaration for Tree (defined in quick/sorted.h)
typedef struct sTree QuickTree;

// V4P context
typedef struct v4p_context_s {
    V4piContextP display;
    V4pSceneP scene;  // Scene = a polygon set
    // View coordinate system (absolute coordinates)
    V4pCoord viewMinX, viewMinY;  // Top-left corner of view (minimum coordinates)
    V4pCoord viewMaxX, viewMaxY;  // Bottom-right corner of view (maximum coordinates)
    V4pColor background;  // background color
    int debug1;
    QuickHeap pointHeap, polygonHeap, activeEdgeHeap;
    List openedAEList;  // ActiveEdge lists
    QuickTable openableAETable;  // ActiveEdge Hash Table
    QuickTree* openedPolygons;  // AVL tree of active polygons sorted by depth
    V4pCoord viewWidth, viewHeight;  // View dimensions (viewMaxX - viewMinX, viewMaxY - viewMinY)
    // Integer scaling factors for coordinate transformations
    // Uses quotient-remainder technique to avoid overflow (see integer_scaling.md)
    V4pCoord screenToView_wholeX, screenToView_remX;  // Screen-to-View scaling (zoom out, X axis)
    V4pCoord screenToView_wholeY, screenToView_remY;  // Screen-to-View scaling (zoom out, Y axis)
    V4pCoord viewToScreen_wholeX, viewToScreen_remX;  // View-to-Screen scaling (zoom in, X axis)
    V4pCoord viewToScreen_wholeY, viewToScreen_remY;  // View-to-Screen scaling (zoom in, Y axis)
    bool scaling;  // Is scaling necessary?
    uint32_t changes;
    uint32_t nextId;
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