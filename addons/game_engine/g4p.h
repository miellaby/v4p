#ifndef GAME_4_POCKET_H
#define GAME_4_POCKET_H
/**
 * G4P (Game 4 Pocket) Engine
 */
#include "v4p.h"
#include "collision.h"

// Button mapping enum
typedef enum {
    G4P_PEN = 0,    // Mouse button
    G4P_UP = 1,     // Up Arrow
    G4P_DOWN = 2,   // Down Arrow
    G4P_LEFT = 3,   // Left Arrow
    G4P_RIGHT = 4,  // Right Arrow
    G4P_SPACE = 5,  // Space
    G4P_SHIFT = 6,  // Shift
    G4P_ALT = 7,    // Alt
    G4P_CTRL = 8    // Ctrl
} G4pButton;

// Event types
typedef enum {
    G4P_EVENT_NONE,
    G4P_EVENT_KEY_DOWN,
    G4P_EVENT_KEY_UP,
    G4P_EVENT_MOUSE_DOWN,
    G4P_EVENT_MOUSE_UP,
    G4P_EVENT_MOUSE_MOVE,
    G4P_EVENT_MOUSE_WHEEL,
    G4P_EVENT_TEXT_INPUT
} G4pEventType;

// Key codes (extended from SDL)
typedef enum {
    G4P_KEY_UNKNOWN = 0,
    G4P_KEY_RETURN = 13,
    G4P_KEY_ESCAPE = 27,
    G4P_KEY_BACKSPACE = 8,
    G4P_KEY_TAB = 9,
    G4P_KEY_SPACE = 32,
    G4P_KEY_UP = 273,
    G4P_KEY_DOWN = 274,
    G4P_KEY_LEFT = 276,
    G4P_KEY_RIGHT = 275,
    G4P_KEY_DELETE = 127,
    G4P_KEY_HOME = 278,
    G4P_KEY_END = 279,
    G4P_KEY_PAGEUP = 280,
    G4P_KEY_PAGEDOWN = 281,
    G4P_KEY_INSERT = 277,
    G4P_KEY_LSHIFT = 304,
    G4P_KEY_RSHIFT = 303,
    G4P_KEY_LCTRL = 306,
    G4P_KEY_RCTRL = 305,
    G4P_KEY_LALT = 308,
    G4P_KEY_RALT = 307
} G4pKeyCode;

// Mouse buttons
typedef enum {
    G4P_MOUSE_LEFT = 1,
    G4P_MOUSE_MIDDLE = 2,
    G4P_MOUSE_RIGHT = 3
} G4pMouseButton;

// Event structure
typedef struct {
    G4pEventType type;
    union {
        struct {
            G4pKeyCode key;
            bool ctrl;
            bool shift;
            bool alt;
        } key;
        struct {
            G4pMouseButton button;
            V4pCoord x, y;
            int clicks;
        } mouse;
        struct {
            V4pCoord x, y;
        } motion;
        struct {
            float x, y;
        } wheel;
        struct {
            char text[32];
            int length;
        } text;
    } data;
} G4pEvent;

// Engine State
typedef struct g4pState_s {
    bool buttons[9];  // Button states
    V4pCoord xpen, ypen;
    uint16_t key;  // Legacy single key press (backend dependant)
} G4pState;

// Game 4 Pocket event polling
extern bool g4p_pollEvent(G4pEvent* event);

// Game 4 Pocket entry points
int g4p_setFramerate(int);
int g4p_getFramerate(void);
int g4p_main(int argc, char* argv[]);

// Game 4 Pocket input system callbacks
void g4pi_init();
void g4pi_destroy();

// Default V4P collision callback implementation of G4P
void g4p_onCollide(V4pCollisionLayer i1,
                   V4pCollisionLayer i2,
                   V4pCoord py,
                   V4pCoord x1,
                   V4pCoord x2,
                   V4pPolygonP p1,
                   V4pPolygonP p2);

// Game 4 Pocket output
extern G4pState g4p_state;
extern int g4p_framerate;
extern int g4p_avgFramePeriod;

// To be implemented by the caller
extern int g4p_onInit(int quality, bool fullscreen);
extern int g4p_onTick(int32_t deltaTime);
extern int g4p_onFrame();
extern void g4p_onQuit();
extern void g4p_delay(int32_t sleepTime);
#endif
