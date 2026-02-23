/**
 * Game Engine Input for Emscripten Bitmap Backend
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "g4p.h"
#include "g4pi.h"

// External global state (defined in g4p.c)
extern G4pState g4p_state;

// Input state
static Boolean g4p_initialized = false;
static Boolean gamepad_connected = false;

// Keyboard event handlers
EM_BOOL key_down_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
    g4p_state.key = keyEvent->keyCode;
    
    // Map common keys to buttons
    switch (keyEvent->keyCode) {
        case 37:  // Left arrow
            g4p_state.buttons[G4P_LEFT] = true;
            break;
        case 38:  // Up arrow
            g4p_state.buttons[G4P_UP] = true;
            break;
        case 39:  // Right arrow
            g4p_state.buttons[G4P_RIGHT] = true;
            break;
        case 40:  // Down arrow
            g4p_state.buttons[G4P_DOWN] = true;
            break;
        case 32:  // Space
            g4p_state.buttons[G4P_SPACE] = true;
            break;
        case 16:  // Shift
            g4p_state.buttons[G4P_SHIFT] = true;
            break;
        case 17:  // Ctrl
            g4p_state.buttons[G4P_CTRL] = true;
            break;
    }
    return true;
}

EM_BOOL key_up_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
    g4p_state.key = 0;
    
    // Map common keys to buttons
    switch (keyEvent->keyCode) {
        case 37:  // Left arrow
            g4p_state.buttons[G4P_LEFT] = false;
            break;
        case 38:  // Up arrow
            g4p_state.buttons[G4P_UP] = false;
            break;
        case 39:  // Right arrow
            g4p_state.buttons[G4P_RIGHT] = false;
            break;
        case 40:  // Down arrow
            g4p_state.buttons[G4P_DOWN] = false;
            break;
        case 32:  // Space
            g4p_state.buttons[G4P_SPACE] = false;
            break;
        case 16:  // Shift
            g4p_state.buttons[G4P_SHIFT] = false;
            break;
        case 17:  // Ctrl
            g4p_state.buttons[G4P_CTRL] = false;
            break;
    }
    return true;
}

// Mouse event handlers
// Return false to allow event to bubble up to HTML elements
EM_BOOL mouse_move_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // Calculate position relative to the bitmap canvas
    EM_ASM({
        var canvas = document.getElementById('v4p-bitmap-canvas');
        if (canvas) {
            var rect = canvas.getBoundingClientRect();
            setValue($0, $2 - rect.left, 'i32');
            setValue($1, $3 - rect.top, 'i32');
        }
    }, &g4p_state.xpen, &g4p_state.ypen, mouseEvent->clientX, mouseEvent->clientY);
    
    return false; // Allow event to bubble up
}

EM_BOOL mouse_down_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // Calculate position relative to the bitmap canvas
    EM_ASM({
        var canvas = document.getElementById('v4p-bitmap-canvas');
        if (canvas) {
            var rect = canvas.getBoundingClientRect();
            setValue($0, $2 - rect.left, 'i32');
            setValue($1, $3 - rect.top, 'i32');
        }
    }, &g4p_state.xpen, &g4p_state.ypen, mouseEvent->clientX, mouseEvent->clientY);
    
    if (mouseEvent->button == 0) {  // Left mouse button
        g4p_state.buttons[G4P_PEN] = true;
    }
    
    return false; // Allow event to bubble up
}

EM_BOOL mouse_up_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // Calculate position relative to the bitmap canvas
    EM_ASM({
        var canvas = document.getElementById('v4p-bitmap-canvas');
        if (canvas) {
            var rect = canvas.getBoundingClientRect();
            setValue($0, $2 - rect.left, 'i32');
            setValue($1, $3 - rect.top, 'i32');
        }
    }, &g4p_state.xpen, &g4p_state.ypen, mouseEvent->clientX, mouseEvent->clientY);
    
    if (mouseEvent->button == 0) {  // Left mouse button
        g4p_state.buttons[G4P_PEN] = false;
    }
    
    return false; // Allow event to bubble up
}

// Gamepad event handler
EM_BOOL gamepad_callback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData) {
    gamepad_connected = (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED);
    return true;
}

// Handle key events from JavaScript (for touch controls)
EMSCRIPTEN_KEEPALIVE
void g4pi_handle_key_event(int eventType, int keyCode) {
    if (eventType == 1) { // Key down
        g4p_state.key = keyCode;
        
        // Map common keys to buttons
        switch (keyCode) {
            case 37:  // Left arrow
                g4p_state.buttons[G4P_LEFT] = true;
                break;
            case 38:  // Up arrow
                g4p_state.buttons[G4P_UP] = true;
                break;
            case 39:  // Right arrow
                g4p_state.buttons[G4P_RIGHT] = true;
                break;
            case 40:  // Down arrow
                g4p_state.buttons[G4P_DOWN] = true;
                break;
            case 32:  // Space
                g4p_state.buttons[G4P_SPACE] = true;
                break;
            case 16:  // Shift
                g4p_state.buttons[G4P_SHIFT] = true;
                break;
            case 17:  // Ctrl
                g4p_state.buttons[G4P_CTRL] = true;
                break;
        }
    } else { // Key up
        // Map common keys to buttons
        switch (keyCode) {
            case 37:  // Left arrow
                g4p_state.buttons[G4P_LEFT] = false;
                break;
            case 38:  // Up arrow
                g4p_state.buttons[G4P_UP] = false;
                break;
            case 39:  // Right arrow
                g4p_state.buttons[G4P_RIGHT] = false;
                break;
            case 40:  // Down arrow
                g4p_state.buttons[G4P_DOWN] = false;
                break;
            case 32:  // Space
                g4p_state.buttons[G4P_SPACE] = false;
                break;
            case 16:  // Shift
                g4p_state.buttons[G4P_SHIFT] = false;
                break;
            case 17:  // Ctrl
                g4p_state.buttons[G4P_CTRL] = false;
                break;
        }
        
        // Clear the current key if this was the last key pressed
        if (keyCode == g4p_state.key) {
            g4p_state.key = 0;
        }
    }
}

// Initialize input system
void g4pi_init() {
    if (g4p_initialized) return;

    // Set up keyboard event listeners
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, key_down_callback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, key_up_callback);

    // Set up mouse event listeners
    emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, mouse_move_callback);
    emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, mouse_down_callback);
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, mouse_up_callback);

    // Set up gamepad event listeners
    emscripten_set_gamepadconnected_callback(0, true, gamepad_callback);
    emscripten_set_gamepaddisconnected_callback(0, true, gamepad_callback);

    g4p_initialized = true;
}

// Update input state
int g4pi_pollEvents() {
    // No special update needed for bitmap backend
    return 0;
}

// Timing functions
Int32 g4p_getTicks() {
    return (Int32)emscripten_get_now();
}

void g4pi_delay(Int32 d) {
    emscripten_sleep(d);
}

// Cleanup
void g4pi_destroy() {
    g4p_initialized = false;
}