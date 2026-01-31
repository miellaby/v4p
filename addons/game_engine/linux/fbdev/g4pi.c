#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <linux/input.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include "game_engine.h"
#include "g4pi.h"

// Global variables for input handling
static int mouse_fd = -1;
static int keyboard_fd = -1;
static struct timespec start_time;

// Initialize input devices
static void init_input_devices() {
    // Try to find mouse device
    char mouse_device[256];
    sprintf(mouse_device, "/dev/input/mouse%d", 0);

    // Try common mouse devices
    for (int i = 0; i < 4; i++) {
        sprintf(mouse_device, "/dev/input/mouse%d", i);
        mouse_fd = open(mouse_device, O_RDONLY | O_NONBLOCK);
        if (mouse_fd >= 0)
            break;
    }

    // Try event devices if mouse not found
    if (mouse_fd < 0) {
        for (int i = 0; i < 8; i++) {
            sprintf(mouse_device, "/dev/input/event%d", i);
            mouse_fd = open(mouse_device, O_RDONLY | O_NONBLOCK);
            if (mouse_fd >= 0) {
                // Check if this is a mouse/pointer device
                // For simplicity, we'll assume it is
                break;
            }
        }
    }

    // Try to find keyboard device
    if (mouse_fd >= 0) {
        // If we found a mouse on eventX, try next for keyboard
        char keyboard_device[256];
        for (int i = 0; i < 8; i++) {
            sprintf(keyboard_device, "/dev/input/event%d", i);
            if (i != (mouse_fd - (mouse_fd % 10))) {  // Skip mouse device
                keyboard_fd = open(keyboard_device, O_RDONLY | O_NONBLOCK);
                if (keyboard_fd >= 0)
                    break;
            }
        }
    }

    if (mouse_fd < 0 && keyboard_fd < 0) {
        fprintf(stderr, "Warning: No input devices found\n");
    }
}

// Cleanup input devices
static void cleanup_input_devices() {
    if (mouse_fd >= 0)
        close(mouse_fd);
    if (keyboard_fd >= 0)
        close(keyboard_fd);
    mouse_fd = keyboard_fd = -1;
}

// get ticks in milliseconds
Int32 g4p_getTicks() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    // Lazy initialization - initialize start_time on first call
    static int initialized = 0;
    if (! initialized) {
        start_time = now;
        initialized = 1;
    }

    // Calculate milliseconds since start
    // Handle potential negative nanoseconds when now.tv_nsec <
    // start_time.tv_nsec
    long seconds = now.tv_sec - start_time.tv_sec;
    long nanoseconds = now.tv_nsec - start_time.tv_nsec;

    if (nanoseconds < 0) {
        seconds--;
        nanoseconds += 1000000000;  // Add 1 second in nanoseconds
    }

    // Convert to milliseconds, being careful about overflow
    // Use long long to avoid overflow during intermediate calculations
    long long total_milliseconds = (long long) seconds * 1000 + nanoseconds / 1000000;

    return (Int32) total_milliseconds;
}

// pause execution for milliseconds
void g4pi_delay(Int32 d) {
    if (d <= 0)
        return;
    struct timespec req;
    req.tv_sec = d / 1000;
    req.tv_nsec = (d % 1000) * 1000000;
    while (nanosleep(&req, &req) == -1 && errno == EINTR) {
        // Continue if interrupted by signal
    }
}

// Read mouse event from device
static void read_mouse_event() {
    if (mouse_fd < 0)
        return;

    struct input_event ev;
    while (read(mouse_fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event)) {
        if (ev.type == EV_REL) {
            if (ev.code == REL_X) {
                g4p_state.xpen += ev.value;
            } else if (ev.code == REL_Y) {
                g4p_state.ypen += ev.value;
            }
        } else if (ev.type == EV_KEY) {
            if (ev.code == BTN_LEFT) {
                g4p_state.buttons[0] = (ev.value == 1);
            }
        }
    }
}

// Read keyboard event from device
static int read_keyboard_event() {
    if (keyboard_fd < 0)
        return 0;

    struct input_event ev;
    while (read(keyboard_fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event)) {
        if (ev.type == EV_KEY && ev.value == 1) {  // Key press
            // ESC key (keycode 1) or any key to quit
            return 1;
        }
    }
    return 0;
}

// poll user events
int g4pi_pollEvents() {
    int rc = 0;  // return code

    // Read mouse events
    read_mouse_event();

    // Read keyboard events
    rc = read_keyboard_event();

    return rc;
}

// Initialize the game engine input system
void g4pi_init() {
    init_input_devices();
}

// Cleanup the game engine input system
void g4pi_destroy() {
    cleanup_input_devices();
}
