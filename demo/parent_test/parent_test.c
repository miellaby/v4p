#include "gamemachine.h"
#include "v4p.h"
#include "v4pi.h"

#define BOX_SIZE 20
#define SPACING 30
#define GRID_SIZE 4

PolygonP original_box;
PolygonP box_matrix[GRID_SIZE][GRID_SIZE];

Boolean gmOnInit() {
  int j, k;
  Coord x, y;
  
  v4pDisplayInit(1, 0); // Normal quality, windowed
  v4pInit();
  v4pSetBGColor(black);  // Black background
  
  // Create original box
  original_box = v4pPolygonNew(absolute, red, 5);
  v4pPolygonRect(original_box, 0, 0, BOX_SIZE, BOX_SIZE);
  v4pPolygonSetAnchorToCenter(original_box); // Set anchor to center for rotation
  
  // Create grid of clones using parent-aware methods (now default)
  for (j = 0; j < GRID_SIZE; j++) {
    for (k = 0; k < GRID_SIZE; k++) {
      x = k * SPACING;
      y = j * SPACING;
      
      // Use standard cloning method (now sets parent automatically)
      box_matrix[j][k] = v4pAddClone(original_box);
      
      // Set different colors for visualization
      v4pPolygonSetColor(box_matrix[j][k], (j * GRID_SIZE + k) * 16);
    }
  }
  
  return success;
}

int frame_count = 0;

Boolean gmOnIterate() {
  int i = frame_count++;
  int j, k;
  
  // Animate using new transform method
  for (j = 0; j < GRID_SIZE; j++) {
    for (k = 0; k < GRID_SIZE; k++) {
      // Transform clones using parent-aware method
      v4pPolygonTransform(box_matrix[j][k], 
                                    k * SPACING, 
                                    j * SPACING + (i / 4) % 20,
                                    (j * k * 2) + (i / 8),
                                    0);
    }
  }
  
  return success;
}

Boolean gmOnFrame() {
  v4pRender();
  return success;
}

void gmOnQuit() {
  // Cleanup if needed
}

int main(int argc, char **argv) {
  return gmMain(argc, argv);
}