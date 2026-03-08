#include <stdio.h>
#include <stdbool.h>
#include "addons/game_engine/g4p.h"
#include "addons/game_engine/g4pi.h"

int g4p_onInit(int quality, bool fullscreen) {
    v4p_init2(quality, fullscreen);  // Normal quality, windowed
    printf("Test initialized\n");
    return 0;
}

int g4p_onTick(int32_t deltaTime) {
    // Poll and process events
    G4pEvent event;
    while (g4p_pollEvent(&event)) {
        switch (event.type) {
            case G4P_EVENT_KEY_DOWN:
                printf("Key down: %d (ctrl=%d, shift=%d, alt=%d)\n",
                       event.data.key.key, event.data.key.ctrl, 
                       event.data.key.shift, event.data.key.alt);
                break;
            case G4P_EVENT_KEY_UP:
                printf("Key up: %d\n", event.data.key.key);
                break;
            case G4P_EVENT_MOUSE_DOWN:
                printf("Mouse down: button=%d at (%d,%d), clicks=%d\n",
                       event.data.mouse.button, event.data.mouse.x, event.data.mouse.y, 
                       event.data.mouse.clicks);
                break;
            case G4P_EVENT_MOUSE_UP:
                printf("Mouse up: button=%d at (%d,%d)\n",
                       event.data.mouse.button, event.data.mouse.x, event.data.mouse.y);
                break;
            case G4P_EVENT_MOUSE_MOVE:
                printf("Mouse move: (%d,%d)\n", event.data.motion.x, event.data.motion.y);
                break;
            case G4P_EVENT_MOUSE_WHEEL:
                printf("Mouse wheel: x=%.2f, y=%.2f\n", event.data.wheel.x, event.data.wheel.y);
                break;
            default:
                printf("Other event: %d\n", event.type);
                break;
        }
    }
    return 0;
}

int g4p_onFrame() {
    return 0;
}

void g4p_onQuit() {
    printf("Test completed\n");
}



int main(int argc, char* argv[]) {
    return g4p_main(argc, argv);
}