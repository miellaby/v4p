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

/* Abbrevs
** display size = rectangle v4pDisplayHeight*v4pDisplayWidth
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

#define YHASH_SIZE 512
#define YHASH_MASK 511

// Polygon type
typedef struct polygon_s {
 PolygonProps props ; // property flags
 PointP point1 ; // points list
 Color color ; // color (any data needed by the drawing function)
 ILayer z ; // depth
 ICollide i ; // collide layer index (i may be != z)
 PolygonP sub1 ; //  subs list
 PolygonP next ; // subs list link
 Coord minx,maxx,miny,maxy; // minimal surrounding rectangle
 Coord minyv, maxyv ; // vertical boundaries in view coordinates
 List ActiveEdge1 ; // ActiveEdges list
} Polygon ;

// type ActiveEdge
typedef struct activeEdge_s {
 Coord x0, y0, x1, y1 ; // vector coordinates; absolute or relative depending on the belonging list
 PolygonP p ; // polygone
 Coord x, o1, o2, s, h, r1, r2 ; // bresenham: offsets, sums, lineNb, remainings
} ActiveEdge ;

// contexte V4P
typedef struct v4pContext_s {
 V4pDisplayP display;
 PolygonP *scene ; // scene = a polygon set
 Coord   xvu0,yvu0,xvu1,yvu1 ; // view corner coordinates
 Color   bgColor;
 int debug1 ;
 QuickHeap pointHeap, polygonHeap, activeEdgeHeap ;
 List    openedAEList ; // ActiveEdge lists
 QuickTable openableAETable; // ActiveEdge Hash Table
 Coord   dxvu,dyvu; // view width and height
 Coord divxvu,modxvu,divyvu,modyvu; // ratios screen / view in result+reminder pairs
 Coord divxvub,modxvub,divyvub,modyvub; // ratios view / screen in result+reminder pairs
 Boolean scaling; // is scaling necessary?
 UInt32 changes;
} V4pContext ;

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
#define V4P_CHANGED_VIEW 4

static V4pContextP v4p = NULL ; // current (selected) v4p Context
V4pContextP v4pDefaultContext = NULL;

// define the current BG color
Color v4pSetBGColor(Color bg) {
  return v4p->bgColor=bg;
}

// define the current view
Boolean v4pSetView(Coord x0, Coord y0, Coord x1, Coord y1) {
  int lineWidth = v4pDisplayWidth, lineNb =  v4pDisplayHeight;

  v4p->xvu0=x0;
  v4p->yvu0=y0;
  v4p->xvu1=x1;
  v4p->yvu1=y1;
  v4p->dxvu=x1-x0;
  v4p->dyvu=y1-y0;
  if (!v4p->dxvu || !v4p->dyvu) return failure; // can't divide by 0
  v4p->divxvu = lineWidth / v4p->dxvu;
  v4p->modxvu = lineWidth % v4p->dxvu;
  v4p->divxvub = v4p->dxvu / lineWidth;
  v4p->modxvub = v4p->dxvu % lineWidth;
  v4p->divyvu = lineNb / v4p->dyvu;
  v4p->modyvu = lineNb % v4p->dyvu;
  v4p->divyvub = v4p->dyvu / lineNb;
  v4p->modyvub = v4p->dyvu % lineNb;
  v4p->scaling = !(v4p->divxvu==1 && v4p->divyvu==1 && v4p->modxvu==0 && v4p->modyvu==0);
  v4p->changes |= V4P_CHANGED_VIEW;
  return success;
}

void v4pSetDisplay(V4pDisplayP d) {
  v4p->display = d;
  // call to refresh internal values depending on current display 
  v4pSetView(v4p->xvu0, v4p->yvu0, v4p->xvu1, v4p->yvu1);
}


// create a v4p context
V4pContextP v4pContextNew() {
  V4pContextP v4p = (V4pContextP)malloc(sizeof(V4pContext)) ;
  int lineWidth = v4pDisplayWidth, lineNb = v4pDisplayHeight;
  v4p->display = v4pDisplayContext;
  v4p->scene = NULL;
  v4p->pointHeap = QuickHeapNewFor(Point) ;
  v4p->polygonHeap = QuickHeapNewFor(Polygon) ;
  v4p->activeEdgeHeap = QuickHeapNewFor(ActiveEdge) ;
  v4p->openableAETable = QuickTableNew(YHASH_SIZE) ; // vertical sort
  v4p->xvu0=0;
  v4p->yvu0=0;
  v4p->xvu1=lineWidth;
  v4p->yvu1=lineNb;
  v4p->dxvu=lineWidth;
  v4p->dyvu=lineNb;
  v4p->divxvu = 1;
  v4p->modxvu = 0;
  v4p->divxvub = 1;
  v4p->modxvub = 0;
  v4p->divyvu = 1;
  v4p->modyvu = 0;
  v4p->divyvub = 1;
  v4p->modyvub = 0;
  v4p->scaling = 0;
  v4p->changes = 255; // All memoization caches to be reset
  return v4p;
}

// select a v4p context
void v4pContextSet(V4pContextP p) {
  v4p = p;
}

// delete a v4p context
void v4pContextFree(V4pContextP p) {
  QuickHeapDelete(v4p->pointHeap) ;
  QuickHeapDelete(v4p->polygonHeap) ;
  QuickHeapDelete(v4p->activeEdgeHeap) ;
  QuickTableDelete(v4p->openableAETable);
  free(p);
}

Boolean v4pInit() {
  if (!v4pDefaultContext) {
     v4pDefaultContext=v4pContextNew();
  }
  v4pContextSet(v4pDefaultContext);
  return success ;
}

void v4pQuit() {
  if (v4pDefaultContext) v4pContextFree(v4pDefaultContext);
  if (v4p == v4pDefaultContext) v4p = NULL;
  v4pDefaultContext = NULL;
}

// create a polygon
PolygonP v4pPolygonNew(PolygonProps t, Color col, ILayer z) {
  PolygonP p = QuickHeapAlloc(v4p->polygonHeap) ;
  p->props = t & ~V4P_CHANGED;
  p->z = z;
  p->i = (ICollide)-1;
  p->color = col;
  p->point1 = NULL;
  p->sub1 = NULL;
  p->next = NULL;
  p->miny = JUMPCOORD; // miny = too much => boundaries to be computed
  p->ActiveEdge1 = NULL ;
  return p;
}

#define v4pPolygonChanged(P) ((P)->props|=V4P_CHANGED)

PolygonP v4pPolygonDelActiveEdges(PolygonP p);

// delete a poly (including its points and subs)
int v4pPolygonDel(PolygonP p) {
 v4pPolygonDelActiveEdges(p) ;
 while (p->point1) v4pPolygonDelPoint(p,p->point1);
 while (p->sub1) v4pPolygonDelSub(p,p->sub1);
 QuickHeapFree(v4p->polygonHeap,p);
 return success ;
}


// Add a polygon to a list linked by the 'next' pointer
PolygonP v4pPolygonIntoList(PolygonP p, PolygonP* list) {
  p->next=*list;
  *list=p;
  return p ;
}

// remove a polygon from a list linked by the next pointer
Boolean v4pPolygonOutOfList(PolygonP p, PolygonP* list) {
  PolygonP ppl,pl;

  if (*list == p)
    *list = p->next;
  else {
    ppl = *list;
    pl = ppl->next;
    while (pl != p && pl) {
      ppl = pl;
      pl = pl->next;
    }
    if (!pl) return (v4pDisplayError("polygon lost"), failure) ;
    ppl->next = p->next;
  }
  p->next=NULL;
  return success ;
}


static v4pPolygonNotMoreInDisabled(PolygonP p) {
    v4pPolygonRemoveProp(p, V4P_IN_DISABLED) ;
    if (p->next)
      v4pPolygonNotMoreInDisabled(p->next) ;
    if (p->sub1 && !(p->props & V4P_DISABLED))
      v4pPolygonNotMoreInDisabled(p->sub1) ;
}

PolygonProps v4pPolygonEnable(PolygonP p) {
   if (!(p->props & V4P_DISABLED))
      return p->props ;

   if (p->sub1 && !(p->props & V4P_IN_DISABLED))
      v4pPolygonNotMoreInDisabled(p->sub1) ;

   return v4pPolygonRemoveProp(p, V4P_DISABLED) ;
}

static v4pPolygonInDisabled(PolygonP p) {
    v4pPolygonPutProp(p, V4P_IN_DISABLED) ;
    if (p->next)
      v4pPolygonInDisabled(p->next) ;
    if (p->sub1)
      v4pPolygonInDisabled(p->sub1) ;
}

PolygonProps v4pPolygonDisable(PolygonP p) {
   if (p->props & V4P_DISABLED)
      return p->props ;
   if (p->sub1)
      v4pPolygonInDisabled(p->sub1) ;
   return v4pPolygonPutProp(p, V4P_DISABLED) ;
}


// Add a polygon to an other polygon subs list
PolygonP v4pPolygonAddSub(PolygonP parent, PolygonP p) {
  if (parent->props & (V4P_DISABLED | V4P_IN_DISABLED))
      v4pPolygonInDisabled(p) ;
  return v4pPolygonIntoList(p, &parent->sub1);
}

// combo PolygonNew+PolygonIntoList
PolygonP v4pListAddPolygon(PolygonP *list, PolygonProps t, Color col, ILayer z) {
  return v4pPolygonIntoList(v4pPolygonNew(t, col, z), list);
}

// combo ListDelPolygon+PolygonOutOfList
Boolean v4pListDelPolygon(PolygonP *list, PolygonP p) {
  return v4pPolygonOutOfList(p, list) || v4pPolygonDel(p) ;
}

// combo PolygonAPolygon+PolygonNew
PolygonP v4pPolygonAddNewSub(PolygonP parent, PolygonProps t, Color col, ILayer z) {
   return v4pPolygonAddSub(parent, v4pPolygonNew(t, col, z)) ;
}

// remove a poly from an other poly subs list, then delete it
Boolean v4pPolygonDelSub(PolygonP parent, PolygonP p) {
   return v4pPolygonOutOfList(p, &parent->sub1) || v4pPolygonDel(p) ;
}

// set a polygon property flag
PolygonProps v4pPolygonPutProp(PolygonP p, PolygonProps i) {
  v4pPolygonChanged(p) ;
  return (p->props |= i) ;
}

// remove a polygon property flag
PolygonProps v4pPolygonRemoveProp(PolygonP p, PolygonProps i) {
  v4pPolygonChanged(p) ;
  return (p->props &= ~i) ;
}

// Add a polygon point
PointP v4pPolygonAddPoint(PolygonP p, Coord x, Coord y) {
   PointP s = QuickHeapAlloc(v4p->pointHeap) ;
   s->x = x;
   s->y = y;
   if (!p->point1) {
	  if (!(x == JUMPCOORD && y == JUMPCOORD)) {
        p->minx = p->maxx = x;
        p->miny = p->maxy = y;
      }
   } else {
      if (x < p->minx) p->minx = x ;
      else if (x > p->maxx) p->maxx = x ;
      if (y < p->miny) p->miny = y ;
      else if (y > p->maxy) p->maxy = y ;
   }
   s->next = p->point1;
   p->point1 = s;
   v4pPolygonChanged(p) ;
   return s ;
}


// Add a "jump" point into a polygon
PointP v4pPolygonAddJump(PolygonP p) {
   PointP s = QuickHeapAlloc(v4p->pointHeap) ;
   s->x = JUMPCOORD;
   s->y = JUMPCOORD;
   s->next = p->point1;
   p->point1 = s;
   v4pPolygonChanged(p) ;
   return s ;
}

// set polygon color
Color v4pPolygonSetColor(PolygonP p, Color c) {
   return p->color = c;
}

// returns a polygon points list
PointP v4pPolygonGetPoints(PolygonP p) {
   return p->point1 ;
}

// returns a polygon depth (layer index)
ILayer v4pPolygonGetZ(PolygonP p) {
   return p->z ;
}

// returns a polygon color
Color v4pPolygonGetColor(PolygonP p) {
   return p->color ;
}

// move a polygon point
PointP v4pPolygonMovePoint(PolygonP p, PointP s, Coord x, Coord y) {
   if (p->miny == JUMPCOORD || (x == JUMPCOORD && y == JUMPCOORD))
      {}
   else if (s->x == p->minx || s->y == p->miny
      || s->x == p->maxx || s->y == p->maxy) {
      p->miny = JUMPCOORD ; // boundaries to be computed again
   } else {
      if (x < p->minx) p->minx = x ;
      else if (x > p->maxx) p->maxx = x ;
      if (y < p->miny) p->miny = y ;
      else if (y > p->maxy) p->maxy = y ;
   }
   s->x = x;
   s->y = y;
   v4pPolygonChanged(p) ;
   return s ;
}

// remove a point from a polygon
PolygonP v4pPolygonDelPoint(PolygonP p, PointP s) {
   PointP pps, ps;

   if (p->point1 == s)
      p->point1 = s->next;
   else {
     pps = p->point1;
     ps = pps->next;
     while (ps != s && ps) {
        pps = ps;
        ps = ps->next;
     }
     if (!ps) return NULL ;
     pps->next = ps->next;
   }

   if (p->miny != JUMPCOORD
       && (s->x == p->minx || s->y == p->miny
           || s->x == p->maxx || s->y == p->maxy)) {
      p->miny = JUMPCOORD ; // boundaries to be computed again
   }

   QuickHeapFree(v4p->pointHeap, s) ;

   v4pPolygonChanged(p) ;
   return p ;
}

// transform hexa char ('0-9,A-F') to int
int v4pXToD(char c) {
   int o,r ;
   o = (int)c ;
   r = o - (int)'0' ;
   if (r >= 0 && r <= 9)
      return r ;
   else {
      r =o - (int)'A';
      if (r >= 0 && r <= 5)
         return 10 + r ;
      else {
         r = o - (int)'a' ;
         return (r >= 0 && r <= 5
                 ? 10 + r : 0) ;
} } }

// set the polygon colliding layer
PolygonP v4pPolygonConcrete(PolygonP p, ICollide i) {
   p->i = i ;
   return p ;
}

// create an ActiveEdge of a polygon
ActiveEdgeP v4pActiveEdgeNew(PolygonP p) {
   ActiveEdgeP b = QuickHeapAlloc(v4p->activeEdgeHeap);
   b->p = p;
   ListAddData(p->ActiveEdge1, b);
   return b ;
}

// delete all ActiveEdges of a poly
PolygonP v4pPolygonDelActiveEdges(PolygonP p) {
   List l ;
   ActiveEdgeP b;
   l = p->ActiveEdge1 ;
   while (l) {
     b = (ActiveEdgeP)ListData(l) ;
     QuickHeapFree(v4p->activeEdgeHeap, b);
     l = ListFree(l) ;
   }
   p->ActiveEdge1 = NULL;
   return p;
}

// called by v4pPolygonTransformClone()
PolygonP v4pRecPolygonTransformClone(Boolean estSub, PolygonP p, PolygonP c, Coord dx, Coord dy, ILayer dz) {
   PointP sp, sc;
   Coord x, y, x2, y2;
   c->miny = JUMPCOORD; // invalidate computed boundaries 
   c->z = c->z+dz;
   sp = p->point1;
   sc = c->point1;
   while (sp) {
      x = sp->x;
      y = sp->y;
      if (!(x == JUMPCOORD && y == JUMPCOORD)) {
        straighten(x, y, &x2, &y2);
        x2+= dx;
        y2+= dy;
        sc->x = x2;
        sc->y = y2;
      } else {
        sc->x = JUMPCOORD;
        sc->y = JUMPCOORD;
      }
      sp = sp->next;
      sc = sc->next;
   }
   v4pPolygonChanged(c);
   if (estSub && p->next)
      v4pRecPolygonTransformClone(true, p->next, c->next, dx, dy, dz);
   if (p->sub1)
      v4pRecPolygonTransformClone(true, p->sub1, c->sub1, dx, dy, dz);
   return c ;
}

// transform a clone c of a polygon p so that points(c) = transfo(points(p),delta-x/y, turn-angle)
PolygonP v4pPolygonTransformClone(PolygonP p, PolygonP c, Coord dx, Coord dy, int angle, ILayer dz) {
  computeCosSin(angle);
  /* a voir: ratiox et ratioy :
       cosa:=(cosa*ratiox) shr 7;
       sina:=(sina*ratioy) shr 7;
   */
  return v4pRecPolygonTransformClone(false, p, c, dx, dy, dz);
}

// transform a polygon
PolygonP v4pPolygonTransform(PolygonP p, Coord dx, Coord dy, int angle, ILayer dz) {
   return v4pPolygonTransformClone(p, p, dx, dy, angle, dz);
}

// called by v4pPolygonClone
PolygonP v4pRecPolygonClone(Boolean estSub, PolygonP p) {
   PointP s;
   PolygonP c = v4pPolygonNew(p->props, p->color, p->z);

   for (s = p->point1 ; s ; s = s->next)
      v4pPolygonAddPoint(c, s->x, s->y);

   if (estSub && p->next)
      c->next = v4pRecPolygonClone(true, p->next);
   if (p->sub1)
      c->sub1 = v4pRecPolygonClone(true, p->sub1);

   return c;
}

// clone a polygon (including its descendants)
PolygonP v4pPolygonClone(PolygonP p) {
   return v4pRecPolygonClone(false, p);
}

// combo PolygonClone+PolygonIntoList
PolygonP v4pListAddClone(PolygonP *list, PolygonP p) {
   return v4pPolygonIntoList(v4pPolygonClone(p), list);
}

// compute the minimal rectangle surrounding a polygon
PolygonP v4pPolygonComputeLimits(PolygonP p) {
   PointP s = p->point1;
   if (!s) { // no point
      p->miny = JUMPCOORD;
   } else {
      Coord minx = JUMPCOORD, maxx = JUMPCOORD, miny = JUMPCOORD, maxy = JUMPCOORD;
      while (s && s->x == JUMPCOORD && s->y == JUMPCOORD)
        s = s->next;
	  if (s) {
	    maxx = minx = s->x;
        maxy = miny = s->y;
        for (s = s->next; s; s = s->next) {
		  if (s->x == JUMPCOORD && s->y == JUMPCOORD) continue;
		  
          if (s->x < minx) minx = s->x;
          else if (s->x > maxx) maxx = s->x;
          if (s->y < miny) miny = s->y;
          else if (s->y > maxy) maxy = s->y;
        }
      }
      p->minx = minx;
      p->miny = miny;
      p->maxx = maxx;
      p->maxy = maxy;
   }
   return p;
}

// transform relative coordinates into absolute (scene related) ones
void v4pViewToAbsolute(Coord x, Coord y, Coord *xa, Coord *ya) {
   int lineWidth = v4pDisplayWidth, lineNb = v4pDisplayHeight;
   *xa = v4p->xvu0 + x * v4p->divxvub + (x * v4p->modxvub) / lineWidth + (x < 0 && v4p->modxvub ? -1 : 0) ;
   *ya = v4p->yvu0 + y * v4p->divyvub + (y * v4p->modyvub) / lineNb + (y < 0 && v4p->modyvub ? -1 : 0);
}

// transform absolute coordinates into relative (scene related) ones
void v4pAbsoluteToView(Coord x, Coord y, Coord *xa, Coord *ya) {
   x -= v4p->xvu0 ;
   y -= v4p->yvu0 ;
   if (v4p->scaling) {
     *xa = x * v4p->divxvu + (x * v4p->modxvu) / v4p->dxvu + (x < 0 && v4p->modxvu ? -1 : 0);
     *ya = y * v4p->divyvu + (y * v4p->modyvu) / v4p->dyvu + (y < 0 && v4p->modyvu ? -1 : 0);
    } else {
     *xa = x ;
     *ya = y ;
    }
}

// return false if polygon is located out of the view area
Boolean v4pIsVisible(PolygonP p) {
   if (!p->point1)
      return false ;
   if (p->miny == JUMPCOORD) // unknown limits
      v4pPolygonComputeLimits(p) ;

   { Coord minx = p->minx, maxx = p->maxx, miny = p->miny, maxy = p->maxy;

     if (!(p->props & relative)) {
       v4pAbsoluteToView(minx, miny, &minx, &miny) ;
       v4pAbsoluteToView(maxx, maxy, &maxx, &maxy) ;
     }
     p->minyv = miny ;
     p->maxyv = maxy ;
     return (maxx >= 0 && maxy >= 0 && minx < v4pDisplayWidth && miny < v4pDisplayHeight) ;
   }
}

// build a list of ActiveEdges for a given polygon
PolygonP v4pPolygonBuildActiveEdgeList(PolygonP p) {
   PointP sa, sb, s1;
   Boolean loop, end;
   int dx, dy, q, r;
   Coord sx0, sx1, sy0, sy1;
   ActiveEdgeP b ;
   int isVisible = 0;

   if (!(p->props & V4P_CHANGED)) {

      // This polygon has not changed. Let's try to be smart
      if (p->props & relative) { // This polygon is defined in view coordinates. No change.
         return p;
      } else if (!(v4p->changes & V4P_CHANGED_VIEW)) {
         // Polygon coordinates are absolute but the view window didn't change. No change.  
         return p;
      } else { // This absolute polygon hasn't changed but it might have moved within view referential.
         // we simply update its boundaries in view coordinates. 
         //Coord stub;
         //v4pAbsoluteToView(0, p->miny, &stub, &(p->minyv));
         //v4pAbsoluteToView(0, p->maxy, &stub, &(p->maxyv));
        isVisible = v4pIsVisible(p);
        if (isVisible && p->ActiveEdge1)
          // if AE lists are set, we return because they are up-to-date.
          return p; 
      }
   } else {
	 isVisible = v4pIsVisible(p);

     // Remember than at least one polygon is changed
     v4p->changes |= (p->props & relative) ? V4P_CHANGED_RELATIVE : V4P_CHANGED_ABSOLUTE;
     p->props &= ~V4P_CHANGED ; // remove the flag saying this polygon is changed.
   }


   // Need to recompile AE
   // ==================== 
   v4pPolygonDelActiveEdges(p) ;

   if ((p->props & (V4P_DISABLED | V4P_IN_DISABLED | invisible)))
      return p;

   if (!isVisible)
    // Not visible, we don't build AE lists
    return p;

   s1 = p->point1;
   
   int toBeClosed;
   
   
   while (s1) { // lacots
      toBeClosed = true;
      if (s1->x == JUMPCOORD && s1->y == JUMPCOORD) {
	    toBeClosed = false;
	    s1 = s1->next;
      }
      if (!s1) break;
      sa = s1;
      sb = s1->next;
      if (!sb) break;
      loop = false;
      end = false;
      while (!end) { // points
        if ((sa->x != JUMPCOORD || sa->y != JUMPCOORD)
            && (sb->x != JUMPCOORD || sb->y != JUMPCOORD)
            && sa->y != sb->y) { // active
          b = v4pActiveEdgeNew(p) ;

          if (sa->y < sb->y) {
             sx0 = sa->x;
             sy0 = sa->y;
             sx1 = sb->x;
             sy1 = sb->y;
          } else {
             sx0 = sb->x;
             sy0 = sb->y;
             sx1 = sa->x;
             sy1 = sa->y;
           }


           b->x0 = sx0;
           b->y0 = sy0;
           b->x1 = sx1;
           b->y1 = sy1;

           if (p->props & relative) {

             dx = sx1 - sx0;
             dy = sy1 - sy0;
             q = dx / dy ;
             r = dx > 0 ? dx % dy : (-dx) % dy ;
             b->o1 = q ;
             b->o2 = b->o1 + (dx > 0 ? 1 : -1) ;
             b->r1 = r ;
             b->r2 = r - dy ;

           } // relative polygon

         } // active

         if (loop) {
            end = true ;
         } else if (sb->x == s1->x && sb->y == s1->y) {
            // new lacot
            s1 = sb->next ;
            end = true;
         } else {
            sa = sb ;
            sb = sb->next ;
            if (!sb) {
               if (toBeClosed) {
			      sb = s1 ;
			      s1 = NULL;
                  loop = true;
               } else {
				  s1 = NULL;
                  end = true;
               }
            }
         }
      } //  points
   } // lacots


   return p ;
}

// called by v4pSortActiveEdge()
int compareActiveEdgeX(void *data1, void *data2) {
   ActiveEdgeP b1 = data1, b2 = data2 ;
   return (b1->x < b2->x) ;
}

// sort an ActiveEdge list ordered by 'x'
List v4pSortActiveEdge(List list) {
   ListSetDataPrior(compareActiveEdgeX) ;
   return ListSort(list) ;
}

// draw a scan-line slice depending on its polygon
Boolean v4pDrawSlice(int y, int x0, int x1, PolygonP p) {
  return v4pDisplaySlice(y, x0, x1, p ? p->color : v4p->bgColor) ;
}


// build AE lists
void v4pBuildOpenableAELists(PolygonP polygonChain) {
   PolygonP p ;
   List l ;
   ActiveEdgeP b;

   for (p = polygonChain ; p ; p = p->next) {
     int isRelative = p->props & relative;

     v4pPolygonBuildActiveEdgeList(p) ;

     l = p->ActiveEdge1;
     while (l) {
       b = (ActiveEdgeP)ListData(l) ;
       if (isRelative) {
		 QuickTableAdd(v4p->openableAETable, (b->y0 > 0 ? b->y0 : 0) & YHASH_MASK, b);
       } else if (b->y0 < v4p->yvu0) {
         QuickTableAdd(v4p->openableAETable, 0, b);		 
	   } else {
         Coord stub, yr0;
         v4pAbsoluteToView(0, b->y0, &stub, &yr0);
         QuickTableAdd(v4p->openableAETable, yr0 & YHASH_MASK, b);
       }
       l = ListNext(l);
     }

     if (p->sub1) v4pBuildOpenableAELists(p->sub1) ;
   }
}


// open all new scan-line intersected ActiveEdge
Boolean v4pOpenActiveEdge(Coord yl, Coord yu) {

   Boolean open = false ;
   List l;
   ActiveEdgeP b ;
   
   Coord xr0, yr0, xr1, yr1, dx, dy, q, r ;

   l = QuickTableGet(v4p->openableAETable, yl & YHASH_MASK);
   for (; l; l = ListNext(l)) {
      b = (ActiveEdgeP)ListData(l) ;

      if (!(b->p->props & relative)) {
	    if (!yl && b->y0 < v4p->yvu0) {
		   v4pAbsoluteToView(b->x0, b->y0, &xr0, &yr0) ;
        } else {
            v4pAbsoluteToView(b->x0, b->y0, &xr0, &yr0) ;
            if (yr0 != yl) continue;
        }
         
        v4pAbsoluteToView(b->x1, b->y1, &xr1, &yr1) ;
        if (yr1 <= yl) continue ;

        dx = xr1 - xr0;
        dy = yr1 - yr0;
        q = dx / dy ;
        r = dx > 0 ? dx % dy : (-dx) % dy ;
        b->o1 = q ;
        b->o2 = b->o1 + (dx > 0 ? 1 : -1) ;
        b->r1 = r ;
        b->r2 = r - dy ;
        b->s = -dy;
        if (yr0 < yl) { // top crop needed
          int dy2 = yl - yr0 ;
          if (yr0 > 0 && dy2 > v4p->divyvu) v4pDisplayError("issue crop = %d > ratio = %d; yl=%d yu=%d y0=%d", (int)dy2, (int)v4p->divyvub, (int)yl, (int)yu, (int)b->y0);
          xr0 += dy2 * q + dy2 * (dx > 0 ? r : -r) / dy ;
          b->s += (dy2 * r) % dy ;
        }
        b->h = yr1 - yl - 1;
        b->x = xr0 ;
      } else { // relative
        if (yl == 0 && b->y0 > 0 || b->y0 != yl) continue;
        if (b->y1 <= yl) continue ;
        b->s = b->r2 - b->r1 ;
        b->x = b->x0 ;
        b->h = b->y1 - yl - 1;
        if (b->y0 < yl) { // top crop needed
          int dy2 = yl - b->y0 ;
          b->x += dy2 * b->o1 + dy2 * (b->o2 >= 0 ? b->r1 : -b->r1) / dy ;
          b->s += (dy2 * b->r1) % dy ;
        }
      }      
      ListAddData(v4p->openedAEList, b) ;
      open = true ;
   }

   return open ;
}

// Render a scene
Boolean v4pRender() {
   List l, pl ;
   PolygonP p, polyVisible ;
   ActiveEdgeP b, pb ;
   Coord y, px, px_collide ;

   Coord yu;
   int su, ou1, ou2, ru1, ru2;

   ILayer z ;
   PolygonP layers[16] ;
   int zMax ;
   UInt16 bz, bi ; // bit-word of layers & collides
   UInt16 mz, mi ; // masques
   ICollide i, colli1, colli2 ;
   int nColli ;
   PolygonP pColli[16] ;
   Boolean sortNeeded ;

   v4pDisplaySetContext(v4p->display);

   v4pDisplayStart() ;

   // update AE lists and build an y-index hash table

   if (!v4p->scene) return failure;

   QuickTableReset(v4p->openableAETable);
   v4pBuildOpenableAELists(*(v4p->scene));
   
   // list of opened ActiveEdges
   v4p->openedAEList = NULL ;

   // yu (scanline y in absolute coordinates) progression during scanline loop
   ou1 = v4p->divyvub ;
   ou2 = v4p->divyvub + 1 ;
   yu = v4p->yvu0 - ou2;
   ru1 = v4p->modyvub ;
   ru2 = v4p->modyvub - v4pDisplayHeight ;
   su = v4p->modyvub;

   // scan-line loop
   for (y = 0 ; y < v4pDisplayHeight ; y++) {
      sortNeeded = false ;


      if (su >= 0) {
         su+= ru2 ;
         yu+= ou2 ;
      } else {
         su+= ru1 ;
         yu+= ou1 ;
      }


      // loop among opened ActiveEdge
      l = v4p->openedAEList ;
      pl = NULL ;
      px = -(0x7FFF) ; // not sure its really the min, but we dont care
      while (l) {
         b = (ActiveEdgeP)ListData(l);
         if (b->h <= 0) { // close ActiveEdge
            if (pl)
               ListSetNext(pl, l = ListFree(l)) ;
            else
               v4p->openedAEList = l = ListFree(l) ;
         } else { // shift ActiveEdge
            int x ;
            b->h--;
            if (b->o2) {
              if (b->s > 0) {
                x = b->x+= b->o2 ;
                b->s+= b->r2 ;
              } else {
                x = b->x+= b->o1 ;
                b->s+= b->r1 ;
              }
            } else
              x = b->x ;

            if (x < px) sortNeeded = true ;
            px = x ;
            pl = l ;
            l = ListNext(l) ;
         }
      } // opened ActiveEdge loop

      // open newly intersected ActiveEdge

      sortNeeded |= v4pOpenActiveEdge(y, yu) ;
      // sort ActiveEdge
      if (sortNeeded)
         v4p->openedAEList = v4pSortActiveEdge(v4p->openedAEList) ;

      // reset layers
      bz = 0 ;
      polyVisible = NULL ;
      zMax = -1 ;

      // reset collides
      bi = 0 ;
      nColli = 0 ;

      // loop among scanline slices
      px = px_collide = 0 ;
      for (l = v4p->openedAEList ; l ; l = ListNext(l)) { // loop ActiveEdge ouvert / x
          // pb = b ;
          b = (ActiveEdgeP)ListData(l) ;
          p = b->p ;
          z = p->z & 15 ;
          bz ^= ((UInt16)1 << z) ;
          //if (b->x > 1000) v4pDisplayDebug("problem %d %d %d", (int)b->x, (int)px, (pb ? pb->x : -1));
          //if (px > b->x) v4pDisplayError("pb slice %d %d %d", (int)y, (int)px, (int)b->x);
          if ((int)z >= zMax) {
            if (px < v4pDisplayWidth && b->x > 0)
               v4pDrawSlice(y, imax(px, 0), imin(b->x, v4pDisplayWidth), polyVisible);
            px = b->x ;
            if ((int)z > zMax) {
               polyVisible = layers[z] = p ;
               zMax = z ;
            } else { // z == zMax
               zMax = floorLog2(bz) ;
               polyVisible = (zMax >= 0 ? layers[zMax] : NULL) ;
            }
          } else { // z < zMax
            layers[z] = p ;
          }
          if (nColli > 1)
             v4pDisplayCollide(colli1, colli2, y, px_collide, b->x, pColli[colli1], pColli[colli2]);
          px_collide = b->x;
          i = p->i ;
          mi = (i == (ICollide)-1 ? (UInt16)0 : (UInt16)1 << (i & 15)) ;
          if (layers[z]) {
             if (mi) {
                pColli[i] = p ;
                if (!(bi & mi)) {
                   bi |= mi ;
                   nColli++ ;
                   if (nColli == 1)
                      colli1 = i ;
                   else if (nColli == 2)
                      colli2 = i ;
                }
             }
          } else {
             if (bi & mi) {
                bi ^= mi ;
                nColli-- ;
                if (nColli == 1 && i == colli1)
                   colli1 = colli2 ;
                else if (nColli == 2)
                   if (i == colli1)
                      colli1 = floorLog2(bi ^ (1 << colli2)) ;
                   else if (i == colli2)
                      colli2 = floorLog2(bi ^ (1 << colli1)) ;
             }
          }

       } // x opened ActiveEdge loop

       // last slice
       if (px < v4pDisplayWidth)
          v4pDrawSlice(y, imax(0, px), v4pDisplayWidth, polyVisible) ;

   } // y loop ;

   l = v4p->openedAEList ;
   while (l) l = ListFree(l) ;
   v4p->openedAEList = NULL ;

   if (yu != v4p->yvu1 - v4p->divyvub)
      v4pDisplayError("problem %d != %d", (int)yu, (int)v4p->yvu1 - v4p->divyvub);

   v4p->changes = 0 ;
   v4pDisplayEnd() ;
   return success ;
}
// Add 4 points as a rectangle
PolygonP v4pPolygonRect(PolygonP p, Coord x0, Coord y0, Coord x1, Coord y1) {
   v4pPolygonAddPoint(p, x0, y0) ;
   v4pPolygonAddPoint(p, x0, y1) ;
   v4pPolygonAddPoint(p, x1, y1) ;
   v4pPolygonAddPoint(p, x1, y0) ;
   return p ;
}

// Set a list of polygons as a scene
void v4pSetScene(PolygonP *scene) {
   v4p->scene = scene ;
}
