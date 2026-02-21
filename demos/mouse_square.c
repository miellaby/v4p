#include "g4p.h"
#include "v4p.h"

#define GRID_SIZE 10
#define GAP_SIZE 0
#define SQUARE_SIZE 30

V4pPolygonP grid_squares[GRID_SIZE][GRID_SIZE];
V4pPolygonP mouse_square;

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    int i, j;
    
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    
    // Create a prototype square for the grid
    V4pPolygonP proto = v4p_new(V4P_ABSOLUTE, V4P_BLUE, 10);
    v4p_rect(proto, -SQUARE_SIZE/2, -SQUARE_SIZE/2, SQUARE_SIZE/2, SQUARE_SIZE/2);
    
    // Calculate grid positioning
    int start_x = (v4p_displayWidth - (GRID_SIZE * (SQUARE_SIZE + GAP_SIZE)) + GAP_SIZE) / 2;
    int start_y = (v4p_displayHeight - (GRID_SIZE * (SQUARE_SIZE + GAP_SIZE)) + GAP_SIZE) / 2;
    
    // Create grid of squares
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            grid_squares[j][i] = v4p_addClone(proto);
            v4p_setColor(grid_squares[j][i], (i + j) % 2 == 0 ? V4P_TEAL : V4P_BLUE);
            v4p_transform(grid_squares[j][i],
                          start_x + GAP_SIZE + i * (SQUARE_SIZE + GAP_SIZE),
                          start_y + GAP_SIZE + j * (SQUARE_SIZE + GAP_SIZE),
                          0,
                          (i + j) % 2 == 1 ? 2 : 0,
                          256,
                          256);
        }
    }
    
    // Create the mouse-controlled square (different color)
    mouse_square = v4p_addClone(proto);
    v4p_setColor(mouse_square, V4P_RED);
    v4p_setLayer(mouse_square, 11);
    //v4p_rect(mouse_square, -SQUARE_SIZE/2, -SQUARE_SIZE/2, SQUARE_SIZE/2, SQUARE_SIZE/2);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    (void)deltaTime;  // Unused parameter
    
    // Move the mouse square to follow the pen position
    if (g4p_state.buttons[G4P_PEN]) {  // Only move when mouse button is pressed
        v4p_transform(mouse_square, g4p_state.xpen, g4p_state.ypen, 0, 0, 256, 256);
    }
    
    return success;  // Keep running indefinitely
}

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}