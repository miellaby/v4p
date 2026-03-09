// Test: Draw a single red disk in the middle of a white screen
#include "v4p.h"
#include <unistd.h>  // for sleep function


int main() {
    v4p_init();
    v4p_setBGColor(V4P_WHITE);
    v4p_setView(100, 100, v4p_displayWidth - 100, v4p_displayHeight - 100);
    // Create a red disk in the middle of the screen
    V4pPolygonP disk = v4p_addNewDisk(V4P_ABSOLUTE, V4P_RED, 1, v4p_displayWidth / 2, v4p_displayHeight / 2, 50);
    // Add a black stroke circle around the red disk
    V4pPolygonP stroke = v4p_addNewDisk(V4P_ABSOLUTE, V4P_BLACK, 1, v4p_displayWidth / 2, v4p_displayHeight / 2, 55);
    v4p_setStroke(stroke, 1);
    v4p_render();
    sleep(5);  // Display for 2 seconds
    v4p_quit();
    
    return 0;
}