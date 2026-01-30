#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "v4pi.h"
#include "v4p.h"

int main(int argc, char* argv[]) {
    printf("Testing Linux Framebuffer V4P backend...\n");

    // Initialize the display
    if (v4pDisplayInit(V4P_QUALITY_NORMAL, false) != success) {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }

    printf("Display initialized successfully\n");
    printf("Width: %d, Height: %d\n", v4pDisplayWidth, v4pDisplayHeight);

    // Test creating a new context
    V4pDisplayP ctx = v4pDisplayNewContext(320, 240);
    if (! ctx) {
        fprintf(stderr, "Failed to create new context\n");
        v4pDisplayQuit();
        return 1;
    }

    printf("New context created successfully\n");

    // // Test setting context
    // v4pi_setContext (ctx);
    // printf("Context set, current size: %dx%d\n", v4pDisplayWidth,
    // v4pDisplayHeight);

    // Test display functions
    if (v4pi_start() != success) {
        fprintf(stderr, "v4p_displayStart failed\n");
        v4pDisplayFreeContext(ctx);
        v4pDisplayQuit();
        return 1;
    }

    // Draw a simple pattern
    for (int y = 0; y < 100; y++) {
        v4pi_slice(y, 0, 100, red);
    }

    if (v4pi_end() != success) {
        fprintf(stderr, "v4p_displayEnd failed\n");
        v4pDisplayFreeContext(ctx);
        v4pDisplayQuit();
        return 1;
    }

    printf("Display functions tested successfully\n");
    printf("Pausing for 10 seconds to view output...\n");

    // Add 10 second pause to view the output
    sleep(10);

    // Clean up
    v4pDisplayFreeContext(ctx);
    v4pDisplayQuit();

    printf("Test completed successfully!\n");
    return 0;
}