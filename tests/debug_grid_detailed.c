#include "g4p.h"
#include "v4p.h"
#include "_v4p.h"

#define GRID_SIZE 2  // Just 2 squares to see the overlap
#define GAP_SIZE 0   // The problematic case
#define SQUARE_SIZE 30

V4pPolygonP grid_squares[GRID_SIZE][GRID_SIZE];

Boolean g4p_onInit() {
    int i, j;
    
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setBGColor(V4P_BLACK);
    
    // Create a prototype square for the grid
    V4pPolygonP proto = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 10);
    v4p_rect(proto, -SQUARE_SIZE/2, -SQUARE_SIZE/2, SQUARE_SIZE/2, SQUARE_SIZE/2);
    
    // Check the actual proto dimensions
    v4pi_debug("=== PROTOTYPE ANALYSIS ===\n");
    v4p_getLimits(proto, &proto->minx, &proto->maxx, &proto->miny, &proto->maxy);
    v4pi_debug("Proto bounds: x=[%d-%d], y=[%d-%d]\n",
              proto->minx, proto->maxx, proto->miny, proto->maxy);
    v4pi_debug("Proto width: %d, height: %d\n", proto->maxx - proto->minx, proto->maxy - proto->miny);

    // Calculate grid positioning
    int start_x = 100; // Fixed position for easier debugging
    int start_y = 100;

    v4pi_debug("\n=== GRID POSITIONING ===\n");
    v4pi_debug("Start position: (%d, %d)\n", start_x, start_y);
    v4pi_debug("Square size: %d, Gap size: %d\n", SQUARE_SIZE, GAP_SIZE);
    
    // Create just 2 squares side by side
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            int pos_x = start_x + GAP_SIZE + i * (SQUARE_SIZE + GAP_SIZE);
            int pos_y = start_y + GAP_SIZE + j * (SQUARE_SIZE + GAP_SIZE);
            
            v4pi_debug("\nSquare [%d][%d]:\n", i, j);
            v4pi_debug("  Target position: (%d, %d)\n", pos_x, pos_y);
            v4pi_debug("  Expected coverage: x=[%d-%d], y=[%d-%d]\n",
                      pos_x - SQUARE_SIZE/2, pos_x + SQUARE_SIZE/2,
                      pos_y - SQUARE_SIZE/2, pos_y + SQUARE_SIZE/2);
            
            grid_squares[j][i] = v4p_addClone(proto);
            v4p_setColor(grid_squares[j][i], i == 0 ? V4P_RED : V4P_GREEN);
            v4p_transform(grid_squares[j][i], pos_x, pos_y, 0, 0, 256, 256);
            v4p_getLimits(grid_squares[j][i], &grid_squares[j][i]->minx, &grid_squares[j][i]->maxx, &grid_squares[j][i]->miny, &grid_squares[j][i]->maxy);

            // Check actual rendered bounds
            v4pi_debug("  Actual bounds: x=[%d-%d], y=[%d-%d]\n",
                      grid_squares[j][i]->minx, grid_squares[j][i]->maxx,
                      grid_squares[j][i]->miny, grid_squares[j][i]->maxy);
            v4pi_debug("  Actual width: %d, height: %d",
                      grid_squares[j][i]->maxx - grid_squares[j][i]->minx,
                      grid_squares[j][i]->maxy - grid_squares[j][i]->miny);
        }
    }
    
    // Check overlap between the two squares
    V4pPolygonP square0 = grid_squares[0][0];
    V4pPolygonP square1 = grid_squares[0][1];
    
    v4pi_debug("\n=== OVERLAP ANALYSIS ===\n");
    v4pi_debug("Square 0 right edge: %d\n", square0->maxx);
    v4pi_debug("Square 1 left edge: %d\n", square1->minx);
    v4pi_debug("Overlap amount: %d\n", square0->maxx - square1->minx);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    (void)deltaTime;
    return success;
}

Boolean g4p_onFrame() {
    v4p_render();
    return failure; // Exit after one frame
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}