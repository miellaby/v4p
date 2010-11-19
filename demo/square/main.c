#include "gamemachine.h"
#include "v4p.h"
#include "v4pi.h"

PolygonP  scene;
Boolean gmOnInit() {
  int i;
  v4pDisplayInit(1, 1);
  v4pInit();
  scene = NULL;
  v4pSetScene(&scene);
  PolygonP pCol=v4pPolygonNew(absolute, red, 10);
  v4pPolygonRect(pCol, -v4pDisplayWidth / 3, -v4pDisplayHeight / 3,
    v4pDisplayWidth / 3, v4pDisplayHeight / 3);
  PolygonP pCenteredCol = v4pPolygonClone(pCol);
  v4pPolygonIntoList(pCenteredCol, &scene);

  for (i = 0; i<500;i++) {
   v4pSetView(-i, -i, v4pDisplayWidth + i, v4pDisplayHeight + i);
   v4pPolygonTransformClone(pCol, pCenteredCol, v4pDisplayWidth / 2, v4pDisplayHeight / 2, 45, 0);
   v4pRender();
  }
  sleep(2);
}
Boolean gmOnIterate() {
  return failure;
}

int main(int argc, char** argv) {
     return gmMain(argc, argv);
}

