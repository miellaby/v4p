#include "v4p.h"
#include "g4p.h"

int g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);  // Normal quality, windowed
    v4p_setBGColor(V4P_BLACK);

    // Create a filled polygon (default)
    V4pPolygonP filled = v4p_addNew(V4P_ABSOLUTE, V4P_WHITE, 1);
    v4p_addCorners(filled, 50, 50, 150, 150);
    
    // Create a stroke polygon (1px outline) - square
    V4pPolygonP stroke_square = v4p_addNew(V4P_ABSOLUTE, V4P_RED, 2);
    v4p_addCorners(stroke_square, 120, 100, 200, 200);
    v4p_setStroke(stroke_square, 1);  // Set to 1px stroke mode
    
    // Create a stroke polygon with diagonals - diamond shape
    V4pPolygonP stroke_diamond = v4p_addNew(V4P_ABSOLUTE, V4P_GREEN, 3);
    v4p_addPoint(stroke_diamond, 250, 100);
    v4p_addPoint(stroke_diamond, 300, 150);
    v4p_addPoint(stroke_diamond, 250, 200);
    v4p_addPoint(stroke_diamond, 200, 150);
    v4p_setStroke(stroke_diamond, 1);  // Set to 1px stroke mode
    
    // Create a stroke polygon with more complex diagonals - star
    V4pPolygonP stroke_star = v4p_addNew(V4P_ABSOLUTE, V4P_YELLOW, 4);
    v4p_addPoint(stroke_star, 400, 100);
    v4p_addPoint(stroke_star, 420, 140);
    v4p_addPoint(stroke_star, 440, 100);
    v4p_addPoint(stroke_star, 430, 130);
    v4p_addPoint(stroke_star, 460, 130);
    v4p_addPoint(stroke_star, 420, 150);
    v4p_setStroke(stroke_star, 1);  // Set to 1px stroke mode
    
    // Create another filled polygon on top to test depth ordering
    V4pPolygonP top_filled = v4p_addNew(V4P_ABSOLUTE, V4P_BLUE, 5);
    v4p_addCorners(top_filled, 150, 150, 200, 200);
    
    return success;
}

int g4p_onTick(Int32 deltaTime) {
    // No animation needed for this test
    return success;
}

int g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}