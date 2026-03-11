// Test: Draw a rounded rectangle with cut corners
#include "v4p.h"
#include <unistd.h>  // for sleep function

int main() {
    v4p_init();
    v4p_setBGColor(V4P_WHITE);
    v4p_setView(100, 100, v4p_displayWidth - 100, v4p_displayHeight - 100);
    
    // Create a rounded rectangle with cut corners in the middle of the screen
    V4pPolygonP rect = v4p_addNew(V4P_ABSOLUTE, V4P_BLUE, 1);
    v4p_addRoundCorners(rect, v4p_displayWidth / 2 - 100, v4p_displayHeight / 2 - 50, 
                     v4p_displayWidth / 2 + 100, v4p_displayHeight / 2 + 50, 20);
    
    v4p_render();
    sleep(2);  // Display for 2 seconds
    v4p_quit();
    
    return 0;
}