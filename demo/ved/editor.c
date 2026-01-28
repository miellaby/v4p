#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "v4p.h"
#include "v4pi.h"
#include "game_engine.h"
#include "getopt.h"
#include "lowmath.h"

/** global options with their default values */

int quality    = 1;
int fullscreen = 0;

/** collide detection thing */

typedef struct collide_s {
  Coord    x;
  Coord    y;
  UInt16   q;
  PolygonP poly;
} Collide;

extern Collide collides[16];

int            i, j, k, dist, mindist;

ILayer         z;

static Coord   xvu, yvu, lvu,
  xpen1, ypen1, xpen0, ypen0,
  yButton = 1,
  stepGrid;

PolygonP spots[64];

Boolean  pen1;

int      iButton = 0, sel;
int      bAddition, bDel, bLayer,
  bScroll, bSave, bCol, bGrid;

PolygonP
  pSel,
  pCol, pSelCol,
  pGrid, pSelGrid,
  pLayer, pSelLayer,
  currentPolygon, brush, focus;

PointP   currentPoint, s;

ILayer   currentZ;

Color    currentColor, nextColor;

PolygonP buttons[16];

int      addButton(Color col) {
  PolygonP button  = v4pAddNew(relative, col, 14);
  buttons[iButton] = button;
  v4pPolygonRect(button, v4pDisplayWidth - 10, yButton, v4pDisplayWidth - 1, 9 + yButton);
  yButton += 10;
  return iButton++;
}

void ajusteSel(int s) {
  if (sel != s) {
    v4pPolygonTransform(pSel, 0, (s - sel) * 10, 0, 0, 256, 256);
    sel = s;
  }
}

typedef enum { idle,
               push,
               edit } GuiStatus;

int       spotNb;
GuiStatus guiStatus;

Boolean   g4pOnInit() {
  v4pDisplayInit(quality, fullscreen);
  v4pInit();
  v4pSetBGColor(green);
  xvu = -v4pDisplayWidth / 2;
  yvu = -v4pDisplayHeight / 2;
  lvu = v4pDisplayWidth;
  v4pSetView(xvu, yvu, xvu + v4pDisplayWidth, yvu + v4pDisplayHeight);
  spotNb         = 0;
  guiStatus      = idle;
  brush          = NULL;
  focus          = NULL;
  stepGrid       = 8;
  bAddition      = addButton(red);
  bDel           = addButton(gray);
  bLayer         = addButton(blue);
  bScroll        = addButton(yellow);
  bCol           = addButton(black);
  bGrid          = addButton(cyan);
  // bSave = addButton(fluo);
  sel            = 0;
  currentColor   = black;
  currentZ       = 7;
  currentPolygon = NULL;
  //(-xvu,-yvu)=milieu �cran
  pSel           = v4pAddNew(relative, black, 13);
  v4pPolygonRect(pSel, v4pDisplayWidth - 11, 0, v4pDisplayWidth, 11);

  pCol = v4pAddNew(relative, black, 14);
  v4pPolygonRect(pCol, -xvu - 20, -yvu - 20, -xvu + 20, -yvu + 20);
  pSelCol = v4pPolygonAddNewSub(pCol, relative, black, 15);
  v4pPolygonRect(pSelCol, -xvu - 18, -yvu - 18, -xvu + 18, -yvu + 18);
  v4pPolygonDisable(pCol);

  pLayer = v4pAddNew(relative, black, 14);
  v4pPolygonRect(pLayer, -xvu - 3, -yvu - 17, -xvu + 3, -yvu + 17);
  pSelLayer = v4pPolygonAddNewSub(pLayer, relative, red, 15);
  v4pPolygonRect(pSelLayer, -xvu - 2, -yvu - 1, -xvu + 2, -yvu + 1);
  v4pPolygonDisable(pLayer);

  pGrid = v4pAddNew(relative, black, 14);
  v4pPolygonRect(pGrid, -xvu - 9, -yvu - 9, -xvu + 9, -yvu + 9);
  pSelGrid = v4pPolygonAddNewSub(pGrid, relative, red, 15);
  v4pPolygonRect(pSelGrid, -xvu - 2, -yvu - 2, -xvu + 2, -yvu + 2);
  v4pPolygonDisable(pGrid);

  return success;
}

Coord align(Coord x) {
  if (stepGrid <= 1)
    return x;
  else if (x > 0)
    return ((x + stepGrid / 2) / stepGrid) * stepGrid;
  else
    return ((x - stepGrid / 2) / stepGrid) * stepGrid;
}

Boolean g4pOnTick(Int32 deltaTime) {
  Coord stepGrid0, stepGridPrec,
    xs, ys;
  ILayer z0, precZ;
  int    selPrec;

  if (true) {
    // v4pSetView(xvu,yvu,xvu+lvu,yvu+lvu);
    if (g4pState.buttons[0]) {
      if (pen1) {
        g4pState.xpen = (2 * g4pState.xpen + xpen1) / 3;
        g4pState.ypen = (2 * g4pState.ypen + ypen1) / 3;
      }
      xpen1 = g4pState.xpen;
      ypen1 = g4pState.ypen;

      int x, y;
      v4pViewToAbsolute(g4pState.xpen, g4pState.ypen, &x, &y);
      xs = align(x);
      ys = align(y);

      if (guiStatus == push) {  // bar move
        if (sel == bGrid) {
          stepGridPrec = stepGrid;
          stepGrid     = 1 << ((iabs(g4pState.ypen - ypen0) / 4) % 4);
          if (stepGrid != stepGridPrec)
            v4pPolygonTransform(pSelGrid, stepGrid - stepGridPrec, stepGrid - stepGridPrec, 0, 0, 256, 256);
        } else if (sel == bCol) {
          nextColor = (((iabs(g4pState.ypen - ypen0) + iabs(g4pState.xpen - xpen0))) + currentColor) % 255;
          v4pPolygonSetColor(pSelCol, nextColor);
        } else if (sel == bLayer) {
          precZ    = currentZ;
          currentZ = (z0 + (iabs(g4pState.ypen - ypen0) / 4)) % 15;
          if (precZ != currentZ)
            v4pPolygonTransform(pSelLayer, 0, (precZ - currentZ) * 2, 0, 0, 256, 256);
        }
      } else if (guiStatus == edit) {  // screen move
        if (brush) {
          if (sel == bScroll) {
            v4pDel(brush);
            brush = NULL;
          } else {
            v4pPolygonTransform(brush, g4pState.xpen - xpen0, g4pState.ypen - ypen0, 0, 0, 256, 256);
            xpen0 = g4pState.xpen;
            ypen0 = g4pState.ypen;
          }
        }
        if (sel == bAddition) {
          if (currentPoint) {
            if (spotNb < 64)
              v4pPolygonTransform(spots[spotNb],
                                  xs - currentPoint->x,
                                  ys - currentPoint->y,
                                  0, 0, 256, 256);
            v4pPolygonMovePoint(currentPolygon, currentPoint, xs, ys);
          }
        } else if (sel == bGrid && currentPoint) {
          v4pPolygonMovePoint(focus, currentPoint, xs, ys);

        } else if (collides[2].q > 0) {
          if (sel == bScroll) {
            focus = collides[2].poly;
            xpen0    = xs;
            ypen0    = ys;
          } else if (sel == bGrid) {
            if (!focus) {
              focus        = collides[2].poly;
              s            = v4pPolygonGetPoints(focus);
              mindist      = gaugeDist(s->x - x, s->y - y);
              currentPoint = s;
              s            = s->next;
              while (s) {
                dist = gaugeDist(s->x - x, s->y - y);
                if (dist < mindist) {
                  mindist      = dist;
                  currentPoint = s;
                }
                s = s->next;
              }
            }
          } else
            focus = collides[2].poly;
        } else if (sel == bScroll) {  // scroll fond
          if (focus) {
            v4pPolygonTransform(focus, xs - xpen0, ys - ypen0, 0, 0, 256, 256);
            xpen0 = xs;
            ypen0 = ys;
          } else {
            v4pSetView(
              align(xvu + g4pState.xpen - xpen0),
              align(yvu + g4pState.ypen - ypen0),
              align(xvu + g4pState.xpen - xpen0) + v4pDisplayWidth,
              align(yvu + g4pState.ypen - ypen0) + v4pDisplayHeight);
          }
        }
      } else {                                                                              // pen down
        if (g4pState.xpen > v4pDisplayWidth - 10 && g4pState.ypen < yButton) {  // bar pen down
          selPrec = sel;
          ajusteSel(g4pState.ypen / 10);
          if (selPrec == bAddition) {
            if (currentPolygon && spotNb <= 2)
              v4pDel(currentPolygon);

            while (spotNb) {
              spotNb--;
              if (spotNb < 64)
                v4pDel(spots[spotNb]);
            }
          }
          spotNb         = 0;
          focus          = NULL;
          currentPolygon = NULL;
          currentPoint   = NULL;
          xpen0             = g4pState.xpen;
          ypen0             = g4pState.ypen;
          if (sel == bCol) {
            v4pPolygonEnable(pCol);
          } else if (sel == bScroll) {
          } else if (sel == bLayer) {
            v4pPolygonEnable(pLayer);
            z0 = currentZ;
          } else if (sel == bGrid) {
            v4pPolygonEnable(pGrid);
            stepGrid0 = stepGrid;
            ypen0        = 4 * (g4pState.ypen - floorLog2(stepGrid));
          }
          guiStatus = push;
        } else {  // screen pen down
          if (sel == bAddition) {
            if (spotNb == 0) {
              currentPolygon = v4pAddNew(standard, currentColor, currentZ);
              v4pPolygonConcrete(currentPolygon, 0);
            }
            currentPoint = v4pPolygonAddPoint(currentPolygon, xs, ys);
            if (spotNb < 64) {
              spots[spotNb] = v4pAddNew(standard, currentColor, 14);
              v4pPolygonRect(spots[spotNb], xs - 1, ys - 1, xs + 1, ys + 1);
            }
          }
          brush = v4pAddNew(relative, black, 15);
          v4pPolygonRect(brush, g4pState.xpen - 1, g4pState.ypen - 1, g4pState.xpen + 1, g4pState.ypen + 1);
          v4pPolygonConcrete(brush, 2);
          xpen0        = g4pState.xpen;
          ypen0        = g4pState.ypen;
          guiStatus = edit;
        }  // tap ecran
      }  // pen down
    } else {                    // no pen
      if (guiStatus == push) {  // bar pen up
        v4pPolygonDisable(pCol);
        if (sel == bCol)
          v4pPolygonSetColor(buttons[bCol], (currentColor = nextColor));
        if (sel == bLayer)
          v4pPolygonDisable(pLayer);
        if (sel == bGrid)
          v4pPolygonDisable(pGrid);
      } else if (guiStatus == edit) {  // screen pen up
        if (sel == bAddition) {
          spotNb++;
        } else if (focus) {
          if (sel == bCol)
            v4pPolygonSetColor(focus, currentColor);
          if (sel == bDel)
            v4pDel(focus);
          if (sel == bLayer)
            v4pPolygonTransform(focus, 0, 0, 0, currentZ - v4pPolygonGetZ(focus), 256, 256);
          focus        = NULL;
          currentPoint = NULL;
        } else if (sel == bScroll && !focus) {
          xvu = align(xvu + (g4pState.xpen - xpen0));
          yvu = align(yvu + (g4pState.ypen - ypen0));
          v4pSetView(xvu, yvu, xvu + v4pDisplayWidth, yvu + v4pDisplayHeight);
        }
        if (brush) {
          v4pDel(brush);
          brush = NULL;
        }
      }  // pen up ecran;
      guiStatus = idle;
    }  // no pen
    pen1 = g4pState.buttons[0];
  }  // buffer
  return success;
}

Boolean g4pOnFrame() {
  v4pRender();
  return success;
}

void g4pOnQuit() {
  v4pDisplayQuit();
}

struct option longopts[] =
  {
    {"version", 0, 0, 'v'},
    {"help", 0, 0, 'h'},
    {"fullscreen", 0, 0, 'f'},
    {"quality", 1, 0, 'q'},
    {0, 0, 0, 0}};

int main(int argc, char **argv) {
  int v = 0, h = 0, lose = 0, optc;

  while ((optc = getopt_long(argc, argv, "hvf", longopts, (int *)0)) != EOF) {
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

      default:
        lose = 1;
        break;
    }
  }

  if (v) {  // show version
    fprintf(stderr, "%s\n", "1.0");
    if (!h)
      return 0;
  }

  if (h) {  // show help
    fprintf(stderr, "%s [-hvf] [--help] [--version] [--fullscreen]\n", argv[0]);
    fputs("  -h, --help\n", stderr);
    fputs("  -v, --version\n", stderr);
    fputs("  -f, --fullscreen\t\t\tfullscreen mode\n", stderr);
    return 0;
  }

  return g4pMain(argc, argv);
}
