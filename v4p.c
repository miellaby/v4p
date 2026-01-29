/* v4p = Vectors rendition engine For Pocket

   This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Concepts:
** Display: lineNb*lineWidth sized straight rectangle
** View: rectangular part of scene to render
** Scene: a list of polygons
** Polygon: path of n Points (idealy closed), Color and Depth
** Depth = z: Layer number (the highest layer masks the others)
** Color: any data needed by the drawing function
** Point: x,y coordinates in scene referential
** Radius: polygon may have a radius. Turn its points into disks. A 1-point polygon with a radius is a regular Disk.
** Sub: attached sub-polygon will be transformed within its parent
** Clone: Clone transformation is always derived from its Source rather than its current geometry
** ActiveEdge: non horizontal edge inside view
** Bouding Box: smallest straight rectangle containing polygon
** ActiveEdge : non horizontal edge inside view
** sub : sub-polygon related to a parent polygon (rotation,moving,optimisation)
** z = depth = layer #
** XHeap : reserved memory for X
** XP : X pointer
** IX : X table indice
** v4pDisplayX : lower layer function call
** opened polygon : polygon intersected by the scan-line : min(y(points)) < y < max(y(points))
** to be opened polygon : y(scan-line) < min(y(points))
** closed polygon : y(scan-line) > max(y(points))
** absolute = in scene-referential (position on screen depends on view)
** relative = in screen-referential (0,0 ==> screen corner)
*/
#include <stdlib.h>
#include "v4p.h"
#include "lowmath.h"
#include "quickheap.h"
#include "sortable.h"
#include "quicktable.h"
#include "v4pi.h"
#include <stdio.h>

#define YHASH_SIZE 512
#define YHASH_MASK 511

// Polygon type
typedef struct polygon_s {
  PolygonProps props;                   // property flags
  PointP       point1;                  // list of points (only 1 for disk)
  Color        color;                   // color (any data needed by the drawing function)
  Coord        radius;                  // point radius (disk = 1 point with positive radius)
  ILayer       z;                       // depth
  ICollide     i;                       // collide layer index (i may be != z)
  PolygonP     sub1;                    //  subs list
  PolygonP     next;                    // subs list link
  PolygonP     parent;                  // parent polygon reference (for clones)
  Coord        anchor_x, anchor_y;      // rotation anchor point (default: 0,0)
  Coord        minx, maxx, miny, maxy;  // bounding box
  Coord        minyv, maxyv;            // vertical boundaries in view coordinates
  List         ActiveEdge1;             // ActiveEdges list
} Polygon;

// type ActiveEdge
typedef struct activeEdge_s {
  PolygonP p;                        // parent polygon
  Coord    x0, y0, x1, y1;           // vector coordinates; absolute or relative depending on the belonging list
  Coord    x0v, y0v, x1v, y1v;       // vector coordinates in view
  // bresenham
  Coord    x;  // current x
  Coord    o1; // offset when accumulator under limit
  Coord    o2; // offset when accumulator cross the limit
  Coord    s;  // accumulator
  Coord    h;  // heigh of the edge (scanline count)
  Coord    r1; // remaining 1
  Coord    r2; // r1 - dy
  Boolean  circle;  // rigth or left edge of a circle (coordinates are its bounding box)
} ActiveEdge;

// contexte V4P
typedef struct v4pContext_s {
  V4pDisplayP display;
  V4pSceneP   scene;                   // scene = a polygon set
  Coord       xvu0, yvu0, xvu1, yvu1;  // view corner coordinates
  Polygon     dummyBgPoly;             // just for color
  int         debug1;
  QuickHeap   pointHeap, polygonHeap, activeEdgeHeap;
  List        openedAEList;                        // ActiveEdge lists
  QuickTable  openableAETable;                     // ActiveEdge Hash Table
  Coord       dxvu, dyvu;                          // view width and height
  Coord       divxvu, modxvu, divyvu, modyvu;      // ratios screen / view in result+reminder pairs
  Coord       divxvub, modxvub, divyvub, modyvub;  // ratios view / screen in result+reminder pairs
  Boolean     scaling;                             // is scaling necessary?
  UInt32      changes;
} V4pContext;

/*
 * About screen vs view ratios:
 * divyvu == 0 when view bigger than screen (zoom out)
 * divyvub == 0 when screen bigger than view (zoom in)
 * when zoom in : yline++ ==> y-absolute may be inchanged
 * so when divyvu == 0, y-absolute comparison is avoided
 * when zoom out : yline++ ==>  y-absolute+=x where x>0
 */
#define V4P_CHANGED_ABSOLUTE 1
#define V4P_CHANGED_RELATIVE 2
#define V4P_CHANGED_VIEW     4

static V4pContextP v4p               = NULL;  // current (selected) v4p Context
V4pSceneP          v4pDefaultScene   = NULL;
V4pContextP        v4pDefaultContext = NULL;

// change the v4p current context
void               v4pSetContext(V4pContextP p) {
  v4p = p;
}

// set the BG color
Color v4pSetBGColor(Color bg) {
  return v4pPolygonSetColor(&(v4p->dummyBgPoly), bg);
}

// set the view
Boolean v4pSetView(Coord x0, Coord y0, Coord x1, Coord y1) {
  int lineWidth = v4pDisplayWidth, lineNb = v4pDisplayHeight;

  v4p->xvu0 = x0;
  v4p->yvu0 = y0;
  v4p->xvu1 = x1;
  v4p->yvu1 = y1;
  v4p->dxvu = x1 - x0;
  v4p->dyvu = y1 - y0;
  if (!v4p->dxvu || !v4p->dyvu)
    return failure;  // can't divide by 0
  v4p->divxvu  = lineWidth / v4p->dxvu;
  v4p->modxvu  = lineWidth % v4p->dxvu;
  v4p->divxvub = v4p->dxvu / lineWidth;
  v4p->modxvub = v4p->dxvu % lineWidth;
  v4p->divyvu  = lineNb / v4p->dyvu;
  v4p->modyvu  = lineNb % v4p->dyvu;
  v4p->divyvub = v4p->dyvu / lineNb;
  v4p->modyvub = v4p->dyvu % lineNb;
  v4p->scaling = !(v4p->divxvu == 1 && v4p->divyvu == 1 && v4p->modxvu == 0 && v4p->modyvu == 0);
  v4p->changes |= V4P_CHANGED_VIEW;
  return success;
}

// set the display
void v4pSetDisplay(V4pDisplayP d) {
  v4p->display = d;
  // call to refresh internal values depending on current display
  v4pSetView(v4p->xvu0, v4p->yvu0, v4p->xvu1, v4p->yvu1);
}

// Set the scene
void v4pSetScene(V4pSceneP scene) {
  v4p->scene = scene;
}

// Get the scene
V4pSceneP v4pGetScene() {
  return v4p->scene;
}

// create a v4p context
V4pContextP v4pContextNew() {
  V4pContextP v4p       = (V4pContextP)malloc(sizeof(V4pContext));
  int         lineWidth = v4pDisplayWidth, lineNb = v4pDisplayHeight;
  v4p->display           = v4pDisplayContext;
  v4p->scene             = v4pDefaultScene;
  v4p->pointHeap         = QuickHeapNewFor(Point);
  v4p->polygonHeap       = QuickHeapNewFor(Polygon);
  v4p->activeEdgeHeap    = QuickHeapNewFor(ActiveEdge);
  v4p->openableAETable   = QuickTableNew(YHASH_SIZE);  // vertical sort
  v4p->dummyBgPoly.color = 0;
  v4p->xvu0              = 0;
  v4p->yvu0              = 0;
  v4p->xvu1              = lineWidth;
  v4p->yvu1              = lineNb;
  v4p->dxvu              = lineWidth;
  v4p->dyvu              = lineNb;
  v4p->divxvu            = 1;
  v4p->modxvu            = 0;
  v4p->divxvub           = 1;
  v4p->modxvub           = 0;
  v4p->divyvu            = 1;
  v4p->modyvu            = 0;
  v4p->divyvub           = 1;
  v4p->modyvub           = 0;
  v4p->scaling           = 0;
  v4p->changes           = 255;  // All memoization caches to be reset
  return v4p;
}

// delete a v4p context
void v4pContextFree(V4pContextP p) {
  QuickHeapDelete(v4p->pointHeap);
  QuickHeapDelete(v4p->polygonHeap);
  QuickHeapDelete(v4p->activeEdgeHeap);
  QuickTableDelete(v4p->openableAETable);
  free(p);
}

// Create a new scene
V4pSceneP v4pSceneNew() {
  V4pSceneP s = (V4pSceneP)malloc(sizeof(V4pScene));
  s->label    = "";
  s->polygons = NULL;
  return s;
}

void v4pSceneFree(V4pSceneP s) {
  free(s);
}

// v4p init
Boolean v4pInit() {
  if (!v4pDefaultScene) {
    v4pDefaultScene = v4pSceneNew();
  }
  if (!v4pDefaultContext) {
    v4pDefaultContext = v4pContextNew();
  }
  v4pSetContext(v4pDefaultContext);
  v4pSetView(0, 0, v4pDisplayWidth, v4pDisplayHeight);
}

// v4p quit
void v4pQuit() {
  if (v4pDefaultContext)
    v4pContextFree(v4pDefaultContext);
  if (v4p == v4pDefaultContext)
    v4p = NULL;
  v4pDefaultContext = NULL;
}

// create a polygon
PolygonP v4pPolygonNew(PolygonProps t, Color col, ILayer z) {
  PolygonP p     = QuickHeapAlloc(v4p->polygonHeap);
  p->props       = t & ~V4P_CHANGED;
  p->z           = z;
  p->i           = (ICollide)-1;
  p->color       = col;
  p->radius      = 0;
  p->point1      = NULL;
  p->sub1        = NULL;
  p->next        = NULL;
  p->parent      = NULL;  // No parent by default
  p->anchor_x    = 0;     // Default anchor at origin
  p->anchor_y    = 0;
  p->miny        = JUMPCOORD;  // miny = too much => boundaries to be computed
  p->ActiveEdge1 = NULL;
  return p;
}

// combo PolygonNew+SceneAdd
PolygonP v4pSceneAddNew(V4pSceneP s, PolygonProps t, Color col, ILayer z) {
  PolygonP p = v4pPolygonNew(t, col, z);
  v4pSceneAdd(s, p);
  return p;
}
PolygonP v4pAddNew(PolygonProps t, Color col, ILayer z) {
  return v4pSceneAddNew(v4p->scene, t, col, z);
}

// create a disk
PolygonP v4pDiskNew(PolygonProps t, Color col, ILayer z, Coord center_x, Coord center_y, Coord radius) {
  PolygonP p = v4pPolygonNew(t, col, z);
  p->radius = radius;
  v4pPolygonAddPoint(p, center_x, center_y);
  return p;
}

// combo DiskNew+SceneAdd
PolygonP v4pSceneAddNewDisk(V4pSceneP s, PolygonProps t, Color col, ILayer z, Coord center_x, Coord center_y, Coord radius) {
  PolygonP p = v4pDiskNew(t, col, z, center_x, center_y, radius);
  v4pSceneAdd(s, p);
  return p;
}

PolygonP v4pAddNewDisk(PolygonProps t, Color col, ILayer z, Coord center_x, Coord center_y, Coord radius) {
  return v4pSceneAddNewDisk(v4p->scene, t, col, z, center_x, center_y, radius);
}

#define v4pPolygonChanged(P) ((P)->props |= V4P_CHANGED)

PolygonP v4pPolygonDelActiveEdges(PolygonP p);

// delete a poly (including its points and subs)
int      v4pPolygonDel(PolygonP p) {
  v4pPolygonDelActiveEdges(p);
  while (p->point1)
    v4pPolygonDelPoint(p, p->point1);
  while (p->sub1)
    v4pPolygonDelSub(p, p->sub1);
  QuickHeapFree(v4p->polygonHeap, p);
  return success;
}

// Add a polygon to a list linked by the 'next' pointer
PolygonP v4pPolygonIntoList(PolygonP p, PolygonP *list) {
  p->next = *list;
  *list   = p;
  return p;
}

// remove a polygon from a list linked by the next pointer
Boolean v4pPolygonOutOfList(PolygonP p, PolygonP *list) {
  PolygonP ppl, pl;

  if (*list == p)
    *list = p->next;
  else {
    ppl = *list;
    pl  = ppl->next;
    while (pl != p && pl) {
      ppl = pl;
      pl  = pl->next;
    }
    if (!pl)
      return (v4pDisplayError("polygon lost"), failure);
    ppl->next = p->next;
  }
  p->next = NULL;
  return success;
}

static void v4pPolygonNotMoreInDisabled(PolygonP p) {
  v4pPolygonRemoveProp(p, V4P_IN_DISABLED);
  if (p->next)
    v4pPolygonNotMoreInDisabled(p->next);
  if (p->sub1 && !(p->props & V4P_DISABLED))
    v4pPolygonNotMoreInDisabled(p->sub1);
}

PolygonProps v4pPolygonEnable(PolygonP p) {
  if (!(p->props & V4P_DISABLED))
    return p->props;

  if (p->sub1 && !(p->props & V4P_IN_DISABLED))
    v4pPolygonNotMoreInDisabled(p->sub1);

  return v4pPolygonRemoveProp(p, V4P_DISABLED);
}

static void v4pPolygonInDisabled(PolygonP p) {
  v4pPolygonPutProp(p, V4P_IN_DISABLED);
  if (p->next)
    v4pPolygonInDisabled(p->next);
  if (p->sub1)
    v4pPolygonInDisabled(p->sub1);
}

PolygonProps v4pPolygonDisable(PolygonP p) {
  if (p->props & V4P_DISABLED)
    return p->props;
  if (p->sub1)
    v4pPolygonInDisabled(p->sub1);
  return v4pPolygonPutProp(p, V4P_DISABLED);
}

// Add a polygon to an other polygon subs list
PolygonP v4pPolygonAddSub(PolygonP parent, PolygonP p) {
  if (parent->props & (V4P_DISABLED | V4P_IN_DISABLED))
    v4pPolygonInDisabled(p);
  return v4pPolygonIntoList(p, &parent->sub1);
}

// Add a polygon into the scene
V4pSceneP v4pSceneAdd(V4pSceneP s, PolygonP p) {
  v4pPolygonIntoList(p, &(s->polygons));
  return v4p->scene;
}

PolygonP v4pAdd(PolygonP p) {
  return v4pSceneAdd(v4p->scene, p), p;
}

// remove a polygon from the scene
V4pSceneP v4pSceneRemove(V4pSceneP s, PolygonP p) {
  v4pPolygonOutOfList(p, &(s->polygons));
  return s;
}

PolygonP v4pRemove(PolygonP p) {
  return v4pSceneRemove(v4p->scene, p), p;
}

// combo PolygonDel+SceneRemove
Boolean v4pSceneDel(V4pSceneP s, PolygonP p) {
  return v4pSceneRemove(s, p) && v4pPolygonDel(p);
}

Boolean v4pDel(PolygonP p) {
  return v4pSceneDel(v4p->scene, p);
}

// combo PolygonAddSub+PolygonNew
PolygonP v4pPolygonAddNewSub(PolygonP parent, PolygonProps t, Color col, ILayer z) {
  return v4pPolygonAddSub(parent, v4pPolygonNew(t, col, z));
}

// remove a poly from an other poly subs list, then delete it
Boolean v4pPolygonDelSub(PolygonP parent, PolygonP p) {
  return v4pPolygonOutOfList(p, &parent->sub1) || v4pPolygonDel(p);
}

// set a polygon property flag
PolygonProps v4pPolygonPutProp(PolygonP p, PolygonProps i) {
  v4pPolygonChanged(p);
  return (p->props |= i);
}

// remove a polygon property flag
PolygonProps v4pPolygonRemoveProp(PolygonP p, PolygonProps i) {
  v4pPolygonChanged(p);
  return (p->props &= ~i);
}

// Add a polygon point
PointP v4pPolygonAddPoint(PolygonP p, Coord x, Coord y) {
  PointP s = QuickHeapAlloc(v4p->pointHeap);
  s->x     = x;
  s->y     = y;
  Coord r  = p->radius;
  if ((x & y) != JUMPCOORD) {
    if (p->miny == JUMPCOORD) {
      p->minx = x - r;
      p->maxx = x + r;
      p->miny = y - r;
      p->maxy = y + r;
    } else {
      if (x - r < p->minx)
        p->minx = x - r;
      if (x + r > p->maxx)
        p->maxx = x + r;
      if (y - r < p->miny)
        p->miny = y - r;
      if (y + r > p->maxy)
        p->maxy = y + r;
    }
  }
  s->next   = p->point1;
  p->point1 = s;
  v4pPolygonChanged(p);
  return s;
}

// Add a "jump" point into a polygon
PointP v4pPolygonAddJump(PolygonP p) {
  PointP s  = QuickHeapAlloc(v4p->pointHeap);
  s->x      = JUMPCOORD;
  s->y      = JUMPCOORD;
  s->next   = p->point1;
  p->point1 = s;
  v4pPolygonChanged(p);
  return s;
}

// set polygon radius (disk)
Coord v4pPolygonSetRadius(PolygonP p, Coord radius) {
  Coord delta = p->radius - radius;
  if (p->miny != JUMPCOORD) {
    p->minx -= delta;
    p->maxx += delta;
    p->maxy -= delta;
    p->maxy += delta;
  }
  p->radius = radius;
  v4pPolygonChanged(p);
  return radius;
}

// set polygon color
Color v4pPolygonSetColor(PolygonP p, Color c) {
  return p->color = c;
}

// returns a polygon points list
PointP v4pPolygonGetPoints(PolygonP p) {
  return p->point1;
}

// returns a polygon depth (layer index)
ILayer v4pPolygonGetZ(PolygonP p) {
  return p->z;
}

// returns a polygon color
Color v4pPolygonGetColor(PolygonP p) {
  return p->color;
}

// move a polygon point
PointP v4pPolygonMovePoint(PolygonP p, PointP s, Coord x, Coord y) {
  Coord r = p->radius;
  if (p->miny == JUMPCOORD || ((x & y) == JUMPCOORD)) {
  } else if (s->x - r == p->minx || s->y - r == p->miny || s->x + r == p->maxx || s->y + r == p->maxy) {
    p->miny = JUMPCOORD;  // boundaries to be computed again
  } else {
    if (x - r < p->minx)
      p->minx = x - r;
    if (x + r > p->maxx)
      p->maxx = x + r;
    if (y - r < p->miny)
      p->miny = y - r;
    if (y + r > p->maxy)
      p->maxy = y + r;
  }
  s->x = x;
  s->y = y;
  v4pPolygonChanged(p);
  return s;
}

// remove a point from a polygon
PolygonP v4pPolygonDelPoint(PolygonP p, PointP s) {
  PointP pps, ps;

  if (p->point1 == s)
    p->point1 = s->next;
  else {
    pps = p->point1;
    ps  = pps->next;
    while (ps != s && ps) {
      pps = ps;
      ps  = ps->next;
    }
    if (!ps)
      return NULL;
    pps->next = ps->next;
  }

  Coord r = p->radius;
  if (p->miny != JUMPCOORD && (s->x - r == p->minx || s->y - r == p->miny || s->x + r == p->maxx || s->y + r == p->maxy)) {
    p->miny = JUMPCOORD;  // boundaries to be computed again
  }

  QuickHeapFree(v4p->pointHeap, s);

  v4pPolygonChanged(p);
  return p;
}

// transform hexa char ('0-9,A-F') to int
int v4pXToD(char c) {
  int o, r;
  o = (int)c;
  r = o - (int)'0';
  if (r >= 0 && r <= 9)
    return r;
  else {
    r = o - (int)'A';
    if (r >= 0 && r <= 5)
      return 10 + r;
    else {
      r = o - (int)'a';
      return (r >= 0 && r <= 5
                ? 10 + r
                : 0);
    }
  }
}

// set the polygon colliding layer
PolygonP v4pPolygonConcrete(PolygonP p, ICollide i) {
  p->i = i;
  return p;
}

// create an ActiveEdge of a polygon
ActiveEdgeP v4pAddNewActiveEdge(PolygonP p, PointP a, PointP b) {
  ActiveEdgeP ae = QuickHeapAlloc(v4p->activeEdgeHeap);
  ae->p = p;
  ae->circle     = false;
  ListAddData(p->ActiveEdge1, ae);
  int sx0, sy0, sx1, sy1;
  if (a->y < b->y) {
    sx0 = a->x;
    sy0 = a->y;
    sx1 = b->x;
    sy1 = b->y;
  } else {
    sx0 = b->x;
    sy0 = b->y;
    sx1 = a->x;
    sy1 = a->y;
  }

  ae->x0 = sx0;
  ae->y0 = sy0;
  ae->x1 = sx1;
  ae->y1 = sy1;

  if (p->props & relative) { // relative polygon
    ae->x0v = sx0;
    ae->y0v = sy0;
    ae->x1v = sx1;
    ae->y1v = sy1;
  }

  return ae;
}

// create a circle edge for a polygon point
ActiveEdgeP v4pAddNewCircleEdges(PolygonP p, PointP c) {
  Coord r = p->radius,
        x = c->x, y = c->y;
  
  v4pDisplayDebug("CIRCLE: Creating circle edges for point (%d, %d), radius=%d\n", x, y, r);
  
  Point top = { x: x, y: y - r}, bottom_left = { x: x - r, y: y + r }, bottom_right = { x: x + r, y: y + r };
  
  v4pDisplayDebug("CIRCLE: Circle bounding box - top:(%d,%d), left:(%d,%d), right:(%d,%d)\n",
                  top.x, top.y, bottom_left.x, bottom_left.y, bottom_right.x, bottom_right.y);
  
  ActiveEdgeP left   = v4pAddNewActiveEdge(p, &top, &bottom_left);
  left->circle      = true;
  v4pDisplayDebug("CIRCLE: Created left circle edge %p\n", (void*)left);
  
  ActiveEdgeP right  = v4pAddNewActiveEdge(p, &top, &bottom_right);
  right->circle      = true;
  v4pDisplayDebug("CIRCLE: Created right circle edge %p\n", (void*)right);
}

// delete all ActiveEdges of a poly
PolygonP v4pPolygonDelActiveEdges(PolygonP p) {
  List        l;
  ActiveEdgeP b;
  l = p->ActiveEdge1;
  while (l) {
    b = (ActiveEdgeP)ListData(l);
    QuickHeapFree(v4p->activeEdgeHeap, b);
    l = ListFree(l);
  }
  p->ActiveEdge1 = NULL;
  return p;
}

// called by v4pPolygonTransformClone()
PolygonP v4pRecPolygonTransformClone(Boolean estSub, PolygonP p, PolygonP c, Coord dx, Coord dy, int angle, ILayer dz, Coord anchor_x, Coord anchor_y, Coord zoom_x, Coord zoom_y) {
  PointP sp, sc;
  Coord  x, y, x2, y2, tx, ty;

  c->z += dz;            // Shift z
  if (c->radius && zoom_x == zoom_y)  // Scale radius (only if zoom_x == zoom_y)
    c->radius = (p->radius * zoom_x) >> 8;

  c->miny = JUMPCOORD;  // invalidate computed boundaries

  sp      = p->point1;
  sc      = c->point1;
  
  computeCosSin(angle);
  
  while (sp) {
    x = sp->x;
    y = sp->y;
    if ((x & y) != JUMPCOORD) {
      // Translate point relative to anchor
      tx = x - anchor_x;
      ty = y - anchor_y;
      
      // Apply rotation
      straighten(tx, ty, &x2, &y2);
      
      // Apply zoom/scaling
      x2 = (x2 * zoom_x) >> 8;
      y2 = (y2 * zoom_y) >> 8;
      
      // Translate back and apply position delta
      sc->x = x2 + anchor_x + dx;
      sc->y = y2 + anchor_y + dy;
    } else {
      sc->x = JUMPCOORD;
      sc->y = JUMPCOORD;
    }
    sp = sp->next;
    sc = sc->next;
  }
  v4pPolygonChanged(c);
  if (estSub && p->next)
    v4pRecPolygonTransformClone(true, p->next, c->next, dx, dy, angle, dz, anchor_x, anchor_y, zoom_x, zoom_y);
  if (p->sub1)
    v4pRecPolygonTransformClone(true, p->sub1, c->sub1, dx, dy, angle, dz, anchor_x, anchor_y, zoom_x, zoom_y);
  return c;
}

// transform a clone c of a polygon p so that points(c) = transfo(points(p),delta-x/y, turn-angle)
PolygonP v4pPolygonTransformClone(PolygonP p, PolygonP c, Coord dx, Coord dy, int angle, ILayer dz, Coord zoom_x, Coord zoom_y) {
  /* a voir: ratiox et ratioy :
       cosa:=(cosa*ratiox) shr 7;
       sina:=(sina*ratioy) shr 7;
   */
  // Use the clone's anchor point for the entire transformation tree
  Coord anchor_x = c->anchor_x;
  Coord anchor_y = c->anchor_y;
  return v4pRecPolygonTransformClone(false, p, c, dx, dy, angle, dz, anchor_x, anchor_y, zoom_x, zoom_y);
}

// transform a polygon
PolygonP v4pPolygonTransform(PolygonP p, Coord dx, Coord dy, int angle, ILayer dz, Coord zoom_x, Coord zoom_y) {
  if (p->parent) { // If this polygon has a parent
    // transform relatively to parent
    return v4pPolygonTransformClone(p->parent, p, dx, dy, angle, dz, zoom_x, zoom_y);
  } else {
    // Otherwise, transform in place
    return v4pPolygonTransformClone(p, p, dx, dy, angle, dz, zoom_x, zoom_y);
  }
}

// called by v4pPolygonClone
PolygonP v4pRecPolygonClone(Boolean estSub, PolygonP p) {
  PointP   s;
  PolygonP c = v4pPolygonNew(p->props, p->color, p->z);
  c->radius = p->radius;
  for (s = p->point1; s; s = s->next)
    v4pPolygonAddPoint(c, s->x, s->y);

  // Set parent reference for clones (but not for sub-polygons)
  if (!estSub) {
    c->parent = p;
    // Also copy the anchor point from the parent
    c->anchor_x = p->anchor_x;
    c->anchor_y = p->anchor_y;
  }

  if (estSub && p->next)
    c->next = v4pRecPolygonClone(true, p->next);
  if (p->sub1)
    c->sub1 = v4pRecPolygonClone(true, p->sub1);

  return c;
}

// clone a polygon (including its descendants) with parent reference
PolygonP v4pPolygonClone(PolygonP p) {
  return v4pRecPolygonClone(false, p);
}

// combo PolygonClone+SceneAdd
PolygonP v4pSceneAddClone(V4pSceneP s, PolygonP p) {
  PolygonP c = v4pPolygonClone(p);
  v4pSceneAdd(s, c);
  return c;
}

PolygonP v4pAddClone(PolygonP p) {
  return v4pSceneAddClone(v4p->scene, p);
}

// Forward declaration for anchor functions
PolygonP v4pPolygonComputeLimits(PolygonP p);

// set polygon anchor point to its center
PolygonP v4pPolygonSetAnchorToCenter(PolygonP p) {
  if (p->miny == JUMPCOORD) {
    v4pPolygonComputeLimits(p);
  }
  p->anchor_x = (p->minx + p->maxx) / 2;
  p->anchor_y = (p->miny + p->maxy) / 2;
  return p;
}

// set polygon anchor point manually
PolygonP v4pPolygonSetAnchor(PolygonP p, Coord x, Coord y) {
  p->anchor_x = x;
  p->anchor_y = y;
  return p;
}

// compute the minimal rectangle surrounding a polygon
PolygonP v4pPolygonComputeLimits(PolygonP p) {
  Coord  minx = JUMPCOORD, maxx = JUMPCOORD, miny = JUMPCOORD, maxy = JUMPCOORD;
  PointP s = p->point1;
  while (s && (s->x & s->y) == JUMPCOORD) {
    s = s->next;
  }
  if (s) {  // at least one point
    Coord r = p->radius;
    minx = s->x - r;
    maxx = s->x + r;
    miny = s->y - r;
    maxy = s->y + r;
    for (s = s->next; s; s = s->next) {
      Coord x = s->x, y = s->y;
      if ((x & y) == JUMPCOORD)
        continue;

      if (x - r < minx)
        minx = x - r;
      if (x + r > maxx)
        maxx = x + r;
      if (y - r < miny)
        miny = y - r;
      if (y + r > maxy)
        maxy = y + r;
    }
  }
  p->minx = minx;
  p->miny = miny;
  p->maxx = maxx;
  p->maxy = maxy;
  return p;
}

// transform relative coordinates into absolute (scene related) ones
void v4pViewToAbsolute(Coord x, Coord y, Coord *xa, Coord *ya) {
  int lineWidth = v4pDisplayWidth, lineNb = v4pDisplayHeight;
  *xa = v4p->xvu0 + x * v4p->divxvub + (x * v4p->modxvub) / lineWidth + (x < 0 && v4p->modxvub ? -1 : 0);
  *ya = v4p->yvu0 + y * v4p->divyvub + (y * v4p->modyvub) / lineNb + (y < 0 && v4p->modyvub ? -1 : 0);
}

// transform absolute coordinates into relative (scene related) ones
void v4pAbsoluteToView(Coord x, Coord y, Coord *xa, Coord *ya) {
  x -= v4p->xvu0;
  y -= v4p->yvu0;
  if (v4p->scaling) {
    *xa = x * v4p->divxvu + (x * v4p->modxvu) / v4p->dxvu + (x < 0 && v4p->modxvu ? -1 : 0);
    *ya = y * v4p->divyvu + (y * v4p->modyvu) / v4p->dyvu + (y < 0 && v4p->modyvu ? -1 : 0);
  } else {
    *xa = x;
    *ya = y;
  }
}

// return false if polygon is located out of the view area
Boolean v4pIsVisible(PolygonP p) {
  if (!p->point1)
    return false;
  if (p->miny == JUMPCOORD)  // unknown limits
    v4pPolygonComputeLimits(p);

  Coord minx = p->minx, maxx = p->maxx, miny = p->miny, maxy = p->maxy;

  if (!(p->props & relative)) {
    v4pAbsoluteToView(minx, miny, &minx, &miny);
    v4pAbsoluteToView(maxx, maxy, &maxx, &maxy);
  }
  p->minyv = miny;
  p->maxyv = maxy;
  return (maxx >= 0 && maxy >= 0 && minx < v4pDisplayWidth && miny < v4pDisplayHeight);
}

// build a list of ActiveEdges for a given polygon
PolygonP v4pPolygonBuildActiveEdgeList(PolygonP p) {
  Boolean isVisible = false;
  Boolean isCircle = p->radius > 0;

  if (!(p->props & V4P_CHANGED)) {
    // This polygon has not changed. Let's try to be smart
    if (p->props & relative) {  // This polygon is defined in view coordinates. No change.
      return p;
    } else if (!(v4p->changes & V4P_CHANGED_VIEW)) {
      // Polygon coordinates are absolute but the view window didn't change. No change.
      return p;
    } else {  // This absolute polygon hasn't changed but it might have moved within view referential.
              // we simply update its boundaries in view coordinates.
              // Coord stub;
      // v4pAbsoluteToView(0, p->miny, &stub, &(p->minyv));
      // v4pAbsoluteToView(0, p->maxy, &stub, &(p->maxyv));
      isVisible = v4pIsVisible(p);
      if (isVisible && p->ActiveEdge1)
        // if AE lists are set, we return because they are up-to-date.
        return p;
    }
  } else {
    isVisible = v4pIsVisible(p);

    // Remember than at least one polygon is changed
    v4p->changes |= (p->props & relative) ? V4P_CHANGED_RELATIVE : V4P_CHANGED_ABSOLUTE;
    p->props &= ~V4P_CHANGED;  // remove the flag saying this polygon is changed.
  }

  // Need to recompile AE
  // ====================
  v4pPolygonDelActiveEdges(p);

  if ((p->props & (V4P_DISABLED | V4P_IN_DISABLED | invisible)))
    return p;

  if (!isVisible) {
    // don't build AE lists of hidden polygons
    return p;
  }

  PointP s1 = p->point1;
  v4pDisplayDebug("POLYGON: Building active edges for polygon %p, isCircle=%d, radius=%d\n", (void*)p, isCircle, p->radius);
  
  while (s1) {  // path subset
    if ((s1->x & s1->y) == JUMPCOORD) {
      s1 = s1->next;
      continue;
    }
    PointP sa = s1, sb = sa->next;
    Boolean closed = false;
    v4pDisplayDebug("POLYGON: Processing point (%d, %d), next=%p\n", sa->x, sa->y, (void*)sb);
    
    // sub-path loop
    while (sb && (sb->x & sb->y) != JUMPCOORD) { // while in sub-path
      v4pDisplayDebug("POLYGON: Processing edge from (%d, %d) to (%d, %d)\n", sa->x, sa->y, sb->x, sb->y);
      
      if (sa->y != sb->y) {  // add an active edge
        v4pAddNewActiveEdge(p, sa, sb);
      }
      if (sa != s1 && sb->x == s1->x && sb->y == s1->y) { // the path is closed
        break;
      }

      if (isCircle) { // add circle edges around vertice
        v4pDisplayDebug("POLYGON: Adding circle edges for vertex (%d, %d)\n", sa->x, sa->y);
        v4pAddNewCircleEdges(p, sa);
      }

      // next edge
      sa = sb;
      sb = sb->next;
    }
    if (!sb) { // no more vertice
      v4pDisplayDebug("POLYGON: End of path subset, last point (%d, %d)\n", sa->x, sa->y);
      
      if (isCircle) {
        // add circle edge around last vertice (or only vertice for single-point polygons)
        v4pDisplayDebug("POLYGON: Adding circle edges for final vertex (%d, %d)\n", sa->x, sa->y);
        v4pAddNewCircleEdges(p, sa);
      }

      if (sa->y != s1->y) {  // add a closing edge
        v4pDisplayDebug("POLYGON: Adding closing edge from (%d, %d) to (%d, %d)\n", sa->x, sa->y, s1->x, s1->y);
        v4pAddNewActiveEdge(p, sa, s1);
      }
      break;
    }
    s1 = sb->next;
  }  // path subset
  
  v4pDisplayDebug("POLYGON: Finished building active edges for polygon %p\n", (void*)p);

  return p;
}

// called by v4pSortActiveEdge()
int compareActiveEdgeX(void *data1, void *data2) {
  ActiveEdgeP b1 = data1, b2 = data2;
  return (b1->x < b2->x);
}

// sort an ActiveEdge list ordered by 'x'
List v4pSortActiveEdge(List list) {
  ListSetDataPrior(compareActiveEdgeX);
  return ListSort(list);
}

// build AE lists
void v4pBuildOpenableAELists(PolygonP polygonChain) {
  PolygonP    p;
  List        l;
  ActiveEdgeP b;

  for (p = polygonChain; p; p = p->next) {
    int isRelative = p->props & relative;

    v4pPolygonBuildActiveEdgeList(p);

    l = p->ActiveEdge1;
    while (l) {
      b = (ActiveEdgeP)ListData(l);
      if (isRelative) {
        QuickTableAdd(v4p->openableAETable, (b->y0 > 0 ? b->y0 : 0) & YHASH_MASK, l);
      } else {
        v4pAbsoluteToView(b->x0, b->y0, &(b->x0v), &(b->y0v));
        v4pAbsoluteToView(b->x1, b->y1, &(b->x1v), &(b->y1v));
        if (b->y0 < v4p->yvu0) {
          QuickTableAdd(v4p->openableAETable, 0, l);
        } else {
          QuickTableAdd(v4p->openableAETable, b->y0v & YHASH_MASK, l);
        }
      }
      l = ListNext(l);
    }

    if (p->sub1)
      v4pBuildOpenableAELists(p->sub1);
  }
}

// open all new scan-line intersected ActiveEdge, returns them as a list
List v4pOpenActiveEdge(Coord yl, Coord yu) {
  List        newlyOpenedAEList = NULL;
  List        l;
  ActiveEdgeP b;

  Coord       xr0, yr0, xr1, yr1, dx, dy, q, r;

  v4pDisplayDebug("EDGE_OPEN: Opening active edges for scanline y=%d\n", yl);

  l = QuickTableGet(v4p->openableAETable, yl & YHASH_MASK);
  for (; l; l = l->quick) {
    b = (ActiveEdgeP)ListData(l);
    xr0 = b->x0v;
    yr0 = b->y0v;
    xr1 = b->x1v;
    yr1 = b->y1v;
    
    v4pDisplayDebug("EDGE_OPEN: Processing edge %p: (%d,%d) to (%d,%d), circle=%d\n", 
                   (void*)b, xr0, yr0, xr1, yr1, b->circle);
    
    if (yl == 0) {
      if (yr0 > 0)
        continue;
    } else if (yr0 != yl)
      continue;
    if (yr1 <= yl)
      continue;
    
    b->h = yr1 - yl - 1;
    b->x = xr0;
    dx = xr1 - xr0;
    dy = yr1 - yr0;
    
    v4pDisplayDebug("EDGE_OPEN: Opening edge %p, height=%d, dx=%d, dy=%d\n", 
                   (void*)b, b->h, dx, dy);
    
    if (!b->circle) {
      q     = dx / dy;
      r     = dx > 0 ? dx % dy : (-dx) % dy;
      b->o1 = q;
      b->o2 = b->o1 + (dx > 0 ? 1 : -1);
      b->r1 = r;
      b->r2 = r - dy;
      b->s  = -dy;
      int dy2 = yl - yr0;
      if (dy2 > 0) {  // edge top truncation
        b->x += dy2 * q + dy2 * (dx > 0 ? r : -r) / dy;
        b->s += (dy2 * r) % dy;
      }
    }
    ListAddData(newlyOpenedAEList, b);
  }
  if (newlyOpenedAEList)
    newlyOpenedAEList = v4pSortActiveEdge(newlyOpenedAEList);
  return newlyOpenedAEList;
}

// Render a scene
Boolean v4pRender() {
  List        l, pl;
  PolygonP    p, polyVisible;
  ActiveEdgeP b;
  Coord       y, px, px_collide;

  Coord       yu;
  int         su, ou1, ou2, ru1, ru2;

  ILayer      z;
  PolygonP    layers[16];
  int         zMax;
  UInt16      bz, bi;  // bit-word of layers & collides
  UInt16      mi;      // masques
  ICollide    i, colli1, colli2;
  int         nColli;
  PolygonP    pColli[16];
  Boolean     sortNeeded;

  v4pDisplaySetContext(v4p->display);

  v4pDisplayStart();

  // update AE lists and build an y-index hash table

  QuickTableReset(v4p->openableAETable);
  v4pBuildOpenableAELists(v4p->scene->polygons);

  // list of opened ActiveEdges
  v4p->openedAEList = NULL;

  // yu (scanline y in absolute coordinates) progression during scanline loop
  ou1               = v4p->divyvub;
  ou2               = v4p->divyvub + 1;
  yu                = v4p->yvu0 - ou2;
  ru1               = v4p->modyvub;
  ru2               = v4p->modyvub - v4pDisplayHeight;
  su                = v4p->modyvub;

  // scan-line loop
  for (y = 0; y < v4pDisplayHeight; y++) {
    sortNeeded = false;

    if (su >= 0) {
      su += ru2;
      yu += ou2;
    } else {
      su += ru1;
      yu += ou1;
    }

    // loop among opened ActiveEdge
    l  = v4p->openedAEList;
    pl = NULL;
    px = -(0x7FFF);  // not sure its really the min, but we dont care
    while (l) {
      b = (ActiveEdgeP)ListData(l);
      if (b->h <= 0) {  // close ActiveEdge
        v4pDisplayDebug("EDGE_CLOSE: Closing edge %p at y=%d\n", (void*)b, y);
        if (pl)
          ListSetNext(pl, l = ListFree(l));
        else
          v4p->openedAEList = l = ListFree(l);
      } else {  // shift ActiveEdge
        int x;
        b->h--;
        if (b->circle) {
          Coord xr0 = b->x0v;
          Coord yr0   = b->y0v;
          Coord xr1   = b->x1v;
          Coord yr1   = b->y1v;
          Coord r = (yr1 - yr0) / 2;

          UInt32 sign  = -(xr1 <= xr0) | (xr1 > xr0);  // sign -1 (left) or 1 (right)
          Coord dy   = (2 * y - yr1 - yr0) / 2; // dy: -r->0->r
          UInt32 dx    = isqrt(r * r - dy * dy);
          
          x = xr0 + sign * dx;
          b->x = x;
          v4pDisplayDebug("CIRCLE_SHIFT: Shift circle edge %p (%d,%d)x(%d,%d) to x=%d, y=%d\n",
                          (void *)b, xr0, yr0, xr1, yr1, x, y);

        } else {
          v4pDisplayDebug("CIRCLE_SHIFT: Shift edge %p (%d,%d)x(%d,%d) to x=%d, y=%d\n",
                          (void *)b, b->x0v, b->y0v, b->x1v, b->y1v, x, y);
          if (b->o2) {
            if (b->s > 0) {
              x = b->x += b->o2;
              b->s += b->r2;
            } else {
              x = b->x += b->o1;
              b->s += b->r1;
            }
          } else {
            x = b->x;
          }
        }

        sortNeeded |= (x < px);
        px = x;
        pl = l;
        l  = ListNext(l);
      }
    }  // opened ActiveEdge loop

    // sort ActiveEdge
    if (sortNeeded)
      v4p->openedAEList = v4pSortActiveEdge(v4p->openedAEList);

    // open newly intersected ActiveEdge
    List newlyOpenedAEList = v4pOpenActiveEdge(y, yu);
    if (newlyOpenedAEList) {
      ListSetDataPrior(compareActiveEdgeX);
      v4p->openedAEList = (v4p->openedAEList
                             ? ListMerge(v4p->openedAEList, newlyOpenedAEList)
                             : newlyOpenedAEList);
    }

    // reset layers
    bz          = 0;
    polyVisible = &(v4p->dummyBgPoly);
    zMax        = -1;

    // reset collides
    bi          = 0;
    nColli      = 0;

    // loop among scanline slices
    px = px_collide = 0;
    for (l = v4p->openedAEList; l; l = ListNext(l)) {  // loop ActiveEdge ouvert / x
      // pb = b ;
      b = (ActiveEdgeP)ListData(l);
      p = b->p;
      z = p->z & 15;
      bz ^= ((UInt16)1 << z);
      // if (b->x > 1000) v4pDisplayDebug("problem %d %d %d", (int)b->x, (int)px, (pb ? pb->x : -1));
      // if (px > b->x) v4pDisplayError("pb slice %d %d %d", (int)y, (int)px, (int)b->x);
      if ((int)z >= zMax) {
        if (px < v4pDisplayWidth && b->x > 0)
          v4pDisplaySlice(y, imax(px, 0), imin(b->x, v4pDisplayWidth), polyVisible->color);
        px = b->x;
        if ((int)z > zMax) {
          polyVisible = layers[z] = p;
          zMax                    = z;
        } else {  // z == zMax
          zMax        = floorLog2(bz);
          polyVisible = (zMax >= 0 ? layers[zMax] : &(v4p->dummyBgPoly));
        }
      } else {  // z < zMax
        layers[z] = p;
      }
      if (nColli > 1)
        v4pDisplayCollide(colli1, colli2, y, px_collide, b->x, pColli[colli1], pColli[colli2]);
      px_collide = b->x;
      i          = p->i;
      mi         = (i == (ICollide)-1 ? (UInt16)0 : (UInt16)1 << (i & 15));
      if (layers[z]) {
        if (mi) {
          pColli[i] = p;
          if (!(bi & mi)) {
            bi |= mi;
            nColli++;
            if (nColli == 1)
              colli1 = i;
            else if (nColli == 2)
              colli2 = i;
          }
        }
      } else {
        if (bi & mi) {
          bi ^= mi;
          nColli--;
          if (nColli == 1 && i == colli1)
            colli1 = colli2;
          else if (nColli == 2) {
            if (i == colli1) {
              colli1 = floorLog2(bi ^ (1 << colli2));
            } else if (i == colli2) {
              colli2 = floorLog2(bi ^ (1 << colli1));
            }
          }
        }
      }

    }  // x opened ActiveEdge loop

    // last slice
    if (px < v4pDisplayWidth)
      v4pDisplaySlice(y, imax(0, px), v4pDisplayWidth, polyVisible->color);

  }  // y loop ;

  l = v4p->openedAEList;
  while (l)
    l = ListFree(l);
  v4p->openedAEList = NULL;

  if (yu != v4p->yvu1 - v4p->divyvub)
    v4pDisplayError("problem %d != %d", (int)yu, (int)v4p->yvu1 - v4p->divyvub);

  v4p->changes = 0;
  v4pDisplayEnd();
  return success;
}
// Add 4 points as a rectangle
PolygonP v4pPolygonRect(PolygonP p, Coord x0, Coord y0, Coord x1, Coord y1) {
  v4pPolygonAddPoint(p, x0, y0);
  v4pPolygonAddPoint(p, x0, y1);
  v4pPolygonAddPoint(p, x1, y1);
  v4pPolygonAddPoint(p, x1, y0);
  return p;
}
