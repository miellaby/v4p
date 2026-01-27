#include "gamemachine.h"
#include "v4p.h"
#include "v4pi.h"

#define COLORS_PER_ROW 16
#define COLOR_BOX_SIZE 20
#define SPACING 3

PolygonP box;
PolygonP color_boxes[256];
int current_color = 0;

Boolean gmOnInit() {
  int i;
  Coord x, y;
  
  v4pDisplayInit(1, 0); // Normal quality, windowed
  v4pInit();
  v4pSetBGColor(black);  // Black background
  box = v4pPolygonNew(absolute, i, 4);
  v4pPolygonRect(box, 0, 0, COLOR_BOX_SIZE, COLOR_BOX_SIZE);

  // Create a polygon for each color in the palette
  for (i = 0; i < 256; i++) {
    x = (i % COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
    y = (i / COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);

    color_boxes[i] = v4pAddClone(box);
    v4pPolygonTransformClone(box, color_boxes[i], x, y, 0, 0, 256, 256);
    v4pPolygonSetColor(color_boxes[i], i);
  }
  
  return success;
}

Boolean gmOnIterate() {
  int i;
  
  // Check for mouse interaction - select color based on mouse position
  if (gmMachineState.buttons[0]) { // Mouse button pressed
    int mouse_x = gmMachineState.xpen;
    int mouse_y = gmMachineState.ypen;
    
    // Calculate which color box the mouse is over
    int col = mouse_x / (COLOR_BOX_SIZE + SPACING);
    int row = mouse_y / (COLOR_BOX_SIZE + SPACING);
    int selected_color = row * COLORS_PER_ROW + col;
    
    // Validate the selection
    if (col >= 0 && col < COLORS_PER_ROW && row >= 0 && selected_color < 256) {
      current_color = selected_color;
    }

    // Adjust polygons
    Coord x, y;
    for (i = 0; i < 256; i++) {
      x = (i % COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
      y = (i / COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
      v4pPolygonTransformClone(box, color_boxes[i], x, y, (i == current_color ? 5 : 0), 0, 256, 256);
    }
  }

  
  // Add some informative text (if text rendering is available)
  // Note: This would require text rendering functionality which may not be available
  // in the basic v4p engine. You could add this later if needed.
  
  return success; // Keep running indefinitely
}

Boolean gmOnFrame() {
  v4pRender();
  return success;
}

void gmOnQuit() {
  // // Cleanup
  // int i;
  // for (i = 0; i < 256; i++) {
  //   if (color_boxes[i]) {
  //     v4pPolygonDel(color_boxes[i]);
  //   }
  // }
  v4pDisplayQuit();
}

int main(int argc, char **argv) {
  return gmMain(argc, argv);
}
