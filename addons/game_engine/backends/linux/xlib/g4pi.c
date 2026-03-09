#include "g4p.h"
#include "g4pi.h"
#include "v4pi.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC / 1000)
#define MAX_EVENTS 32

// Event buffer for storing events between polls
typedef struct {
    G4pEvent events[MAX_EVENTS];
    int count;
    int read_ptr;
} EventBuffer;

static EventBuffer event_buffer = {0};

// Initialize the game engine
void g4pi_init() {
    // Xlib initialization is typically handled by the display system
    // This stub can be extended if needed for Xlib-specific initialization
}

// Helper function to add an event to the buffer
static void add_event(const G4pEvent* event) {
    if (event_buffer.count < MAX_EVENTS) {
        event_buffer.events[(event_buffer.read_ptr + event_buffer.count) % MAX_EVENTS] = *event;
        event_buffer.count++;
    }
}

// Helper function to convert X11 keysym to G4P key code
static G4pKeyCode xlib_to_g4p_key(KeySym keysym) {
    switch (keysym) {
        case XK_Return: return G4P_KEY_RETURN;
        case XK_Escape: return G4P_KEY_ESCAPE;
        case XK_BackSpace: return G4P_KEY_BACKSPACE;
        case XK_Tab: return G4P_KEY_TAB;
        case XK_space: return G4P_KEY_SPACE;
        case XK_Up: return G4P_KEY_UP;
        case XK_Down: return G4P_KEY_DOWN;
        case XK_Left: return G4P_KEY_LEFT;
        case XK_Right: return G4P_KEY_RIGHT;
        case XK_Delete: return G4P_KEY_DELETE;
        case XK_Home: return G4P_KEY_HOME;
        case XK_End: return G4P_KEY_END;
        case XK_Page_Up: return G4P_KEY_PAGEUP;
        case XK_Page_Down: return G4P_KEY_PAGEDOWN;
        case XK_Insert: return G4P_KEY_INSERT;
        case XK_Shift_L: return G4P_KEY_LSHIFT;
        case XK_Shift_R: return G4P_KEY_RSHIFT;
        case XK_Control_L: return G4P_KEY_LCTRL;
        case XK_Control_R: return G4P_KEY_RCTRL;
        case XK_Alt_L: return G4P_KEY_LALT;
        case XK_Alt_R: return G4P_KEY_RALT;
        default: return (G4pKeyCode)keysym; // Return the raw keysym for unmapped keys
    }
}

// Poll a single event from the event buffer
bool g4p_pollEvent(G4pEvent* event) {
    if (event_buffer.count > 0) {
        *event = event_buffer.events[event_buffer.read_ptr];
        event_buffer.read_ptr = (event_buffer.read_ptr + 1) % MAX_EVENTS;
        event_buffer.count--;
        return true;
    }
    return false;
}

// Cleanup the game engine
void g4pi_destroy() {
    // Xlib cleanup is typically handled by the display system
    // This stub can be extended if needed for Xlib-specific cleanup
}

int g4pi_pollEvents() {
    int rc = 0;
    XEvent report;
    // retrieve one event, returns if none
    Display* currentDisplay = *(Display **)v4pi_context; // XLib 1st struct attribute is display
    if (! XCheckMaskEvent(currentDisplay, (~0), &report))
        return success;

    switch (report.type) {
        case Expose:
            // remove all Expose pending events
            while (XCheckTypedEvent(currentDisplay, Expose, &report))
                ;

            // one operate Expose events only when g4p_framerate == 0
            if (g4p_framerate == 0)
                rc |= g4p_onFrame();
            break;

        case ConfigureNotify:
            /*  Store new window width & height  */
            // v4p_displayWidth = v4pi_context->width  =
            // report.xconfigure.width; v4p_displayHeight =
            // v4pi_context->height = report.xconfigure.height;
            break;

        case ButtonPress:
            g4p_state.buttons[G4P_PEN] = 1;
            {
                // Create detailed mouse event
                G4pEvent event;
                event.type = G4P_EVENT_MOUSE_DOWN;
                event.data.mouse.button = report.xbutton.button;
                event.data.mouse.x = report.xbutton.x;
                event.data.mouse.y = report.xbutton.y;
                event.data.mouse.clicks = 1;
                add_event(&event);
            }
            break;

        case ButtonRelease:
            g4p_state.buttons[G4P_PEN] = 0;
            {
                // Create detailed mouse event
                G4pEvent event;
                event.type = G4P_EVENT_MOUSE_UP;
                event.data.mouse.button = report.xbutton.button;
                event.data.mouse.x = report.xbutton.x;
                event.data.mouse.y = report.xbutton.y;
                event.data.mouse.clicks = 1;
                add_event(&event);
            }
            break;

        case MotionNotify:
            {
                int root_x, root_y;
                int pos_x, pos_y;
                Window root, child;
                unsigned int keys_buttons;

                while (XCheckMaskEvent(currentDisplay, ButtonMotionMask, &report))
                    ;
                if (! XQueryPointer(currentDisplay,
                                    report.xmotion.window,
                                    &root,
                                    &child,
                                    &root_x,
                                    &root_y,
                                    &pos_x,
                                    &pos_y,
                                    &keys_buttons))
                    break;
                g4p_state.xpen = pos_x;
                g4p_state.ypen = pos_y;
                
                // Create detailed motion event
                G4pEvent event;
                event.type = G4P_EVENT_MOUSE_MOVE;
                event.data.motion.x = pos_x;
                event.data.motion.y = pos_y;
                add_event(&event);
                break;
            }
        case KeyPress:
            {
                char buffer[32];
                KeySym keysym;
                XComposeStatus compose;
                int length = XLookupString(&report.xkey, buffer, sizeof(buffer), &keysym, &compose);
                
                // Map keys to buttons array
                switch (keysym) {
                    case XK_Up:     g4p_state.buttons[G4P_UP] = 1; break;
                    case XK_Down:   g4p_state.buttons[G4P_DOWN] = 1; break;
                    case XK_Left:   g4p_state.buttons[G4P_LEFT] = 1; break;
                    case XK_Right:  g4p_state.buttons[G4P_RIGHT] = 1; break;
                    case XK_space:  g4p_state.buttons[G4P_SPACE] = 1; break;
                    case XK_Shift_L:
                    case XK_Shift_R: g4p_state.buttons[G4P_SHIFT] = 1; break;
                    case XK_Alt_L:
                    case XK_Alt_R:   g4p_state.buttons[G4P_ALT] = 1; break;
                    case XK_Control_L:
                    case XK_Control_R:  g4p_state.buttons[G4P_CTRL] = 1; break;
                    default:
                        // Unmapped key, store in key field
                        g4p_state.key = (uint16_t)keysym;
                }
                
                // Create detailed key event
                G4pEvent event;
                event.type = G4P_EVENT_KEY_DOWN;
                event.data.key.key = xlib_to_g4p_key(keysym);
                event.data.key.ctrl = (report.xkey.state & ControlMask) != 0;
                event.data.key.shift = (report.xkey.state & ShiftMask) != 0;
                event.data.key.alt = (report.xkey.state & Mod1Mask) != 0;
                add_event(&event);
                
                if (keysym == XK_Escape)
                    rc = 1;
                break;
            }
        case KeyRelease:
            {
                KeySym keysym = XLookupKeysym(&report.xkey, 0);
                
                // Clear key buttons on key up
                switch (keysym) {
                    case XK_Up:     g4p_state.buttons[G4P_UP] = 0; break;
                    case XK_Down:   g4p_state.buttons[G4P_DOWN] = 0; break;
                    case XK_Left:   g4p_state.buttons[G4P_LEFT] = 0; break;
                    case XK_Right:  g4p_state.buttons[G4P_RIGHT] = 0; break;
                    case XK_space:  g4p_state.buttons[G4P_SPACE] = 0; break;
                    case XK_Shift_L:
                    case XK_Shift_R: g4p_state.buttons[G4P_SHIFT] = 0; break;
                    case XK_Alt_L:
                    case XK_Alt_R:   g4p_state.buttons[G4P_ALT] = 0; break;
                    case XK_Control_L:
                    case XK_Control_R:  g4p_state.buttons[G4P_CTRL] = 0; break;
                    default: // Unmapped key, clear key field if it matches
                        if (g4p_state.key == keysym)
                            g4p_state.key = 0;
                        break;
                }
                
                // Create detailed key event
                G4pEvent event;
                event.type = G4P_EVENT_KEY_UP;
                event.data.key.key = xlib_to_g4p_key(keysym);
                event.data.key.ctrl = (report.xkey.state & ControlMask) != 0;
                event.data.key.shift = (report.xkey.state & ShiftMask) != 0;
                event.data.key.alt = (report.xkey.state & Mod1Mask) != 0;
                add_event(&event);
                break;
            }
    }
    return rc;
}
