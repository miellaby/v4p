/**
 * G4P Implementation for Emscripten DOM (WebAssembly)
 * This provides input handling for web browsers with DOM backend
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "g4pi.h"
#include "g4p.h"

// External global state (defined in g4p.c)
extern G4pState g4p_state;

// Keyboard state
static UInt16 current_key = 0;

// Gamepad state
static Boolean gamepad_connected = false;

// Keyboard event handlers
EM_BOOL key_down_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
    current_key = keyEvent->keyCode;
    
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
        case 18:  // Alt
            g4p_state.buttons[G4P_ALT] = true;
            break;
    }
    
    // Set the current key
    current_key = keyEvent->keyCode;
    g4p_state.key = keyEvent->keyCode;
    
    return true;
}

EM_BOOL key_up_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
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
        case 18:  // Alt
            g4p_state.buttons[G4P_ALT] = false;
            break;
    }
    
    // Clear the current key if this was the last key pressed
    if (keyEvent->keyCode == current_key) {
        current_key = 0;
        g4p_state.key = 0;
    }
    
    return true;
}

// Mouse event handlers
EM_BOOL mouse_move_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // For DOM backend, we need to get the position relative to the container
    EM_ASM({
        var container = document.getElementById('v4p-container');
        if (container) {
            var rect = container.getBoundingClientRect();
            setValue($0, $2 - rect.left, 'i32');
            setValue($1, $3 - rect.top, 'i32');
        }
    }, &g4p_state.xpen, &g4p_state.ypen, mouseEvent->clientX, mouseEvent->clientY);
    
    return true;
}

EM_BOOL mouse_down_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // For DOM backend, we need to get the position relative to the container
    EM_ASM({
        var container = document.getElementById('v4p-container');
        if (container) {
            var rect = container.getBoundingClientRect();
            setValue($0, $2 - rect.left, 'i32');
            setValue($1, $3 - rect.top, 'i32');
        }
    }, &g4p_state.xpen, &g4p_state.ypen, mouseEvent->clientX, mouseEvent->clientY);
    
    if (mouseEvent->button == 0) {  // Left mouse button
        g4p_state.buttons[G4P_PEN] = true;
    }
    
    return true;
}

EM_BOOL mouse_up_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // For DOM backend, we need to get the position relative to the container
    EM_ASM({
        var container = document.getElementById('v4p-container');
        if (container) {
            var rect = container.getBoundingClientRect();
            setValue($0, $2 - rect.left, 'i32');
            setValue($1, $3 - rect.top, 'i32');
        }
    }, &g4p_state.xpen, &g4p_state.ypen, mouseEvent->clientX, mouseEvent->clientY);
    
    if (mouseEvent->button == 0) {  // Left mouse button
        g4p_state.buttons[G4P_PEN] = false;
    }
    
    return true;
}

// Gamepad event handlers
EM_BOOL gamepad_callback(int eventType, const EmscriptenGamepadEvent* gamepadEvent, void* userData) {
    if (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED) {
        gamepad_connected = true;
    } else if (eventType == EMSCRIPTEN_EVENT_GAMEPADDISCONNECTED) {
        gamepad_connected = false;
    }
    
    return true;
}

// Initialize input system
void g4pi_init() {
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
}

// Update input state
void g4pi_update() {
    // Update gamepad state if connected
    if (gamepad_connected) {
        // For now, we'll just use the gamepad connection state
        // A more complete implementation would poll the gamepad state
        // using emscripten_get_gamepad_status()
    }
}

// Get ticks in milliseconds
Int32 g4p_getTicks() {
    return emscripten_get_now();
}

// pause execution
void g4pi_delay(Int32 d) {
    // Emscripten async delay using emscripten_sleep
    // This requires ASYNCIFY support
    if (d > 0) {
        emscripten_sleep(d);
    }
}

// poll user events
int g4pi_pollEvents() {
    // Update input state
    g4pi_update();
    return 0; // Return 0 to indicate no quit request
}

// Clean up input system
void g4pi_destroy() {
    // No special cleanup needed for Emscripten
}