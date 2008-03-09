#include "v4pi.h"


extern Boolean pen ;
extern Coord xpen, ypen ;

int  i,j,k,dist,mindist;

ILayer z ;

PolygonP  scene;

Coord x,y,xvu,yvu,lvu,
 xpen1,ypen1, yButton = 1,
 x0,y0,x1,y1,stepGrid ;

PolygonP spots[64] ;

Boolean pen1 ;

int iButton = 0, sel ;
int bAddition,bDel,bLayer,
 bScroll,bSave,bCol,bGrid ;

PolygonP
 pSel,
 pCol,pSelCol,
 pGrid,pSelGrid,
 pLayer,pSelLayer,
 currentPolygon,brush,focus;

PointP currentPoint, s ;

ILayer currentZ;

Color currentColor, nextColor ;

PolygonP buttons[16] ;

int addButton(Color col) {
  PolygonP button = v4pListAddPolygon(&scene, relative, col, 14);
  buttons[iButton] = button ;
  v4pPolygonRect(button, lineWidth - 10, yButton, lineWidth - 1, 9 + yButton);
  yButton+=10;
  return iButton++;
}

void ajusteSel(int s) {
  if (sel != s) {
    v4pPolygonTransform(pSel, 0, (s - sel) * 10, 0, 0);
    sel=s;
  }
}

typedef enum { idle, push, edit} GuiStatus ;

int spotNb ;
GuiStatus guiStatus ;



Boolean initApp() {
  v4pFlavorInit(green);
  scene = NULL;
  v4pSetScene(&scene);
  xvu = -lineWidth / 2;
  yvu = -lineNb / 2;
  lvu = lineWidth ;
  v4pSetView(xvu, yvu, xvu + lvu, yvu + lvu);
  spotNb=0;
  guiStatus= idle;
  brush = NULL;
  focus = NULL;
  stepGrid = 8;
  bAddition = addButton(red);
  bDel = addButton(gray);
  bLayer = addButton(blue);
  bScroll = addButton(yellow);
  bCol = addButton(black);
  bGrid = addButton(cyan);
  //bSave = addButton(fluo);
  sel = 0 ;
  currentColor = black ;
  currentZ = 7 ;
  currentPolygon = NULL;
  //(-xvu,-yvu)=milieu �cran
  pSel = v4pListAddPolygon(&scene, relative, black, 13);
  v4pPolygonRect(pSel, lineWidth - 11, 0 , lineWidth, 11);
  pCol=v4pListAddPolygon(&scene, relative | disabled,black, 14);
  v4pPolygonRect(pCol, -xvu - 20, -yvu - 20, -xvu + 20, -yvu + 20);
  pSelCol=v4pPolygonAddNewSub(pCol, relative, black, 15);
  v4pPolygonRect(pSelCol, -xvu - 18, -yvu - 18, -xvu + 18, -yvu + 18);
  pLayer=v4pListAddPolygon(&scene, relative | disabled, black, 14);
  v4pPolygonRect(pLayer, -xvu - 3, -yvu - 17, -xvu + 3, -yvu + 17);
  pSelLayer=v4pPolygonAddNewSub(pLayer, relative, red, 15);
  v4pPolygonRect(pSelLayer, -xvu - 2, -yvu - 1, -xvu + 2, -yvu + 1);
  pGrid=v4pListAddPolygon(&scene, relative | disabled, black, 14);
  v4pPolygonRect(pGrid, -xvu - 9, -yvu - 9, -xvu + 9, -yvu + 9);
  pSelGrid=v4pPolygonAddNewSub(pGrid, relative, red, 15);
  v4pPolygonRect(pSelGrid, -xvu - 2, -yvu - 2, -xvu + 2, -yvu + 2);
  return success ;
}

Coord align(Coord x) {
  if (stepGrid <= 1)
     return x ;
  else if (x>0)
     return ((x + stepGrid / 2) / stepGrid) * stepGrid;
  else
     return ((x - stepGrid / 2) / stepGrid) * stepGrid;
}

Boolean iterateApp() {
  Coord  stepGrid0, stepGridPrec,
     xs, ys,
     xi, yi,
     t1, t2 ;
  ILayer z0, precZ ;
  int selPrec ;

  if (true) {
    //v4pSetView(xvu,yvu,xvu+lvu,yvu+lvu);
    v4pAffiche();
    if (pen) {
      if (pen1) {
        xpen = (2 * xpen + xpen1) / 3;
        ypen = (2 * ypen + ypen1) / 3;
      }
      xpen1 = xpen;
      ypen1 = ypen;

      v4pViewToAbsolute(xpen, ypen, &x, &y);
      xs = align(x);
      ys = align(y);

      if (guiStatus == push) { // bar move
        if (sel == bGrid) {
         stepGridPrec = stepGrid;
         stepGrid = 1 << ((iabs(ypen-y0) / 4) % 4) ;
         if (stepGrid != stepGridPrec)
            v4pPolygonTransform(pSelGrid, stepGrid - stepGridPrec, stepGrid - stepGridPrec, 0, 0);
        } else if (sel == bCol) {
         nextColor = (((iabs(ypen - y0) + iabs(xpen - x0))) + currentColor) % 255;
         v4pPolygonSetColor(pSelCol, nextColor);
        } else if (sel == bLayer) {
         precZ = currentZ;
         currentZ = (z0 + (iabs(ypen - y0) / 4)) % 15 ;
         if (precZ != currentZ) v4pPolygonTransform(pSelLayer, 0, (precZ - currentZ) * 2, 0, 0);
        }
      } else if (guiStatus == edit) { // screen move
        if (brush) {
          if (sel == bScroll) {
            v4pListDelPolygon(&scene, brush);
            brush = NULL;
          } else {
            v4pPolygonTransform(brush, xpen - x0, ypen - y0, 0, 0);
            x0 = xpen;
            y0 = ypen;
          }
        }
        if (sel == bAddition) {
          if (currentPoint) {
             v4pPolygonTransform(spots[spotNb],
               xs - currentPoint->x,
               ys - currentPoint->y,
               0, 0);
             v4pPolygonMovePoint(currentPolygon, currentPoint, xs, ys);
          }
        } else if (sel==bGrid && currentPoint) {
          v4pPolygonMovePoint(focus, currentPoint, xs, ys);
          
        } else if (collides[2].q > 0) {
          if (sel == bScroll) {
            focus = collides[2].poly;
            x0 = xs;
            y0 = ys;
          } else if (sel == bGrid) {
            if (!focus) {
              focus = collides[2].poly;
              s = v4pPolygonGetPoints(focus);
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
            focus =collides[2].poly;
        } else if (sel == bScroll) { // scroll fond
          if (focus) {
            v4pPolygonTransform(focus, xs - x0, ys - y0, 0, 0);
            x0 = xs; y0 = ys;
          } else {
            v4pSetView(
              align(xvu + xpen - x0),
              align(yvu + ypen - y0),
              align(xvu + xpen - x0) + lvu,
              align(yvu + ypen - y0) + lvu);
          }
        }
      } else { //pen down
        if (xpen > lineWidth - 10 && ypen < yButton) { //bar pen down
         selPrec = sel;
         ajusteSel(ypen / 10);
         if (selPrec == bAddition) {
           if (currentPolygon && spotNb <= 2)
             v4pListDelPolygon(&scene, currentPolygon);
           
           while (spotNb) {
             spotNb--;
             if (spotNb < 20) v4pListDelPolygon(&scene, spots[spotNb]);
           }
         }
         spotNb = 0;
         focus = NULL;
         currentPolygon = NULL;
         currentPoint = NULL;
         x0 = xpen;
         y0 = ypen;
         if (sel == bCol) {
           v4pPolygonEnable(pCol);
         } else if (sel == bScroll) {
         } else if (sel == bLayer) {
           v4pPolygonEnable(pLayer);
           z0 = currentZ;
         } else if (sel == bGrid) {
           v4pPolygonEnable(pGrid);
           stepGrid0 = stepGrid;
           y0 = 4 * (ypen - floorLog2(stepGrid));
         }
         guiStatus = push;
        } else { // screen pen down
         if (sel == bAddition) {
           if (spotNb == 0) {
             currentPolygon = v4pListAddPolygon(&scene,standard, currentColor, currentZ);
             v4pPolygonConcrete(currentPolygon, 0);
           }
           currentPoint = v4pPolygonAddPoint(currentPolygon, xs, ys);
           if (spotNb < 20) {
             spots[spotNb] = v4pListAddPolygon(&scene,standard, currentColor,14);
             v4pPolygonRect(spots[spotNb], xs - 1 , ys - 1 , xs + 1 , ys + 1);
           }
         }
         brush = v4pListAddPolygon(&scene, relative, black, 15);
         v4pPolygonRect(brush, xpen - 1 , ypen - 1 , xpen + 1 , ypen + 1);
         v4pPolygonConcrete(brush, 2);
         x0 = xpen;
         y0 = ypen;
         guiStatus = edit;
       }//tap ecran
     }//pen down
    } else { // no pen
      if (guiStatus == push) { // bar pen up
       v4pPolygonDisable(pCol);
       if (sel == bCol) v4pPolygonSetColor(buttons[bCol], (currentColor = nextColor));
       if (sel == bLayer) v4pPolygonDisable(pLayer);
       if (sel == bGrid) v4pPolygonDisable(pGrid);
      } else if (guiStatus==edit) { // screen pen up
       if (sel == bAddition) {
         spotNb++;
       } else if (focus) {
         if (sel == bCol) v4pPolygonSetColor(focus, currentColor);
         if (sel == bDel) v4pListDelPolygon(&scene, focus);
         if (sel == bLayer) v4pPolygonTransform(focus, 0, 0, 0, currentZ - v4pPolygonGetZ(focus));
         focus = NULL;
         currentPoint = NULL;
       } else if (sel == bScroll && !focus) {
         xvu = align(xvu + (xpen - x0));
         yvu = align(yvu + (ypen - y0));
         v4pSetView(xvu, yvu, xvu + lvu, yvu + lvu);
       }
       if (brush) {
         v4pListDelPolygon(&scene, brush);
         brush = NULL;
       }
      }//pen up ecran;
    guiStatus = idle;
  }//no pen
  pen1 = pen;
 }//buffer
 return success ;
}

