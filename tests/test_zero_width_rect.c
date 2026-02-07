#include "v4p.h"
#include "g4p.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test data structure
typedef struct {
    const char* name;
    V4pCoord x0, y0, x1, y1;
    int expected_points;
    Boolean should_render;
} TestCase;

// Test cases for various rectangle configurations
TestCase test_cases[] = {
    {"Normal rectangle", 0, 0, 100, 100, 4, success},
    {"Zero width rectangle (x0 == x1)", 50, 50, 50, 100, 4, success},
    {"Zero height rectangle (y0 == y1)", 50, 50, 100, 50, 4, success},
    {"Zero width and height (single point)", 75, 75, 75, 75, 4, success},
    {"Negative coordinates", -50, -50, -10, -10, 4, success},
    {"Mixed positive/negative", -25, -25, 25, 25, 4, success},
    {"Very large rectangle", 0, 0, 1000, 1000, 4, success},
    {"Reversed coordinates (x1 < x0)", 100, 100, 50, 50, 4, success},
    {"Reversed coordinates (y1 < y0)", 100, 100, 50, 50, 4, success},
};

const int num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    
    printf("Testing zero-width and edge case rectangles...\n");
    printf("Total test cases: %d\n\n", num_test_cases);
    
    // Create test polygons for each case
    for (int i = 0; i < num_test_cases; i++) {
        V4pPolygonP p = v4p_addNew(V4P_ABSOLUTE, V4P_WHITE, i);
        
        printf("Test %d: %s\n", i, test_cases[i].name);
        printf("  Coordinates: (%d, %d) to (%d, %d)\n", 
               test_cases[i].x0, test_cases[i].y0, 
               test_cases[i].x1, test_cases[i].y1);
        
        // Create the rectangle
        v4p_rect(p, test_cases[i].x0, test_cases[i].y0, test_cases[i].x1, test_cases[i].y1);

        
        // Position the rectangle so it's visible
        v4p_transform(p, i * 120 + 50, 150, 0, 0, 256, 256);
        
        printf("\n");
    }
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // No special logic needed for this test
    return success;
}

Boolean g4p_onFrame() {
    // Only render one frame and return failure to exit
    v4p_render();
    return failure; // Exit after first frame
}

void g4p_onQuit() {
    printf("\nTest completed. Waiting 3 seconds before exit...\n");
    sleep(3); // Delay for 3 seconds to see the rendered output
    v4p_quit();
}

int main(int argc, char** argv) {
    printf("Starting zero-width rectangle test...\n");
    return g4p_main(argc, argv);
}