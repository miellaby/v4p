/**
 * v4p = Vectors rendition engine for Pocket
 */
#ifndef V4P_H
#define V4P_H
#include "v4p_ll.h"
#include "v4p_color.h"
#include "v4p_trace.h"

typedef UInt32 V4pLayer;  // Rendering layer (< 32)
typedef UInt32 V4pCollisionLayer;  // Collision layer (< 32)
typedef UInt32 V4pCollisionMask;  // Collision mask (<= 0xFFFFFFFF)

typedef UInt16 V4pFlag;
#define V4P_STANDARD (V4pFlag) 0
#define V4P_HIDDEN (V4pFlag) 2  // invisibles help to gather polygons
#define V4P_TRANSLUCENT (V4pFlag) 4  // to be done
#define V4P_ABSOLUTE (V4pFlag) 0
#define V4P_RELATIVE (V4pFlag) 16  // view (not scene) related coordinates
#define V4P_DISABLED (V4pFlag) 32  // wont be displayed for now
#define V4P_IN_DISABLED (V4pFlag) 64  // ancester disabled
#define V4P_CHANGED (V4pFlag) 128  // definition changed since last rendering

// Quality vs. Perfs Levels
#define V4P_QUALITY_LOW 0
#define V4P_QUALITY_NORMAL 1
#define V4P_QUALITY_HIGH 2

// UX
#define V4P_UX_NORMAL 0
#define V4P_UX_FULLSCREEN 1

typedef V4pFlag V4pProps;

typedef struct v4p_point_s* V4pPointP;
typedef struct v4p_polygon_s* V4pPolygonP;
typedef struct v4p_context_s* V4pContextP;

typedef struct v4p_scene_s {
    char* label;
    V4pPolygonP polygons;
} V4pScene, *V4pSceneP;

typedef struct v4p_point_s {
    V4pCoord x, y;
    V4pPointP next;
} V4pPoint;

#define V4P_NIL ((V4pCoord) INT32_MAX)

/**
 * Variables
 */
extern V4pContextP v4p_defaultContext;  // Default context (set once by v4pinit())
extern V4pSceneP v4p_defaultScene;  // Default scene within default context (set
                                    // once by v4pinit())

extern V4pCoord v4p_displayWidth;  // current display width
extern V4pCoord v4p_displayHeight;  // current display height

/**
 * Functions
 */

// v4p library fundamentals
Boolean v4p_init();
Boolean v4p_init2(int quality, Boolean fullscreen);
void v4p_setContext(V4pContextP);  // change the (default) context
Boolean v4p_render();
void v4p_quit();

// v4p context
V4pContextP v4p_newContext();
void v4p_destroyContext(V4pContextP);
V4pColor v4p_setBGColor(V4pColor bg);
Boolean v4p_setView(V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1);
void v4p_setScene(V4pSceneP s);
V4pSceneP v4p_getScene();

// v4p scene
V4pSceneP v4p_newScene();
void v4p_destroyScene(V4pSceneP);
V4pSceneP v4p_sceneAdd(V4pSceneP, V4pPolygonP);
V4pSceneP v4p_sceneRemove(V4pSceneP, V4pPolygonP);
void v4p_clearScene();

// v4p view
void v4p_viewToAbsolute(V4pCoord x, V4pCoord y, V4pCoord* xa, V4pCoord* ya);
void v4p_absoluteToView(V4pCoord x, V4pCoord y, V4pCoord* xa, V4pCoord* ya);

// v4p polygon
V4pPolygonP v4p_new(V4pProps t, V4pColor col, V4pLayer z);
V4pPolygonP v4p_newDisk(V4pProps t, V4pColor col, V4pLayer z, V4pCoord center_x, V4pCoord center_y, V4pCoord radius);
V4pPolygonP v4p_clone(V4pPolygonP p);
V4pPolygonP v4p_setCollisionMask(V4pPolygonP p, V4pCollisionMask collisionMask);
V4pPolygonP v4p_intoList(V4pPolygonP p, V4pPolygonP* list);
Boolean v4p_outOfList(V4pPolygonP p, V4pPolygonP* list);
V4pPolygonP v4p_addSub(V4pPolygonP parent, V4pPolygonP p);
V4pPolygonP v4p_addNewSub(V4pPolygonP parent, V4pProps t, V4pColor col, V4pLayer z);
Boolean v4p_destroyFromParent(V4pPolygonP parent, V4pPolygonP p);
V4pPolygonP v4p_destroyPointFrom(V4pPolygonP p, V4pPointP s);
V4pProps v4p_putProp(V4pPolygonP p, V4pProps i);
V4pProps v4p_removeProp(V4pPolygonP p, V4pProps i);
V4pProps v4p_setRelative(V4pPolygonP p, Boolean relative);
V4pPointP v4p_addPoint(V4pPolygonP p, V4pCoord x, V4pCoord y);
V4pPointP v4p_addJump(V4pPolygonP p);
V4pPointP v4p_movePoint(V4pPolygonP p, V4pPointP s, V4pCoord x, V4pCoord y);
V4pColor v4p_setColor(V4pPolygonP p, V4pColor c);
V4pLayer v4p_setLayer(V4pPolygonP p, V4pLayer z);
V4pCoord v4p_setRadius(V4pPolygonP p, V4pCoord radius);
Boolean  v4p_setVisibility(V4pPolygonP p, Boolean visible);
V4pPointP v4p_getPoints(V4pPolygonP p);
V4pLayer v4p_getLayer(V4pPolygonP p);
V4pCollisionMask v4p_getCollisionMask(V4pPolygonP p);
UInt32 v4p_getId(V4pPolygonP p);
V4pColor v4p_getColor(V4pPolygonP p);
V4pPolygonP v4p_getLimits(V4pPolygonP p, V4pCoord* minx, V4pCoord* maxx, V4pCoord* miny, V4pCoord* maxy);
V4pProps v4p_enable(V4pPolygonP p);
V4pProps v4p_disable(V4pPolygonP p);

// Visibility check
Boolean v4p_isVisible(V4pPolygonP p);

// Polygon tree traversal functions
V4pPolygonP v4p_getFirstSub(V4pPolygonP parent);
V4pPolygonP v4p_getNextSub(V4pPolygonP current);

// transformation
V4pPolygonP v4p_transformClone(V4pPolygonP p, V4pPolygonP c, V4pCoord dx, V4pCoord dy, int angle, V4pLayer dz,
                               V4pCoord zoom_x, V4pCoord zoom_y);
V4pPolygonP v4p_transform(V4pPolygonP p, V4pCoord dx, V4pCoord dy, int angle, V4pLayer dz, V4pCoord zoom_x,
                          V4pCoord zoom_y);
V4pPolygonP v4p_centerPolygon(V4pPolygonP p);

// anchor point management
V4pPolygonP v4p_setAnchorToCenter(V4pPolygonP p);
V4pPolygonP v4p_setAnchor(V4pPolygonP p, V4pCoord x, V4pCoord y);

// helpers & combo
V4pPolygonP v4p_rect(V4pPolygonP p, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1);
V4pPolygonP v4p_add(V4pPolygonP p);
V4pPolygonP v4p_remove(V4pPolygonP);
V4pPolygonP v4p_sceneAddNewPoly(V4pSceneP, V4pProps t, V4pColor col, V4pLayer z);
V4pPolygonP v4p_sceneAddNewDisk(V4pSceneP, V4pProps t, V4pColor col, V4pLayer z, V4pCoord center_x, V4pCoord center_y,
                                V4pCoord radius);
V4pPolygonP v4p_sceneAddClone(V4pSceneP, V4pPolygonP p);
V4pPolygonP v4p_addNew(V4pProps t, V4pColor col, V4pLayer z);
V4pPolygonP v4p_addClone(V4pPolygonP p);
Boolean v4p_destroy(V4pPolygonP p);
Boolean v4p_destroyFromScene(V4pPolygonP p);

// Collision detection when rendering
typedef void (*V4pCollisionCallback)(V4pCollisionLayer i1, V4pCollisionLayer i2, V4pCoord py, V4pCoord x1, V4pCoord x2,
                                     V4pPolygonP p1, V4pPolygonP p2);

// Collision callback function (see game engine implmentation)
void v4p_setCollisionCallback(V4pCollisionCallback f);

#endif
