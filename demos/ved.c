#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "v4p.h"
#include "g4p.h"
#include "lowmath.h"

/** global options with their default values */

int quality = 1;
int fullscreen = 0;



int i, j, k, dist, mindist;

V4pLayer z;

static V4pCoord xvu, yvu, lvu, xpen1, ypen1, xpen0, ypen0, yButton = 1, stepGrid;

V4pPolygonP spots[64];

Boolean pen1;

int iButton = 0, sel;
int bAddition, bDel, bLayer, bScroll, bSave, bCol, bGrid;

V4pPolygonP pSel, pCol, pSelCol, pGrid, pSelGrid, pLayer, pSelLayer, currentPolygon, brush, focus;

V4pPointP currentPoint, s;

V4pLayer currentZ;

V4pColor currentColor, nextColor;

V4pPolygonP buttons[16];

int addButton(V4pColor col) {
    V4pPolygonP button = v4p_addNew(V4P_RELATIVE, col, 14);
    buttons[iButton] = button;
    v4p_rect(button, v4p_displayWidth - 10, yButton, v4p_displayWidth - 1, 9 + yButton);
    yButton += 10;
    return iButton++;
}

void ajusteSel(int s) {
    if (sel != s) {
        v4p_transform(pSel, 0, (s - sel) * 10, 0, 0, 256, 256);
        sel = s;
    }
}

typedef enum { idle, push, edit } GuiStatus;

int spotNb;
GuiStatus guiStatus;

// Collision point callback
V4pPolygonP polygonUnderPen=NULL;
void g4p_onCollisionPoint(V4pPolygonP p1,
                          V4pPolygonP p2,
                          V4pCoord avg_x,
                          V4pCoord avg_y,
                          UInt16 count) {
    polygonUnderPen = p2;    
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_GREEN);
    xvu = -v4p_displayWidth / 2;
    yvu = -v4p_displayHeight / 2;
    lvu = v4p_displayWidth;
    v4p_setView(xvu, yvu, xvu + v4p_displayWidth, yvu + v4p_displayHeight);
    spotNb = 0;
    guiStatus = idle;
    brush = NULL;
    focus = NULL;
    stepGrid = 8;
    bAddition = addButton(V4P_RED);
    bDel = addButton(V4P_GRAY);
    bLayer = addButton(V4P_BLUE);
    bScroll = addButton(V4P_YELLOW);
    bCol = addButton(V4P_BLACK);
    bGrid = addButton(V4P_CYAN);
    // bSave = addButton(V4P_FLUO);
    sel = 0;
    currentColor = V4P_BLACK;
    currentZ = 7;
    currentPolygon = NULL;
    //(-xvu,-yvu)=milieu �cran
    pSel = v4p_addNew(V4P_RELATIVE, V4P_BLACK, 13);
    v4p_rect(pSel, v4p_displayWidth - 11, 0, v4p_displayWidth, 11);

    pCol = v4p_addNew(V4P_RELATIVE, V4P_BLACK, 14);
    v4p_rect(pCol, -xvu - 20, -yvu - 20, -xvu + 20, -yvu + 20);
    pSelCol = v4p_addNewSub(pCol, V4P_RELATIVE, V4P_BLACK, 15);
    v4p_rect(pSelCol, -xvu - 18, -yvu - 18, -xvu + 18, -yvu + 18);
    v4p_disable(pCol);

    pLayer = v4p_addNew(V4P_RELATIVE, V4P_BLACK, 14);
    v4p_rect(pLayer, -xvu - 3, -yvu - 17, -xvu + 3, -yvu + 17);
    pSelLayer = v4p_addNewSub(pLayer, V4P_RELATIVE, V4P_RED, 15);
    v4p_rect(pSelLayer, -xvu - 2, -yvu - 1, -xvu + 2, -yvu + 1);
    v4p_disable(pLayer);

    pGrid = v4p_addNew(V4P_RELATIVE, V4P_BLACK, 14);
    v4p_rect(pGrid, -xvu - 9, -yvu - 9, -xvu + 9, -yvu + 9);
    pSelGrid = v4p_addNewSub(pGrid, V4P_RELATIVE, V4P_RED, 15);
    v4p_rect(pSelGrid, -xvu - 2, -yvu - 2, -xvu + 2, -yvu + 2);
    v4p_disable(pGrid);

    // Set collision point callback
    g4p_setCollisionCallback(g4p_onCollisionPoint);

    return success;
}

V4pCoord align(V4pCoord x) {
    if (stepGrid <= 1)
        return x;
    else if (x > 0)
        return ((x + stepGrid / 2) / stepGrid) * stepGrid;
    else
        return ((x - stepGrid / 2) / stepGrid) * stepGrid;
}

Boolean g4p_onTick(Int32 deltaTime) {
    V4pCoord stepGrid0, stepGridPrec, xs, ys;
    V4pLayer z0, precZ;
    int selPrec;

    if (true) {
        // v4p_setView(xvu,yvu,xvu+lvu,yvu+lvu);
        if (g4p_state.buttons[G4P_PEN]) {
            if (pen1) {
                g4p_state.xpen = (2 * g4p_state.xpen + xpen1) / 3;
                g4p_state.ypen = (2 * g4p_state.ypen + ypen1) / 3;
            }
            xpen1 = g4p_state.xpen;
            ypen1 = g4p_state.ypen;

            int x, y;
            v4p_viewToAbsolute(g4p_state.xpen, g4p_state.ypen, &x, &y);
            xs = align(x);
            ys = align(y);

            if (guiStatus == push) {  // bar move
                if (sel == bGrid) {
                    stepGridPrec = stepGrid;
                    stepGrid = 1 << ((iabs(g4p_state.ypen - ypen0) / 4) % 4);
                    if (stepGrid != stepGridPrec)
                        v4p_transform(pSelGrid,
                                      stepGrid - stepGridPrec,
                                      stepGrid - stepGridPrec,
                                      0,
                                      0,
                                      256,
                                      256);
                } else if (sel == bCol) {
                    nextColor = (((iabs(g4p_state.ypen - ypen0) + iabs(g4p_state.xpen - xpen0)))
                                 + currentColor)
                        % 255;
                    v4p_setColor(pSelCol, nextColor);
                } else if (sel == bLayer) {
                    precZ = currentZ;
                    currentZ = (abs(g4p_state.ypen - ypen0) / 4) % 15;
                    if (precZ != currentZ)
                        v4p_transform(pSelLayer, 0, (precZ - currentZ) * 2, 0, 0, 256, 256);
                }
            } else if (guiStatus == edit) {  // screen move
                if (brush) {
                    if (sel == bScroll) {
                        v4p_destroyFromScene(brush);
                        brush = NULL;
                    } else {
                        v4p_transform(brush,
                                      g4p_state.xpen - xpen0,
                                      g4p_state.ypen - ypen0,
                                      0,
                                      0,
                                      256,
                                      256);
                        xpen0 = g4p_state.xpen;
                        ypen0 = g4p_state.ypen;
                    }
                }
                if (sel == bAddition) {
                    if (currentPoint) {
                        if (spotNb < 64)
                            v4p_transform(spots[spotNb],
                                          xs - currentPoint->x,
                                          ys - currentPoint->y,
                                          0,
                                          0,
                                          256,
                                          256);
                        v4p_movePoint(currentPolygon, currentPoint, xs, ys);
                    }
                } else if (sel == bGrid && currentPoint) {
                    v4p_movePoint(focus, currentPoint, xs, ys);

                } else if (polygonUnderPen) {
                    if (sel == bScroll) {
                        focus = polygonUnderPen;
                        xpen0 = xs;
                        ypen0 = ys;
                    } else if (sel == bGrid) {
                        if (! focus) {
                            focus = polygonUnderPen;
                            s = v4p_getPoints(focus);
                            mindist = gaugeDist(s->x - x, s->y - y);
                            currentPoint = s;
                            s = s->next;
                            while (s) {
                                dist = gaugeDist(s->x - x, s->y - y);
                                if (dist < mindist) {
                                    mindist = dist;
                                    currentPoint = s;
                                }
                                s = s->next;
                            }
                        }
                    } else
                        focus = polygonUnderPen;
                } else if (sel == bScroll) {  // scroll fond
                    if (focus) {
                        v4p_transform(focus, xs - xpen0, ys - ypen0, 0, 0, 256, 256);
                        xpen0 = xs;
                        ypen0 = ys;
                    } else {
                        v4p_setView(align(xvu + g4p_state.xpen - xpen0),
                                    align(yvu + g4p_state.ypen - ypen0),
                                    align(xvu + g4p_state.xpen - xpen0) + v4p_displayWidth,
                                    align(yvu + g4p_state.ypen - ypen0) + v4p_displayHeight);
                    }
                }
            } else {  // pen down
                if (g4p_state.xpen > v4p_displayWidth - 10
                    && g4p_state.ypen < yButton) {  // bar pen down
                    selPrec = sel;
                    ajusteSel(g4p_state.ypen / 10);
                    if (selPrec == bAddition) {
                        if (currentPolygon && spotNb <= 2)
                            v4p_destroyFromScene(currentPolygon);

                        while (spotNb) {
                            spotNb--;
                            if (spotNb < 64)
                                v4p_destroyFromScene(spots[spotNb]);
                        }
                    }
                    spotNb = 0;
                    focus = NULL;
                    currentPolygon = NULL;
                    currentPoint = NULL;
                    xpen0 = g4p_state.xpen;
                    ypen0 = g4p_state.ypen;
                    if (sel == bCol) {
                        v4p_enable(pCol);
                    } else if (sel == bScroll) {
                    } else if (sel == bLayer) {
                        v4p_enable(pLayer);
                        z0 = currentZ;
                    } else if (sel == bGrid) {
                        v4p_enable(pGrid);
                        stepGrid0 = stepGrid;
                        ypen0 = 4 * (g4p_state.ypen - floorLog2(stepGrid));
                    }
                    guiStatus = push;
                } else {  // screen pen down
                    if (sel == bAddition) {
                        if (spotNb == 0) {
                            currentPolygon = v4p_addNew(V4P_STANDARD, currentColor, currentZ);
                            v4p_setCollisionMask(currentPolygon, 1);
                        }
                        currentPoint = v4p_addPoint(currentPolygon, xs, ys);
                        if (spotNb < 64) {
                            spots[spotNb] = v4p_addNew(V4P_STANDARD, currentColor, 14);
                            v4p_rect(spots[spotNb], xs - 1, ys - 1, xs + 1, ys + 1);
                        }
                    }
                    brush = v4p_addNew(V4P_RELATIVE, V4P_BLACK, 15);
                    v4p_rect(brush,
                             g4p_state.xpen - 1,
                             g4p_state.ypen - 1,
                             g4p_state.xpen + 1,
                             g4p_state.ypen + 1);
                    v4p_setCollisionMask(brush, 2);
                    xpen0 = g4p_state.xpen;
                    ypen0 = g4p_state.ypen;
                    guiStatus = edit;
                }  // tap ecran
            }  // pen down
        } else {  // no pen
            if (guiStatus == push) {  // bar pen up
                v4p_disable(pCol);
                if (sel == bCol)
                    v4p_setColor(buttons[bCol], (currentColor = nextColor));
                if (sel == bLayer)
                    v4p_disable(pLayer);
                if (sel == bGrid)
                    v4p_disable(pGrid);
            } else if (guiStatus == edit) {  // screen pen up
                if (sel == bAddition) {
                    spotNb++;
                } else if (focus) {
                    if (sel == bCol)
                        v4p_setColor(focus, currentColor);
                    if (sel == bDel)
                        v4p_destroyFromScene(focus);
                    if (sel == bLayer)
                        v4p_transform(focus, 0, 0, 0, currentZ - v4p_getLayer(focus), 256, 256);
                    focus = NULL;
                    currentPoint = NULL;
                } else if (sel == bScroll && ! focus) {
                    xvu = align(xvu + (g4p_state.xpen - xpen0));
                    yvu = align(yvu + (g4p_state.ypen - ypen0));
                    v4p_setView(xvu, yvu, xvu + v4p_displayWidth, yvu + v4p_displayHeight);
                }
                if (brush) {
                    v4p_destroyFromScene(brush);
                    brush = NULL;
                }
            }  // pen up ecran;
            guiStatus = idle;
        }  // no pen
        pen1 = g4p_state.buttons[G4P_PEN];
    }  // buffer
    return success;
}

Boolean g4p_onFrame() {
    polygonUnderPen = NULL;
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

struct option longopts[] = { { "version", 0, 0, 'v' },
                             { "help", 0, 0, 'h' },
                             { "fullscreen", 0, 0, 'f' },
                             { "quality", 1, 0, 'q' },
                             { 0, 0, 0, 0 } };

int main(int argc, char** argv) {
    int v = 0, h = 0, lose = 0, optc;

    while ((optc = getopt_long(argc, argv, "hvf", longopts, (int*) 0)) != EOF) {
        switch (optc) {
            case 'v':
                v = 1;
                break;
            case 'h':
                h = 1;
                break;
            case 'f':
                fullscreen = 1;
                break;
            case 'q':
                quality = atoi(optarg);
                break;

            default:
                lose = 1;
                break;
        }
    }

    if (v) {  // show version
        fprintf(stderr, "%s\n", "1.0");
        if (! h)
            return 0;
    }

    if (h) {  // show help
        fprintf(stderr, "%s [-hvf] [--help] [--version] [--fullscreen]\n", argv[0]);
        fputs("  -h, --help\n", stderr);
        fputs("  -v, --version\n", stderr);
        fputs("  -f, --fullscreen\t\t\tfullscreen mode\n", stderr);
        return 0;
    }

    return g4p_main(argc, argv);
}
