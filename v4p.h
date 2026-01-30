/**
 * v4p = Vectors rendition engine for Pocket
 */
#ifndef V4P_H
#define V4P_H
#include "v4p_ll.h"

typedef UInt16 ILayer;  // < 16
typedef UInt16 ICollide;  // < 16

typedef UInt16 Flags;
typedef Flags PolygonProps;
#define standard (PolygonProps) 0
#define complement (PolygonProps) 1  // substracte/add a region to parent
#define invisible (PolygonProps) 2  // invisibles help to gather polygons
#define translucent (PolygonProps) 4  // to be done
#define absolute (PolygonProps) 0
#define relative (PolygonProps) 16  // view (not scene) related coordinates
#define V4P_DISABLED (Flags) 32  // wont be displayed for now
#define V4P_IN_DISABLED (Flags) 64  // ancester disabled
#define V4P_CHANGED (Flags) 128  // definition changed since last rendering

typedef struct point_s* PointP;
typedef struct polygon_s* PolygonP;
typedef struct activeEdge_s* ActiveEdgeP;
typedef struct v4pContext_s* V4pContextP;

typedef struct scene_s {
    char* label;
    PolygonP polygons;
} V4pScene, *V4pSceneP;

typedef struct point_s {
    Coord x, y;
    PointP next;
} Point;

#define JUMPCOORD ((Coord) ~0)

/**
 * Variables
 */
extern V4pContextP v4p_defaultContext;  // Default context (set once by v4pinit())
extern V4pSceneP v4p_defaultScene;  // Default scene within default context (set
                                    // once by v4pinit())

/**
 * Functions
 */

// v4p library fundamentals
Boolean v4p_init();
void v4p_setContext(V4pContextP);  // note there is a default context
Boolean v4p_render();
void v4p_quit();

// v4p context
V4pContextP v4p_contextNew();
void v4p_contextFree(V4pContextP);
Color v4p_setBGColor(Color bg);
Boolean v4p_setView(Coord x0, Coord y0, Coord x1, Coord y1);
void v4p_setScene(V4pSceneP s);
V4pSceneP v4p_getScene();

// v4p scene
V4pSceneP v4p_sceneNew();
void v4p_sceneFree(V4pSceneP);
V4pSceneP v4p_sceneAdd(V4pSceneP, PolygonP);
V4pSceneP v4p_sceneRemove(V4pSceneP, PolygonP);

// v4p view
void v4p_viewToAbsolute(Coord x, Coord y, Coord* xa, Coord* ya);
void v4p_absoluteToView(Coord x, Coord y, Coord* xa, Coord* ya);

// v4p polygon
PolygonP v4p_new(PolygonProps t, Color col, ILayer z);
PolygonP v4p_newDisk(PolygonProps t,
                     Color col,
                     ILayer z,
                     Coord center_x,
                     Coord center_y,
                     Coord radius);
PolygonP v4p_clone(PolygonP p);
PolygonP v4p_concrete(PolygonP p, ICollide i);
PolygonP v4p_intoList(PolygonP p, PolygonP* list);
Boolean v4p_outOfList(PolygonP p, PolygonP* list);
PolygonP v4p_addSub(PolygonP parent, PolygonP p);
PolygonP v4p_addNewSub(PolygonP parent, PolygonProps t, Color col, ILayer z);
Boolean v4p_destroyFromParent(PolygonP parent, PolygonP p);
PolygonP v4p_destroyPointFrom(PolygonP p, PointP s);
PolygonProps v4p_putProp(PolygonP p, PolygonProps i);
PolygonProps v4p_removeProp(PolygonP p, PolygonProps i);
PointP v4p_addPoint(PolygonP p, Coord x, Coord y);
PointP v4p_addJump(PolygonP p);
PointP v4p_movePoint(PolygonP p, PointP s, Coord x, Coord y);
Color v4p_setColor(PolygonP p, Color c);
Color v4p_setLayer(PolygonP p, ILayer z);
Coord v4p_setRadius(PolygonP p, Coord radius);
PointP v4p_getPoints(PolygonP p);
ILayer v4p_getZ(PolygonP p);
Color v4p_getColor(PolygonP p);
PolygonProps v4p_enable(PolygonP p);
PolygonProps v4p_disable(PolygonP p);

// transformation
PolygonP v4p_transformClone(PolygonP p,
                            PolygonP c,
                            Coord dx,
                            Coord dy,
                            int angle,
                            ILayer dz,
                            Coord zoom_x,
                            Coord zoom_y);
PolygonP v4p_transform(PolygonP p,
                       Coord dx,
                       Coord dy,
                       int angle,
                       ILayer dz,
                       Coord zoom_x,
                       Coord zoom_y);

// anchor point management
PolygonP v4p_setAnchorToCenter(PolygonP p);
PolygonP v4p_setAnchor(PolygonP p, Coord x, Coord y);

// helpers & combo
PolygonP v4p_rect(PolygonP p, Coord x0, Coord y0, Coord x1, Coord y1);
PolygonP v4p_add(PolygonP p);
PolygonP v4p_remove(PolygonP);
PolygonP v4p_sceneAddNewPoly(V4pSceneP, PolygonProps t, Color col, ILayer z);
PolygonP v4p_sceneAddNewDisk(V4pSceneP,
                             PolygonProps t,
                             Color col,
                             ILayer z,
                             Coord center_x,
                             Coord center_y,
                             Coord radius);
PolygonP v4p_sceneAddClone(V4pSceneP, PolygonP p);
Boolean v4p_sceneDel(V4pSceneP, PolygonP p);
PolygonP v4p_addNew(PolygonProps t, Color col, ILayer z);
PolygonP v4p_addClone(PolygonP p);
Boolean v4p_destroy(PolygonP p);

#endif
