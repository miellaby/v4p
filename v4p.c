/**
 * v4p = Vectors rendition engine For Pocket
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Concepts:
 * Display: lineNb*lineWidth sized straight rectangle
 * View: rectangular part of scene to render
 * Scene: a list of polygons
 * Polygon: path of n Points (ideally closed), V4pColor and Depth
 * Depth = z: Layer number (the highest layer masks the others)
 * V4pColor: any data needed by the drawing function
 * Point: x,y coordinates in scene referential
 * Radius: polygon may have a radius. Turn its points into disks. A 1-point
 * polygon with a radius is a regular Disk. Sub: attached sub-polygon will be
 * transformed within its parent Clone: Clone transformation is always derived
 * from its Source rather than its current geometry ActiveEdge: non horizontal
 * edge inside view Bounding Box: smallest straight rectangle containing polygon
 * ActiveEdge: non horizontal edge inside view
 * Sub: sub-polygon related to a parent polygon (rotation, moving, optimization)
 * z = depth = layer #
 * XHeap: reserved memory for X
 * XP: X pointer
 * IX: X table index
 * v4pDisplayX: lower layer function call
 * Opened polygon: polygon intersected by the scan-line: min(y(points)) < y <
 * max(y(points)) To be opened polygon: y(scan-line) < min(y(points)) Closed
 * polygon: y(scan-line) > max(y(points)) Absolute: in scene-referential
 * (position on screen depends on view) Relative: in screen-referential (0,0 ==>
 * screen corner)
 */
#define _V4P_C
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "v4p.h"
#include "_v4p.h"

static V4pContextP v4p = NULL;  // current (selected) v4p Context
V4pSceneP v4p_defaultScene = NULL;
V4pContextP v4p_defaultContext = NULL;

// Change the v4p current context
void v4p_setContext(V4pContextP p) {
    v4p = p;
}

// Set the BG color
V4pColor v4p_setBGColor(V4pColor bg) {
    return v4p_setColor(&(v4p->dummyBgPoly), bg);
}

// Set the view
Boolean v4p_setView(V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1) {
    int dxdi = v4p_displayWidth;
    int dydi = v4p_displayHeight;
    int dxvu = x1 - x0;
    int dyvu = y1 - y0;
    if (! dxvu || ! dyvu) {
        return failure;  // Can't divide by 0
    }
    v4p->xvu0 = x0;
    v4p->yvu0 = y0;
    v4p->xvu1 = x1;
    v4p->yvu1 = y1;
    v4p->dxvu = dxvu;
    v4p->dyvu = dyvu;
    v4p->divxvu = dxdi / dxvu;
    v4p->modxvu = dxdi % dxvu;
    v4p->divxvub = dxvu / dxdi;
    v4p->modxvub = dxvu % dxdi;
    v4p->divyvu = dydi / dyvu;
    v4p->modyvu = dydi % dyvu;
    v4p->divyvub = dyvu / dydi;
    v4p->modyvub = dyvu % dydi;
    v4p->scaling = ! (v4p->divxvu == 1 && v4p->divyvu == 1 && v4p->modxvu == 0 && v4p->modyvu == 0);
    v4p->changes |= V4P_CHANGED_VIEW;
    return success;
}

// Set the display
void v4pi_set(V4piContextP d) {
    v4p->display = d;
    // Call to refresh internal values depending on current display
    v4p_setView(v4p->xvu0, v4p->yvu0, v4p->xvu1, v4p->yvu1);
}

// Set the scene
void v4p_setScene(V4pSceneP scene) {
    v4p->scene = scene;
}

// Get the scene
V4pSceneP v4p_getScene() {
    return v4p->scene;
}

// Create a v4p context
V4pContextP v4p_newContext() {
    V4pContextP v4p = (V4pContextP) malloc(sizeof(V4pContext));
    int lineWidth = v4p_displayWidth, lineNb = v4p_displayHeight;

    v4p->display = v4pi_context;
    v4p->scene = v4p_defaultScene;
    v4p->pointHeap = QuickHeapNewFor(V4pPoint);
    v4p->polygonHeap = QuickHeapNewFor(Polygon);
    v4p->activeEdgeHeap = QuickHeapNewFor(ActiveEdge);
    v4p->openableAETable = QuickTableNew(YHASH_SIZE);  // Vertical sort
    v4p->dummyBgPoly.color = 0;
    v4p->xvu0 = 0;
    v4p->yvu0 = 0;
    v4p->xvu1 = lineWidth;
    v4p->yvu1 = lineNb;
    v4p->dxvu = lineWidth;
    v4p->dyvu = lineNb;
    v4p->divxvu = 1;
    v4p->modxvu = 0;
    v4p->divxvub = 1;
    v4p->modxvub = 0;
    v4p->divyvu = 1;
    v4p->modyvu = 0;
    v4p->divyvub = 1;
    v4p->modyvub = 0;
    v4p->scaling = 0;
    v4p->changes = 255;  // All memoization caches to be reset
    v4p->nextId = 0;  // to number polygons uniquely

    return v4p;
}

// Delete a v4p context
void v4p_destroyContext(V4pContextP p) {
    QuickHeapDestroy(v4p->pointHeap);
    QuickHeapDestroy(v4p->polygonHeap);
    QuickHeapDestroy(v4p->activeEdgeHeap);
    QuickTableDelete(v4p->openableAETable);
    free(p);
}

// Create a new scene
V4pSceneP v4p_newScene() {
    V4pSceneP s = (V4pSceneP) malloc(sizeof(V4pScene));
    s->label = "";
    s->polygons = NULL;
    return s;
}

void v4p_destroyScene(V4pSceneP s) {
    free(s);
}

// V4P initialization with parameters
Boolean v4p_init2(int quality, Boolean fullscreen) {
    if (v4pi_init(quality, fullscreen)) {
        return failure;
    }

    if (! v4p_defaultScene) {
        v4p_defaultScene = v4p_newScene();
    }
    if (! v4p_defaultContext) {
        v4p_defaultContext = v4p_newContext();
    }
    v4p_setContext(v4p_defaultContext);
    v4p_setView(0, 0, v4p_displayWidth, v4p_displayHeight);
    return success;
}

// V4P initialization (default parameters)
Boolean v4p_init() {
    return v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
}

// V4P cleanup
void v4p_quit() {
    // NO: not in charge of freeing the user-created scene and context
    // if (v4p->scene != v4p_defaultScene) {
    //     v4p_destroyScene(v4p->scene);
    //     v4p->scene = NULL;
    // }
    // if (v4p != v4p_defaultContext) {
    //     v4p_destroyContext(v4p);
    //     v4p = NULL;
    // }
    v4p_destroyContext(v4p_defaultContext);
    v4p_destroyScene(v4p_defaultScene);
    v4pi_destroy();
}

// Create a polygon
V4pPolygonP v4p_new(V4pProps t, V4pColor col, V4pLayer z) {
    V4pPolygonP p = QuickHeapAlloc(v4p->polygonHeap);
    p->props = t & ~V4P_CHANGED;
    p->z = z;
    p->collisionMask = 0;
    p->color = col;
    p->radius = 0;
    p->point1 = NULL;
    p->sub1 = NULL;
    p->next = NULL;
    p->parent = NULL;  // No parent by default
    p->anchor_x = 0;  // Default anchor at origin
    p->anchor_y = 0;
    p->miny = V4P_NIL;  // miny = too much => boundaries to be computed
    p->ActiveEdge1 = NULL;
    p->id = v4p->nextId++;
    return p;
}

// Combo PolygonNew+SceneAdd
V4pPolygonP v4p_sceneAddNewPoly(V4pSceneP s, V4pProps t, V4pColor col, V4pLayer z) {
    V4pPolygonP p = v4p_new(t, col, z);
    v4p_sceneAdd(s, p);
    return p;
}

V4pPolygonP v4p_addNew(V4pProps t, V4pColor col, V4pLayer z) {
    return v4p_sceneAddNewPoly(v4p->scene, t, col, z);
}

// Create a disk
V4pPolygonP v4p_newDisk(V4pProps t, V4pColor col, V4pLayer z, V4pCoord center_x, V4pCoord center_y, V4pCoord radius) {
    V4pPolygonP p = v4p_new(t, col, z);
    p->radius = radius;
    v4p_addPoint(p, center_x, center_y);
    return p;
}

// Combo DiskNew+SceneAdd
V4pPolygonP v4p_sceneAddNewDisk(V4pSceneP s, V4pProps t, V4pColor col, V4pLayer z, V4pCoord center_x, V4pCoord center_y,
                                V4pCoord radius) {
    V4pPolygonP p = v4p_newDisk(t, col, z, center_x, center_y, radius);
    v4p_sceneAdd(s, p);
    return p;
}

V4pPolygonP v4p_addNewDisk(V4pProps t, V4pColor col, V4pLayer z, V4pCoord center_x, V4pCoord center_y,
                           V4pCoord radius) {
    return v4p_sceneAddNewDisk(v4p->scene, t, col, z, center_x, center_y, radius);
}

#define v4p_changed(P) ((P)->props |= V4P_CHANGED)

V4pPolygonP v4p_destroyActiveEdges(V4pPolygonP p);

// Delete a poly (including its points and subs)
int v4p_destroy(V4pPolygonP p) {
    v4p_destroyActiveEdges(p);
    while (p->point1) {
        v4p_destroyPointFrom(p, p->point1);
    }
    while (p->sub1) {
        v4p_destroyFromParent(p, p->sub1);
    }
    QuickHeapFree(v4p->polygonHeap, p);
    return success;
}

// Add a polygon to a list linked by the 'next' pointer
V4pPolygonP v4p_intoList(V4pPolygonP p, V4pPolygonP* list) {
    assert(*list != p);
    p->next = *list;
    *list = p;
    return p;
}

// Remove a polygon from a list linked by the next pointer
Boolean v4p_outOfList(V4pPolygonP p, V4pPolygonP* list) {
    V4pPolygonP ppl, pl;

    if (*list == p) {
        *list = p->next;
    } else {
        ppl = *list;
        pl = ppl->next;
        while (pl != p && pl) {
            ppl = pl;
            pl = pl->next;
        }
        if (! pl) {
            return (v4p_error("polygon lost"), failure);
        }
        ppl->next = p->next;
    }
    p->next = NULL;
    return success;
}

static void v4p_notMoreInDisabled(V4pPolygonP p) {
    v4p_removeProp(p, V4P_IN_DISABLED);
    if (p->next) v4p_notMoreInDisabled(p->next);
    if (p->sub1 && ! (p->props & V4P_DISABLED)) v4p_notMoreInDisabled(p->sub1);
}

V4pProps v4p_enable(V4pPolygonP p) {
    if (! (p->props & V4P_DISABLED)) return p->props;

    if (p->sub1 && ! (p->props & V4P_IN_DISABLED)) v4p_notMoreInDisabled(p->sub1);

    return v4p_removeProp(p, V4P_DISABLED);
}

static void v4p_inDisabled(V4pPolygonP p) {
    v4p_putProp(p, V4P_IN_DISABLED);
    v4p_changed(p);
    if (p->next) v4p_inDisabled(p->next);
    if (p->sub1) v4p_inDisabled(p->sub1);
}

V4pProps v4p_disable(V4pPolygonP p) {
    if (p->props & V4P_DISABLED) return p->props;
    v4p_changed(p);
    if (p->sub1) v4p_inDisabled(p->sub1);
    return v4p_putProp(p, V4P_DISABLED);
}

// Add a polygon to an other polygon subs list
V4pPolygonP v4p_addSub(V4pPolygonP parent, V4pPolygonP p) {
    if (parent->props & (V4P_DISABLED | V4P_IN_DISABLED)) v4p_inDisabled(p);
    return v4p_intoList(p, &parent->sub1);
}

// Add a polygon into the scene
V4pSceneP v4p_sceneAdd(V4pSceneP s, V4pPolygonP p) {
    v4p_intoList(p, &(s->polygons));
    return v4p->scene;
}

V4pPolygonP v4p_add(V4pPolygonP p) {
    return v4p_sceneAdd(v4p->scene, p), p;
}

// Remove a polygon from the scene
V4pSceneP v4p_sceneRemove(V4pSceneP s, V4pPolygonP p) {
    v4p_outOfList(p, &(s->polygons));
    return s;
}

V4pPolygonP v4p_remove(V4pPolygonP p) {
    return v4p_sceneRemove(v4p->scene, p), p;
}

// Clear all polygons from the current scene
void v4p_clearScene() {
    V4pPolygonP current = v4p->scene->polygons;
    while (current != NULL) {
        V4pPolygonP next = current->next;
        v4p_destroyFromScene(current);
        current = next;
    }
}

// combo remove+destroy from scence
Boolean v4p_destroyFromScene(V4pPolygonP p) {
    return v4p_sceneRemove(v4p->scene, p) && v4p_destroy(p);
}

// combo PolygonAddSub+PolygonNew
V4pPolygonP v4p_addNewSub(V4pPolygonP parent, V4pProps t, V4pColor col, V4pLayer z) {
    return v4p_addSub(parent, v4p_new(t, col, z));
}

// remove a poly from an other poly subs list, then delete it
Boolean v4p_destroyFromParent(V4pPolygonP parent, V4pPolygonP p) {
    return v4p_outOfList(p, &parent->sub1) || v4p_destroy(p);
}

// Get the first sub-polygon of a parent
V4pPolygonP v4p_getFirstSub(V4pPolygonP parent) {
    if (parent) {
        return parent->sub1;
    }
    return NULL;
}

// Get the next sub-polygon (sibling)
V4pPolygonP v4p_getNextSub(V4pPolygonP current) {
    if (current) {
        return current->next;
    }
    return NULL;
}

// Set a polygon property flag
V4pProps v4p_putProp(V4pPolygonP p, V4pProps i) {
    v4p_changed(p);
    return (p->props |= i);
}

// Remove a polygon property flag
V4pProps v4p_removeProp(V4pPolygonP p, V4pProps i) {
    v4p_changed(p);
    return (p->props &= ~i);
}

// Set polygon coordinate system (relative = TRUE for view-related, FALSE for scene-absolute)
V4pProps v4p_setRelative(V4pPolygonP p, Boolean relative) {
    v4p_changed(p);
    if (relative) {
        return (p->props |= V4P_RELATIVE);
    } else {
        return (p->props &= ~V4P_RELATIVE);
    }
}

// Add a polygon point
V4pPointP v4p_addPoint(V4pPolygonP p, V4pCoord x, V4pCoord y) {
    V4pPointP s = QuickHeapAlloc(v4p->pointHeap);
    s->x = x;
    s->y = y;
    V4pCoord r = p->radius;
    if (x != V4P_NIL && y != V4P_NIL) {
        if (p->miny == V4P_NIL) {
            p->minx = x - r;
            p->maxx = x + r;
            p->miny = y - r;
            p->maxy = y + r;
        } else {
            if (x - r < p->minx) {
                p->minx = x - r;
            }
            if (x + r > p->maxx) {
                p->maxx = x + r;
            }
            if (y - r < p->miny) {
                p->miny = y - r;
            }
            if (y + r > p->maxy) {
                p->maxy = y + r;
            }
        }
    }
    s->next = p->point1;
    p->point1 = s;
    v4p_changed(p);
    return s;
}

// Add a "jump" point into a polygon
V4pPointP v4p_addJump(V4pPolygonP p) {
    V4pPointP s = QuickHeapAlloc(v4p->pointHeap);
    s->x = V4P_NIL;
    s->y = V4P_NIL;
    s->next = p->point1;
    p->point1 = s;
    v4p_changed(p);
    return s;
}

// Set polygon radius (disk)
V4pCoord v4p_setRadius(V4pPolygonP p, V4pCoord radius) {
    V4pCoord delta = p->radius - radius;
    if (p->miny != V4P_NIL) {
        p->minx -= delta;
        p->maxx += delta;
        p->maxy -= delta;
        p->maxy += delta;
    }
    p->radius = radius;
    v4p_changed(p);
    return radius;
}

// set polygon color
V4pColor v4p_setColor(V4pPolygonP p, V4pColor c) {
    // Not changed because not affecting geometry
    return p->color = c;
}

// set polygon layer (z-depth)
V4pColor v4p_setLayer(V4pPolygonP p, V4pLayer z) {
    // Not changed because not affecting geometry
    return p->z = z & 31;  // can't be more than 31 with 32-bit mask
}

// returns a polygon id
UInt32 v4p_getId(V4pPolygonP p) {
    return p->id;
}

// returns a polygon collision mask
V4pCollisionMask v4p_getCollisionMask(V4pPolygonP p) {
    return p->collisionMask;
}

// returns a polygon points list
V4pPointP v4p_getPoints(V4pPolygonP p) {
    return p->point1;
}

// returns a polygon depth (layer index)
V4pLayer v4p_getLayer(V4pPolygonP p) {
    return p->z;
}

// returns a polygon color
V4pColor v4p_getColor(V4pPolygonP p) {
    return p->color;
}

// get the polygon limits (bounding box)
V4pPolygonP v4p_getLimits(V4pPolygonP p, V4pCoord* minx, V4pCoord* maxx, V4pCoord* miny, V4pCoord* maxy) {
    if (p->miny == V4P_NIL) {
        v4p_computeLimits(p);
    }
    *minx = p->minx;
    *maxx = p->maxx;
    *miny = p->miny;
    *maxy = p->maxy;
    return p;
}

// move a polygon point
V4pPointP v4p_movePoint(V4pPolygonP p, V4pPointP s, V4pCoord x, V4pCoord y) {
    V4pCoord r = p->radius;
    if (p->miny == V4P_NIL || (x == V4P_NIL || y == V4P_NIL)) {
    } else if (s->x - r == p->minx || s->y - r == p->miny || s->x + r == p->maxx || s->y + r == p->maxy) {
        p->miny = V4P_NIL;  // boundaries to be computed again
    } else {
        if (x - r < p->minx) p->minx = x - r;
        if (x + r > p->maxx) p->maxx = x + r;
        if (y - r < p->miny) p->miny = y - r;
        if (y + r > p->maxy) p->maxy = y + r;
    }
    s->x = x;
    s->y = y;
    v4p_changed(p);
    return s;
}

// remove a point from a polygon
V4pPolygonP v4p_destroyPointFrom(V4pPolygonP p, V4pPointP s) {
    V4pPointP pps, ps;

    if (p->point1 == s)
        p->point1 = s->next;
    else {
        pps = p->point1;
        ps = pps->next;
        while (ps != s && ps) {
            pps = ps;
            ps = ps->next;
        }
        if (! ps) return NULL;
        pps->next = ps->next;
    }

    V4pCoord r = p->radius;
    if (p->miny != V4P_NIL
        && (s->x - r == p->minx || s->y - r == p->miny || s->x + r == p->maxx || s->y + r == p->maxy)) {
        p->miny = V4P_NIL;  // boundaries to be computed again
    }

    QuickHeapFree(v4p->pointHeap, s);

    v4p_changed(p);
    return p;
}

// set the polygon collision mask
V4pPolygonP v4p_setCollisionMask(V4pPolygonP p, V4pCollisionMask collisionMask) {
    p->collisionMask = collisionMask;
    return p;
}

// Create an ActiveEdge of a polygon
ActiveEdgeP v4p_addNewActiveEdge(V4pPolygonP p, V4pPointP a, V4pPointP b) {
    ActiveEdgeP ae = QuickHeapAlloc(v4p->activeEdgeHeap);
    ae->p = p;
    ae->circle = false;
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

    if (p->props & V4P_RELATIVE) {  // Relative polygon
        ae->x0v = sx0;
        ae->y0v = sy0;
        ae->x1v = sx1;
        ae->y1v = sy1;
    }

    return ae;
}

// Create a circle edge for a polygon point
Boolean v4p_addNewCircleEdges(V4pPolygonP p, V4pPointP c) {
    V4pCoord r = p->radius, x = c->x, y = c->y;

    v4p_trace(CIRCLE, "Creating circle edges for point (%d, %d), radius=%d\n", x, y, r);

    V4pPoint top = { x: x, y: y - r }, bottom_left = { x: x - r, y: y + r }, bottom_right = { x: x + r, y: y + r };

    v4p_trace(CIRCLE, "Circle bounding box - top:(%d,%d), left:(%d,%d), "
               "right:(%d,%d)\n",
               top.x, top.y, bottom_left.x, bottom_left.y, bottom_right.x, bottom_right.y);

    ActiveEdgeP left = v4p_addNewActiveEdge(p, &top, &bottom_left);
    left->circle = true;
    v4p_trace(CIRCLE, "Created left circle edge %p\n", (void*) left);

    ActiveEdgeP right = v4p_addNewActiveEdge(p, &top, &bottom_right);
    right->circle = true;
    v4p_trace(CIRCLE, "Created right circle edge %p\n", (void*) right);
    return success;
}

// delete all ActiveEdges of a poly
V4pPolygonP v4p_destroyActiveEdges(V4pPolygonP p) {
    List l;
    ActiveEdgeP b;
    l = p->ActiveEdge1;
    while (l) {
        b = (ActiveEdgeP) ListData(l);
        QuickHeapFree(v4p->activeEdgeHeap, b);
        l = ListFree(l);
    }
    p->ActiveEdge1 = NULL;
    return p;
}

// Called by v4p_transformClone ()
V4pPolygonP v4p_recPolygonTransformClone(Boolean estSub, V4pPolygonP p, V4pPolygonP c, V4pCoord dx, V4pCoord dy,
                                         int angle, V4pLayer dz, V4pCoord anchor_x, V4pCoord anchor_y, V4pCoord zoom_x,
                                         V4pCoord zoom_y) {
    V4pPointP sp, sc;
    V4pCoord x, y, x2, y2, tx, ty;

    c->z = (p->z + dz) & 31;  // Shift z
    if (c->radius && zoom_x == zoom_y) {  // Scale radius (only if zoom_x == zoom_y)
        c->radius = (p->radius * zoom_x) >> 8;
    }

    c->miny = V4P_NIL;  // Invalidate computed boundaries

    sp = p->point1;
    sc = c->point1;

    computeCosSin(angle);

    while (sp) {
        x = sp->x;
        y = sp->y;
        if (x != V4P_NIL && y != V4P_NIL) {
            // Translate point relative to anchor
            tx = x - anchor_x;
            ty = y - anchor_y;

            // Apply rotation
            straighten(tx, ty, &x2, &y2);

            // Apply zoom/scaling
            x2 = (x2 * zoom_x) / 256;
            y2 = (y2 * zoom_y) / 256;

            // Translate back and apply position delta
            sc->x = x2 + anchor_x + dx;
            sc->y = y2 + anchor_y + dy;
        } else {
            sc->x = V4P_NIL;
            sc->y = V4P_NIL;
        }
        sp = sp->next;
        sc = sc->next;
    }
    v4p_changed(c);
    if (estSub && p->next) {
        v4p_recPolygonTransformClone(true, p->next, c->next, dx, dy, angle, dz, anchor_x, anchor_y, zoom_x, zoom_y);
    }
    if (p->sub1) {
        v4p_recPolygonTransformClone(true, p->sub1, c->sub1, dx, dy, angle, dz, anchor_x, anchor_y, zoom_x, zoom_y);
    }
    return c;
}

// Transform a clone c of a polygon p so that points(c) =
// transfo(points(p),delta-x/y, turn-angle)
V4pPolygonP v4p_transformClone(V4pPolygonP p, V4pPolygonP c, V4pCoord dx, V4pCoord dy, int angle, V4pLayer dz,
                               V4pCoord zoom_x, V4pCoord zoom_y) {
    /* a voir: ratiox et ratioy :
       cosa:=(cosa*ratiox) shr 7;
       sina:=(sina*ratioy) shr 7;
     */
    // Use the clone's anchor point for the entire transformation tree
    V4pCoord anchor_x = c->anchor_x;
    V4pCoord anchor_y = c->anchor_y;
    return v4p_recPolygonTransformClone(false, p, c, dx, dy, angle, dz, anchor_x, anchor_y, zoom_x, zoom_y);
}

// Transform a polygon
V4pPolygonP v4p_transform(V4pPolygonP p, V4pCoord dx, V4pCoord dy, int angle, V4pLayer dz, V4pCoord zoom_x,
                          V4pCoord zoom_y) {
    if (p->parent) {  // If this polygon has a parent
        // Transform relatively to parent
        return v4p_transformClone(p->parent, p, dx, dy, angle, dz, zoom_x, zoom_y);
    } else {
        // Otherwise, transform in place
        return v4p_transformClone(p, p, dx, dy, angle, dz, zoom_x, zoom_y);
    }
}

// called by v4p_polygonClone
V4pPolygonP v4p_recPolygonClone(Boolean estSub, V4pPolygonP p) {
    V4pPointP s;
    V4pPolygonP c = v4p_new(p->props, p->color, p->z);
    c->radius = p->radius;
    for (s = p->point1; s; s = s->next) v4p_addPoint(c, s->x, s->y);

    // Set parent reference for clones (but not for sub-polygons)
    if (! estSub) {
        c->parent = p;
        // Also copy the anchor point from the parent
        c->anchor_x = p->anchor_x;
        c->anchor_y = p->anchor_y;
    }

    if (estSub && p->next) c->next = v4p_recPolygonClone(true, p->next);
    if (p->sub1) c->sub1 = v4p_recPolygonClone(true, p->sub1);

    return c;
}

// clone a polygon (including its descendants) with parent reference
V4pPolygonP v4p_clone(V4pPolygonP p) {
    return v4p_recPolygonClone(false, p);
}

// combo PolygonClone+SceneAdd
V4pPolygonP v4p_sceneAddClone(V4pSceneP s, V4pPolygonP p) {
    V4pPolygonP c = v4p_clone(p);
    v4p_sceneAdd(s, c);
    return c;
}

V4pPolygonP v4p_addClone(V4pPolygonP p) {
    return v4p_sceneAddClone(v4p->scene, p);
}

// set polygon anchor point to its center
V4pPolygonP v4p_setAnchorToCenter(V4pPolygonP p) {
    if (p->miny == V4P_NIL) {
        v4p_computeLimits(p);
    }
    p->anchor_x = (p->minx + p->maxx) / 2;
    p->anchor_y = (p->miny + p->maxy) / 2;
    return p;
}

// set polygon anchor point manually
V4pPolygonP v4p_setAnchor(V4pPolygonP p, V4pCoord x, V4pCoord y) {
    p->anchor_x = x;
    p->anchor_y = y;
    return p;
}

// compute the minimal rectangle surrounding a polygon
V4pPolygonP v4p_computeLimits(V4pPolygonP p) {
    V4pCoord minx = V4P_NIL, maxx = V4P_NIL, miny = V4P_NIL, maxy = V4P_NIL;
    V4pPointP s = p->point1;
    while (s && (s->x == V4P_NIL || s->y == V4P_NIL)) {
        s = s->next;
    }
    if (s) {  // at least one point
        V4pCoord r = p->radius;
        minx = s->x - r;
        maxx = s->x + r;
        miny = s->y - r;
        maxy = s->y + r;
        for (s = s->next; s; s = s->next) {
            V4pCoord x = s->x, y = s->y;
            if (x == V4P_NIL || y == V4P_NIL) continue;

            if (x - r < minx) minx = x - r;
            if (x + r > maxx) maxx = x + r;
            if (y - r < miny) miny = y - r;
            if (y + r > maxy) maxy = y + r;
        }
    }
    p->minx = minx;
    p->miny = miny;
    p->maxx = maxx;
    p->maxy = maxy;
    return p;
}

// transform relative coordinates into absolute (scene related) ones
void v4p_viewToAbsolute(V4pCoord x, V4pCoord y, V4pCoord* xa, V4pCoord* ya) {
    int lineWidth = v4p_displayWidth, lineNb = v4p_displayHeight;
    *xa = v4p->xvu0 + x * v4p->divxvub + (x * v4p->modxvub) / lineWidth + (x < 0 && v4p->modxvub ? -1 : 0);
    *ya = v4p->yvu0 + y * v4p->divyvub + (y * v4p->modyvub) / lineNb + (y < 0 && v4p->modyvub ? -1 : 0);
}

// transform absolute coordinates into relative (scene related) ones
void v4p_absoluteToView(V4pCoord x, V4pCoord y, V4pCoord* xa, V4pCoord* ya) {
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
Boolean v4p_isVisible(V4pPolygonP p) {
    if (! p->point1) return false;
    if (p->miny == V4P_NIL)  // unknown limits
        v4p_computeLimits(p);

    V4pCoord minx = p->minx, maxx = p->maxx, miny = p->miny, maxy = p->maxy;

    if (! (p->props & V4P_RELATIVE)) {
        v4p_absoluteToView(minx, miny, &minx, &miny);
        v4p_absoluteToView(maxx, maxy, &maxx, &maxy);
    }
    p->minyv = miny;
    p->maxyv = maxy;
    return (maxx >= 0 && maxy >= 0 && minx < v4p_displayWidth && miny < v4p_displayHeight);
}

// build a list of ActiveEdges for a given polygon
V4pPolygonP v4p_buildActiveEdgeList(V4pPolygonP p) {
    Boolean isVisible = false;
    Boolean isCircle = p->radius > 0;

    if (! (p->props & V4P_CHANGED)) {
        // This polygon has not changed. Let's try to be smart
        if (p->props & V4P_RELATIVE) {  // This polygon is defined in view
            // coordinates. No change.
            return p;
        } else if (! (v4p->changes & V4P_CHANGED_VIEW)) {
            // Polygon coordinates are absolute but the view window didn't
            // change. No change.
            return p;
        } else {  // This absolute polygon hasn't changed but it might have
                  // moved within view referential. we simply update its
                  // boundaries in view coordinates. V4pCoord stub;
            // v4p_absoluteToView(0, p->miny, &stub, &(p->minyv));
            // v4p_absoluteToView(0, p->maxy, &stub, &(p->maxyv));
            isVisible = v4p_isVisible(p);
            if (isVisible) {
                if (p->ActiveEdge1) {
                    // if AE lists are set, we return because they are up-to-date.
                    return p;
                }
            } else {
                if (p->ActiveEdge1) {
                    // if AE lists are set but the polygon is not visible, we
                    // need to destroy them to avoid keeping useless data.
                    return v4p_destroyActiveEdges(p);
                }
            }
        }
    } else {
        isVisible = v4p_isVisible(p);

        // Remember than at least one polygon is changed
        v4p->changes |= (p->props & V4P_RELATIVE) ? V4P_CHANGED_RELATIVE : V4P_CHANGED_ABSOLUTE;
        p->props &= ~V4P_CHANGED;  // remove the flag saying this polygon is changed.
    }

    // Need to recompile AE
    // ====================
    v4p_destroyActiveEdges(p);

    if ((p->props & (V4P_DISABLED | V4P_IN_DISABLED | V4P_UNVISIBLE))) return p;

    if (! isVisible) {
        // don't build AE lists of hidden polygons
        return p;
    }

    V4pPointP s1 = p->point1;
    v4p_trace(POLYGON, "Building active edges for polygon %p, isCircle=%d, "
               "radius=%d\n",
               (void*) p, isCircle, p->radius);

    while (s1) {  // path subset
        if (s1->x == V4P_NIL || s1->y == V4P_NIL) {
            s1 = s1->next;
            continue;
        }
        V4pPointP sa = s1, sb = sa->next;
        v4p_trace(POLYGON, "Processing point (%d, %d), next=%p\n", sa->x, sa->y, (void*) sb);

        // sub-path loop
        while (sb && sb->x != V4P_NIL && sb->y != V4P_NIL) {  // while in sub-path
            v4p_trace(POLYGON, "Processing edge from (%d, %d) to (%d, %d)\n", sa->x, sa->y, sb->x, sb->y);

            if (sa->y != sb->y) {  // add an active edge
                v4p_addNewActiveEdge(p, sa, sb);
            }
            if (sa != s1 && sb->x == s1->x && sb->y == s1->y) {  // the path is closed
                break;
            }

            if (isCircle) {  // add circle edges around vertice
                v4p_trace(POLYGON, "Adding circle edges for vertex (%d, %d)\n", sa->x, sa->y);
                v4p_addNewCircleEdges(p, sa);
            }

            // next edge
            sa = sb;
            sb = sb->next;
        }
        if (! sb) {  // no more vertice
            v4p_trace(POLYGON, "End of path subset, last point (%d, %d)\n", sa->x, sa->y);

            if (isCircle) {
                // add circle edge around last vertice (or only vertice for
                // single-point polygons)
                v4p_trace(POLYGON, "Adding circle edges for final vertex (%d, %d)\n", sa->x, sa->y);
                v4p_addNewCircleEdges(p, sa);
            }

            if (sa->y != s1->y) {  // add a closing edge
                v4p_trace(POLYGON, "Adding closing edge from (%d, %d) to (%d, %d)\n", sa->x, sa->y, s1->x, s1->y);
                v4p_addNewActiveEdge(p, sa, s1);
            }
            break;
        }
        s1 = sb->next;
    }  // path subset

    v4p_trace(POLYGON, "Finished building active edges for polygon %p\n", (void*) p);

    return p;
}

// called by v4p_sortActiveEdge()
int compareActiveEdgeX(void* data1, void* data2) {
    ActiveEdgeP b1 = data1, b2 = data2;
    return (b1->x < b2->x);
}

// sort an ActiveEdge list ordered by 'x'
List v4p_sortActiveEdge(List list) {
    ListSetDataPrior(compareActiveEdgeX);
    return ListSort(list);
}

// build AE lists
void v4p_buildOpenableAELists(V4pPolygonP polygonChain) {
    V4pPolygonP p;
    List l;
    ActiveEdgeP b;

    for (p = polygonChain; p; p = p->next) {
        int isRelative = p->props & V4P_RELATIVE;

        v4p_buildActiveEdgeList(p);

        l = p->ActiveEdge1;
        while (l) {
            b = (ActiveEdgeP) ListData(l);
            if (isRelative) {
                QuickTableAdd(v4p->openableAETable, (b->y0 > 0 ? b->y0 : 0) & YHASH_MASK, l);
            } else {
                v4p_absoluteToView(b->x0, b->y0, &(b->x0v), &(b->y0v));
                v4p_absoluteToView(b->x1, b->y1, &(b->x1v), &(b->y1v));
                if (b->y0 < v4p->yvu0) {
                    QuickTableAdd(v4p->openableAETable, 0, l);
                } else {
                    QuickTableAdd(v4p->openableAETable, b->y0v & YHASH_MASK, l);
                }
            }
            l = ListNext(l);
        }

        if (p->sub1) {
            v4p_buildOpenableAELists(p->sub1);
        }
    }
}

// open all new scan-line intersected ActiveEdge, returns them as a list
List v4p_openActiveEdge(V4pCoord yl, V4pCoord yu) {
    List newlyOpenedAEList = NULL;
    List l;
    ActiveEdgeP b;

    V4pCoord xr0, yr0, xr1, yr1, dx, dy, q, r;

    l = QuickTableGet(v4p->openableAETable, yl & YHASH_MASK);
    for (; l; l = l->quick) {
        b = (ActiveEdgeP) ListData(l);
        xr0 = b->x0v;
        yr0 = b->y0v;
        xr1 = b->x1v;
        yr1 = b->y1v;

        v4p_trace(EDGE, "Candidate edge %p: (%d,%d) to (%d,%d), circle=%d\n", (void*) b, xr0, yr0, xr1, yr1,
                   b->circle);

        if (yl == 0) {
            if (yr0 > 0) continue;
        } else if (yr0 != yl)
            continue;
        if (yr1 <= yl) continue;

        b->h = yr1 - yl - 1;
        b->x = xr0;
        dx = xr1 - xr0;
        dy = yr1 - yr0;

        v4p_trace(OPEN, "Opening edge %p, height=%d, dx=%d, dy=%d\n", (void*) b, b->h, dx, dy);

        if (! b->circle) {
            q = dx / dy;
            r = dx > 0 ? dx % dy : (-dx) % dy;
            b->o1 = q;
            b->o2 = b->o1 + (dx > 0 ? 1 : -1);
            b->r1 = r;
            b->r2 = r - dy;
            b->s = -dy;
            int dy2 = yl - yr0;
            if (dy2 > 0) {  // edge top truncation
                b->x += dy2 * q + dy2 * (dx > 0 ? r : -r) / dy;
                b->s += (dy2 * r) % dy;
            }
        }
        ListAddData(newlyOpenedAEList, b);
    }
    if (newlyOpenedAEList) newlyOpenedAEList = v4p_sortActiveEdge(newlyOpenedAEList);
    return newlyOpenedAEList;
}

// Render a scene
Boolean v4p_render() {
    List l, pl;
    V4pPolygonP p;
    V4pLayer z;  // p->z
    ActiveEdgeP b;
    V4pCoord y;  // scanline.y
    V4pCoord px, px_collide;

    V4pCoord yu;
    int su, ou1, ou2, ru1, ru2;

    V4pPolygonP openedPolygons[32];  // Active polygon per layer
    UInt32 openBitmask;  // Bitmask of layers with active polygon
    V4pPolygonP visiblePolygon;  // Visible (opened at top) polygon

    V4pPolygonP concretePolygons[32];  // Concrete active polygon per layer
    UInt32 concreteBitmask;  // Bitmask of layer with active concrete polygon

    v4pi_setContext(v4p->display);

    v4pi_start();

    // Update AE lists and build an y-index hash table
    QuickTableReset(v4p->openableAETable);
    v4p_buildOpenableAELists(v4p->scene->polygons);

    // List of opened ActiveEdges
    v4p->openedAEList = NULL;

    // yu (scanline y in absolute coordinates) progression during scanline loop
    ou1 = v4p->divyvub;
    ou2 = v4p->divyvub + 1;
    yu = v4p->yvu0 - ou2;
    ru1 = v4p->modyvub;
    ru2 = v4p->modyvub - v4p_displayHeight;
    su = v4p->modyvub;

    // Scan-line loop
    for (y = 0; y < v4p_displayHeight; y++) {
        Boolean sortNeeded = false;

        if (su >= 0) {
            su += ru2;
            yu += ou2;
        } else {
            su += ru1;
            yu += ou1;
        }

        v4p_trace(SCAN, "Render y=%d yu=%d\n", y, yu);

        // Loop among opened ActiveEdge
        l = v4p->openedAEList;
        pl = NULL;
        px = -(0x7FFF);  // Not sure its really the min, but we dont care
        while (l) {
            b = (ActiveEdgeP) ListData(l);
            if (b->h <= 0) {  // Close ActiveEdge
                v4p_trace(OPEN, "Closing edge %p at y=%d\n", (void*) b, y);
                if (pl) {
                    ListSetNext(pl, l = ListFree(l));
                } else {
                    v4p->openedAEList = l = ListFree(l);
                }
            } else {  // Shift ActiveEdge
                int x;
                b->h--;
                if (b->circle) {
                    V4pCoord xr0 = b->x0v;
                    V4pCoord yr0 = b->y0v;
                    V4pCoord xr1 = b->x1v;
                    V4pCoord yr1 = b->y1v;
                    V4pCoord r = (yr1 - yr0) / 2;

                    UInt32 sign = -(xr1 <= xr0) | (xr1 > xr0);  // Sign -1 (left) or 1 (right)
                    V4pCoord dy = (2 * y - yr1 - yr0) / 2;  // dy: -r->0->r
                    UInt32 dx = isqrt(r * r - dy * dy);

                    x = xr0 + sign * dx;
                    b->x = x;
                    v4p_trace(SHIFT, "Shift circle edge %p "
                               "(%d,%d)x(%d,%d) to x=%d, y=%d\n",
                               (void*) b, xr0, yr0, xr1, yr1, x, y);

                } else {
                    v4p_trace(SHIFT, "Shift edge %p (%d,%d)x(%d,%d) to "
                               "x=%d, y=%d\n",
                               (void*) b, b->x0v, b->y0v, b->x1v, b->y1v, x, y);
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
                l = ListNext(l);
            }
        }  // Opened ActiveEdge loop

        // Sort ActiveEdge
        if (sortNeeded) {
            v4p->openedAEList = v4p_sortActiveEdge(v4p->openedAEList);
        }

        // Open newly intersected ActiveEdge
        List newlyOpenedAEList = v4p_openActiveEdge(y, yu);
        if (newlyOpenedAEList) {
            ListSetDataPrior(compareActiveEdgeX);
            v4p->openedAEList
                = (v4p->openedAEList ? ListMerge(v4p->openedAEList, newlyOpenedAEList) : newlyOpenedAEList);
        }

        // Reset opened polygons
        memset(openedPolygons, 0, sizeof(openedPolygons));
        openBitmask = 0;

        // Reset concrete polygons
        memset(concretePolygons, 0, sizeof(concretePolygons));
        concreteBitmask = 0;

        // Reset visible polygon
        int zMax = -1;  // background
        visiblePolygon = &(v4p->dummyBgPoly);  // background

        // Loop among active edges
        px = px_collide = 0;
        for (l = v4p->openedAEList; l; l = ListNext(l)) {
            b = (ActiveEdgeP) ListData(l);
            p = b->p;
            z = p->z & 31;  // Use 32-bit mask

            if ((int) z >= zMax) {  // edge not hidden by upper layers
                if (b->x > 0) {  // slice before current edge
                    if (px < v4p_displayWidth) {
                        v4pi_slice(y, imax(px, 0), imin(b->x, v4p_displayWidth), visiblePolygon->color);
                    }
                    px = b->x;
                }
            }

            // Check collisions between concrete polygons
            // only collisions between pairs in layer order are reported
            UInt32 bitmask = concreteBitmask;
            if (bitmask > 0) {
                V4pCollisionLayer topLayer = floorLog2(bitmask);
                UInt32 bitmaskMinusTop = bitmask & (~((UInt32) 1 << topLayer));
                while (bitmaskMinusTop > 0) {  // Collision with concrete layers
                    V4pCollisionLayer secondLayer = floorLog2(bitmaskMinusTop);
                    V4pPolygonP topConcrete = concretePolygons[topLayer];
                    V4pPolygonP secondConcrete = concretePolygons[secondLayer];
                    // Note collisionCallback != NULL since bitmask != 0
                    collisionCallback(topLayer, secondLayer, y, px_collide, b->x, topConcrete, secondConcrete);
                    bitmask = bitmaskMinusTop;
                    topLayer = secondLayer;
                    bitmaskMinusTop = bitmask & (~((UInt32) 1 << topLayer));
                }
            }

            UInt32 bit_z = (UInt32) 1 << z;
            openBitmask ^= bit_z;
            if (openBitmask & bit_z) {
                // Entering polygon - set as visible for this layer
                openedPolygons[z] = p;
            } else {
                // Leaving polygon - clear the layer
                openedPolygons[z] = NULL;
            }

            if ((int) z > zMax) {  // new top polygon
                visiblePolygon = p;
                zMax = z;
            } else if ((int) z == zMax) {  // leaving polygon at zMax
                // Find new top polygon
                if (openBitmask == 0) {  // No visible polygon
                    zMax = -1;
                    visiblePolygon = &(v4p->dummyBgPoly);  // Background polygon
                } else {  // Find highest visible polygon
                    zMax = floorLog2(openBitmask);
                    visiblePolygon = openedPolygons[zMax];
                }
            }

            if (collisionCallback != NULL) {
                px_collide = b->x;
                if (p->collisionMask != 0) {
                    V4pCollisionMask mask = p->collisionMask;
                    if (openedPolygons[z] == p) {
                        concreteBitmask |= mask;

                        // Store polygon in the primary collision layer
                        V4pCollisionLayer cl = floorLog2(mask);
                        concretePolygons[cl] = p;

                        // If there are additional bits set in the mask, handle them
                        V4pCollisionMask remaining_mask = mask & ~((V4pCollisionMask) 1 << cl);
                        while (remaining_mask != 0) {
                            V4pCollisionLayer additional_cl = floorLog2(remaining_mask);
                            concretePolygons[additional_cl] = p;
                            remaining_mask &= ~((V4pCollisionMask) 1 << additional_cl);
                        }
                    } else {
                        // Clear the collision mask bits when polygon is no longer active
                        concreteBitmask &= ~mask;

                        // Clear ALL concretePolygons entries for this polygon's collision layers
                        V4pCollisionMask temp_mask = mask;
                        while (temp_mask != 0) {
                            V4pCollisionLayer cl = floorLog2(temp_mask);
                            concretePolygons[cl] = NULL;  // Clear the entry
                            temp_mask &= ~((V4pCollisionMask) 1 << cl);
                        }
                    }
                }
            }

        }  // X opened ActiveEdge loop

        // Last slice
        if (px < v4p_displayWidth) {
            v4pi_slice(y, imax(0, px), v4p_displayWidth, visiblePolygon->color);
        }

    }  // Y loop ;

    l = v4p->openedAEList;
    while (l) {
        l = ListFree(l);
    }
    v4p->openedAEList = NULL;

    if (yu != v4p->yvu1 - v4p->divyvub) {
        v4p_error("problem %d != %d", (int) yu, (int) v4p->yvu1 - v4p->divyvub);
    }

    v4p->changes = 0;
    v4pi_end();
    return success;
}
// Add 4 points as a rectangle
V4pPolygonP v4p_rect(V4pPolygonP p, V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1) {
    v4p_addPoint(p, x0, y0);
    v4p_addPoint(p, x0, y1);
    v4p_addPoint(p, x1, y1);
    v4p_addPoint(p, x1, y0);
    return p;
}

// Set the collision callback
void v4p_setCollisionCallback(V4pCollisionCallback callback) {
    collisionCallback = callback;
}
