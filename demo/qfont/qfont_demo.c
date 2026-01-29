#include <stdio.h>
#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"
#include "qfont.h"

#define STRESS_AMOUNT 100
PolygonP pCol;
PolygonP pColMatrix[STRESS_AMOUNT][STRESS_AMOUNT];

int iu = 0;
int diu = STRESS_AMOUNT;
int liu  = 3;

Boolean g4pOnInit() {
  int j, k;

  v4pDisplayInit(1, 0);
  v4pInit();

  v4pSetBGColor(blue);

  pCol = v4pPolygonNew(absolute, red, 10);
  qfontDefinePolygonFromString("HELLO", pCol,
    -v4pDisplayWidth / 4, -v4pDisplayWidth / 16,
     v4pDisplayWidth / 8, v4pDisplayWidth / 8,
     5);
  qfontDefinePolygonFromString("WORLD", pCol,
    -v4pDisplayWidth / 4, v4pDisplayWidth / 16 + 5,
     v4pDisplayWidth / 8, v4pDisplayWidth / 8,
     5);

  for (j= 0; j < STRESS_AMOUNT; j++) {
    for (k = 0; k < STRESS_AMOUNT; k++) {
      pColMatrix[j][k] = v4pAddClone(pCol);
      v4pPolygonTransformClone(pCol, pColMatrix[j][k], v4pDisplayWidth * (2 + 2 * k - STRESS_AMOUNT) / 2, v4pDisplayWidth * (1 + j - STRESS_AMOUNT/2)/2, 0, 10, 256, 256);
    }
  }
  return success;
}

Boolean g4pOnTick(Int32 deltaTime) {
	int i = iu, j, k;
	if (diu>0 && i >128 * STRESS_AMOUNT) diu=-diu;
	if (diu<0 && i + diu < -100) {
	  diu=-diu;
	  liu--;
	}
    v4pSetView(-v4pDisplayWidth * i / 256, -v4pDisplayHeight * i / 256, v4pDisplayWidth + v4pDisplayWidth * i / 256, v4pDisplayHeight + v4pDisplayHeight * i / 256);

    if (0) // dead code, not compatible with qfont because of lacking horizontal edges
      for (j= 0; j < STRESS_AMOUNT; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
          v4pPolygonTransformClone(pCol, pColMatrix[j][k], v4pDisplayWidth * (1 + 2 * k - STRESS_AMOUNT/2) / 2, v4pDisplayWidth * (1 + j - STRESS_AMOUNT/2)/2, (j * k) + i / 16, 0, 256, 256);
        }
      }

  iu+=diu;
  return (liu < 0);
}

Boolean g4pOnFrame() {
  v4pRender();
  return success;
}

void g4pOnQuit() {
  printf("average %d\n", g4pAvgFramePeriod);
  v4pDisplayQuit();
}

int main(int argc, char** argv) {
  return g4pMain(argc, argv);
}