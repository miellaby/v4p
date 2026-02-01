#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "v4pi.h"
#include "v4p.h"

int main(int argc, char* argv[]) {
    printf("Testing Linux Framebuffer V4P backend...\n");

    // Initialize the display
    if (v4pi_init(V4P_QUALITY_NORMAL, false) != success) {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }

    printf("Display initialized successfully\n");
    printf("Width: %d, Height: %d\n", v4p_displayWidth, v4p_displayHeight);

    // Test creating a new context
    V4piContextP ctx = v4pi_newContext(320, 240);
    if (! ctx) {
        fprintf(stderr, "Failed to create new context\n");
        v4pi_destroy();
        return 1;
    }

    printf("New context created successfully\n");

    // // Test setting context
    // v4pi_setContext (ctx);
    // printf("Context set, current size: %dx%d\n", v4p_displayWidth,
    // v4p_displayHeight);

    // Test display functions
    if (v4pi_start() != success) {
        fprintf(stderr, "v4p_displayStart failed\n");
        v4pi_destroyContext(ctx);
        v4pi_destroy();
        return 1;
    }

    // Draw a simple pattern
    for (int y = 0; y < 100; y++) {
        v4pi_slice(y, 0, 100, V4P_RED);
    }

    if (v4pi_end() != success) {
        fprintf(stderr, "v4p_displayEnd failed\n");
        v4pi_destroyContext(ctx);
        v4pi_destroy();
        return 1;
    }

    printf("Display functions tested successfully\n");
    printf("Pausing for 10 seconds to view output...\n");

    // Add 10 second pause to view the output
    sleep(10);

    // Clean up
    v4pi_destroyContext(ctx);
    v4pi_destroy();

    printf("Test completed successfully!\n");
    return 0;
}