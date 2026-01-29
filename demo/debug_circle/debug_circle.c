#include "v4p.h"
#include "v4pi.h"
#include "lowmath.h"
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char **argv) {
    printf("=== DEBUG CIRCLE TEST ===\n");
    printf("Creating single red circle at center of screen\n");
    
    // Initialize v4p
    v4pDisplayInit(1, 0);  // Normal quality, windowed
    v4pInit();
    v4pSetBGColor(white);
    
    // Set view to match screen size
    v4pSetView(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    printf("Screen: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Create single circle at center
    int center_x = SCREEN_WIDTH / 2;
    int center_y = SCREEN_HEIGHT / 2;
    int radius = 100;
    
    printf("Creating circle at (%d, %d) with radius %d\n", center_x, center_y, radius);
    PolygonP circle = v4pDiskNew(absolute, red, 0, center_x, center_y, radius);
    printf("Circle created successfully\n");
    
    // Add to scene
    v4pSceneAddNewDisk(v4pGetScene(), absolute, red, 0, center_x, center_y, radius);
    
    printf("Circle added to scene\n");
    
    // Test rendering
    printf("Testing rendering...\n");
    
    int i;
    for (i = 0; i < 10000; i++) {
        printf("Frame %d/10\n", i+1);
        v4pRender();
    }
    
    printf("Test completed\n");
    
    v4pQuit();
    return 0;
}