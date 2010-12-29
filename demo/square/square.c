#include "gamemachine.h"
#include "v4p.h"
#include "v4pi.h"

#define STRESS_AMOUNT 64
PolygonP scene;
PolygonP pCol;
PolygonP pColMatrix[STRESS_AMOUNT][STRESS_AMOUNT];

int iu = 0;
int diu = STRESS_AMOUNT;
int liu  = 3;


Boolean gmOnInit() {
  int j, k;
  scene = NULL;

  v4pDisplayInit(1, 0);
  v4pInit();

  v4pSetScene(&scene);
  v4pSetBGColor(blue);
 
  pCol=v4pPolygonNew(absolute, red, 10);
  v4pPolygonRect(pCol, -v4pDisplayWidth / 3 + v4pDisplayWidth, -v4pDisplayHeight / 3, v4pDisplayWidth / 3, v4pDisplayHeight / 3);

  for (j= 0; j < STRESS_AMOUNT; j++) {
    for (k = 0; k < STRESS_AMOUNT; k++) {
      pColMatrix[j][k] = v4pPolygonClone(pCol);
      v4pPolygonIntoList(pColMatrix[j][k], &scene);
      //v4pPolygonTransformClone(pCol, pColMatrix[j][k], v4pDisplayWidth * (k - STRESS_AMOUNT / 2), v4pDisplayWidth * (j - STRESS_AMOUNT / 2), 10, 0);
    }
  }
  return success;
}

Boolean gmOnIterate() {
	int i = iu, j, k;
	if (diu>0 && i >128 * STRESS_AMOUNT) diu=-diu;
	if (diu<0 && i + diu < -100) {
	  diu=-diu;
	  liu--;
	}
    v4pSetView(-v4pDisplayWidth * i / 256, -v4pDisplayHeight * i / 256, v4pDisplayWidth + v4pDisplayWidth * i / 256, v4pDisplayHeight + v4pDisplayHeight * i / 256);
    
    if (liu & 1)
      for (j= 0; j < STRESS_AMOUNT; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
          v4pPolygonTransformClone(pCol, pColMatrix[j][k], v4pDisplayWidth * (1 + k - STRESS_AMOUNT/2) / 2, v4pDisplayWidth * (1 + j - STRESS_AMOUNT/2)/2, (j * k) + i / 16, 0);
        }
      }
  iu+=diu;
  return (liu < 0);
}

Boolean gmOnFrame() {
   v4pRender();
   return success;
}

void gmOnQuit() {
  v4pDisplayQuit();
}

int main(int argc, char** argv) {
  return gmMain(argc, argv);
}

