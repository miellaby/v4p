#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"

#define BOX_SIZE 50
#define SPACING 70
#define GRID_SIZE 3

PolygonP box_matrix[GRID_SIZE][GRID_SIZE];
int frame_counter = 0;

Boolean gmOnInit() {
  int i, j;
  v4pDisplayInit(1, 0);  // Normal quality, windowed
  v4pInit();
  v4pSetBGColor(black);  // Black background

  // Create a base box
  PolygonP original_box = v4pPolygonNew(absolute, red, 5);
  v4pPolygonRect(original_box, 0, 0, BOX_SIZE, BOX_SIZE);
  v4pPolygonSetAnchorToCenter(original_box);
  
  // Create a grid of clones
  for (j = 0; j < GRID_SIZE; j++) {
    for (i = 0; i < GRID_SIZE; i++) {
      box_matrix[j][i] = v4pAddClone(original_box);
    }
  }

  return success;  // Keep running indefinitely
}

int frame_count = 0;

Boolean gmOnIterate() {
  int i, j;
  int zoom_factor;
  
  frame_count++;
  
  // Apply different zoom levels to each box
  for (j = 0; j < GRID_SIZE; j++) {
    for (i = 0; i < GRID_SIZE; i++) {
      // Calculate zoom factor based on position and time
      zoom_factor = 128 + ((i + j + frame_count) % 256); // 0.5x to 2.0x zoom
      
      // Transform clones with different zoom levels
      v4pPolygonTransform(box_matrix[j][i],
                                    i * SPACING,
                                    j * SPACING + 100,
                                    (i * j * 2) + frame_count,
                                    0,
                                    zoom_factor, zoom_factor);
    }
  }

  return success;  // Keep running indefinitely
}

Boolean gmOnFrame() {
  return v4pRender();
}

void gmOnQuit() {
  // Cleanup if needed
}

int main(int argc, char **argv) {
  return gmMain(argc, argv);
}