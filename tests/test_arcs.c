// Test: Draw 4 arcs, one in each quadrant, then 16 arcs forming a full disk
#include "v4p.h"
#include <unistd.h>  // for sleep function
#include <math.h>

int main() {
    v4p_init();
    v4p_setBGColor(V4P_BLACK);

    // Create 16 arcs forming a full disk, alternating red and blue
    int center_x = 320, center_y = 240, radius = 100;
    for (int i = 0; i < 16; i++) {
        V4pPolygonP arc = v4p_addNew(V4P_ABSOLUTE, (i % 2 == 0) ? V4P_RED : V4P_BLUE, i);
        
        // Calculate start and end angles
        double angle1 = ((i - 0.25) * 22.5) * M_PI / 180.0;
        double angle2 = ((i + 1 - 0.25) * 22.5) * M_PI / 180.0;
        
        // Calculate start and end points
        int x1 = center_x + radius * cos(angle1);
        int y1 = center_y - radius * sin(angle1);
        int x2 = center_x + radius * cos(angle2);
        int y2 = center_y - radius * sin(angle2);

        v4p_addPoint(arc, center_x, center_y);  // From center
        v4p_addPoint(arc, x1, y1);  // Start point
        v4p_addEllipseCenter(arc, center_x, center_y, radius, radius);  // Center
        v4p_addPoint(arc, x2, y2);  // End point
        v4p_addPoint(arc, center_x, center_y);  // To center
    }

    v4p_render();
    sleep(20);  // Display for 2 seconds
    v4p_quit();
    
    return 0;
}
