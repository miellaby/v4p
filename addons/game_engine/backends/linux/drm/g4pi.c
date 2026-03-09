#include "g4pi.h"
#include "g4p.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>

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
                g4p_state.buttons[G4P_PEN] = (ev.value == 1);
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

// Poll a single event from the event buffer
bool g4p_pollEvent(G4pEvent* event) {
    // TODO
    return false;
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
