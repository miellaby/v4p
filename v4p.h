/*
** v4p = Vectors rendition engine for Pocket
*/
#ifndef V4P_H
#define V4P_H
#include "v4p_ll.h"

/* Abbrevs
** display size = rectangle lineNb*lineWidth
** view = rectangular area defining what part of the scene to display
** scene = a polygons list
** Polygon = closed path of n points, with color and depth
** Color = any data needed by the external drawing function
** Point = x,y coordinates in scene referential
** ActiveEdge : non horizontal edge inside view
** sub : sub-polygon related to a parent polygon (rotation,moving,optimisation)
** z = depth = layer #
** XHeap : reserved memory for X
** XP : X pointer
** IX : X table indice
** XCallBack : external function
** opened polygon : polygon intersected by the scan-line : min(y(points)) < y < max(y(points))
** to be opened polygon : y(scan-line) < min(y(points))
** closed polygon : y(scan-line) > max(y(points))
*/



#define MAX_SOMMET 320
#define MAX_POLY 80
#define MAX_ActiveEdge 200

typedef UInt16 Flags ;
#define standard (Flags)0
#define complement (Flags)1 // substracte/add a region to parent
#define invisible (Flags)2 // invisibles help to gather polygons
#define translucent (Flags)4 // to be done

#define absolute (Flags)0
#define relative (Flags)16 // view (not scene) related coordinates

// RO flags
#define V4P_DISABLED (Flags)32 // wont be displayed for now
#define V4P_IN_DISABLED (Flags)64 // ancester disabled
#define V4P_CHANGED (Flags)128 // definition changed since last rendering

typedef UInt16 ILayer ; // < 16
typedef UInt16 ICollide ; // < 8

//typedef UInt32 Color ;
typedef UInt16 PolygonProps ;
//typedef Int16  Coord ;
typedef struct point_s *PointP ;
typedef struct poly_s *PolygonP ;
typedef struct ba_s *ActiveEdgeP ;
typedef struct v4pContext_s* V4pContextP;


typedef struct point_s {
 Coord x, y ;
 PointP next ;
} Point ;

/******************************
 * Variables
 ******************************/
V4pContextP v4pDefaultContext; // default context (set once by v4pinit())

/******************************
 * Functions
 ******************************/

// v4p library
Boolean v4pInit() ;
void    v4pQuit() ;

// v4p context
int     v4pSetBGColor(Color bg);
void    v4pSetScene(PolygonP *scene) ;
Boolean v4pSetView(Coord x0, Coord y0, Coord x1, Coord y1) ;
Boolean v4pRender() ;

// v4pContext
V4pContextP  v4pContextNew();
void    v4pContextSet(V4pContextP);
void    v4pContextFree(V4pContextP);

// v4pPolygon
PolygonP v4pPolygonNew(PolygonProps t, Color col, ILayer pz) ;
PolygonP v4pPolygonClone(PolygonP p) ;
PolygonP v4pPolygonConcrete(PolygonP p, ICollide i) ;
PolygonP v4pPolygonIntoList(PolygonP p, PolygonP* list) ;
Boolean v4pPolygonOutOfList(PolygonP p, PolygonP* list) ;
PolygonP v4pPolygonAddSub(PolygonP parent, PolygonP p) ;
PolygonP v4pPolygonAddNewSub(PolygonP parent, PolygonProps t, Color col, ILayer z) ;
Boolean  v4pPolygonDelSub(PolygonP parent, PolygonP p) ;
PolygonP v4pPolygonDelPoint(PolygonP p, PointP s) ;
PolygonProps v4pPolygonPutProp(PolygonP p, PolygonProps i) ;
PolygonProps v4pPolygonRemoveProp(PolygonP p, PolygonProps i) ;
PointP   v4pPolygonAddPoint(PolygonP p, Coord x, Coord y) ;
PointP   v4pPolygonMovePoint(PolygonP p, PointP s, Coord x, Coord y) ;
Color   v4pPolygonSetColor(PolygonP p, Color c) ;
PointP   v4pPolygonGetPoints(PolygonP p) ;
ILayer   v4pPolygonGetZ(PolygonP p) ;
Color   v4pPolygonGetColor(PolygonP p) ;

// v4pList
#define v4pListNew(LISTE) (LISTE)=NULL
PolygonP v4pListAddPolygon(PolygonP *list, PolygonProps t, Color col, ILayer z) ;
PolygonP v4pListAddClone(PolygonP *list, PolygonP p) ;
Boolean  v4pListDelPolygon(PolygonP *list, PolygonP p) ;

// transformation
PolygonP v4pPolygonTransformClone(PolygonP p, PolygonP c, Coord dx, Coord dy, int angle, ILayer dz) ;
PolygonP v4pPolygonTransform(PolygonP p, Coord dx, Coord dy, int angle, ILayer dz) ;

// helpers
void v4pViewToAbsolute(Coord x, Coord y, Coord *xa, Coord *ya) ;
void v4pAbsoluteToView(Coord x, Coord y, Coord *xa, Coord *ya) ;
PolygonP v4pPolygonRect(PolygonP p, Coord x0, Coord y0, Coord x1, Coord y1) ;
PolygonProps v4pPolygonEnable(PolygonP p) ;
PolygonProps v4pPolygonDisable(PolygonP p) ;

#endif
