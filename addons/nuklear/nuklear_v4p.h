/*
 * MIT License
 *
 * Copyright (c) 2024 v4p integration
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*
 * ==============================================================
 *                              API
 * ===============================================================
 */

#ifndef NK_V4P_H_
#define NK_V4P_H_

#include "v4p.h"

struct nk_context;

NK_API struct nk_context *nk_v4p_init(V4pSceneP scene, V4pCoord width, V4pCoord height);
NK_API void                  nk_v4p_render(struct nk_context *ctx);
NK_API void                  nk_v4p_shutdown(struct nk_context *ctx);
NK_API void                  nk_v4p_resize(struct nk_context *ctx, V4pCoord width, V4pCoord height);
NK_API int                   nk_v4p_handle_event(struct nk_context *ctx, G4pEvent *evt);


#endif
/*
 * ==============================================================
 *                          IMPLEMENTATION
 * ===============================================================
 */
#ifdef NK_V4P_IMPLEMENTATION

#include "qfont/qfont.h"
#include "v4p_trace.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

struct nk_v4p_context {
    struct nk_context ctx; // must be first for proper casting to nk_context*
    V4pSceneP scene;  // Reference to a v4p scene
    struct nk_recti scissors;
    struct nk_user_font font;  // Custom font instead of atlas
    V4pLayer current_layer;  // Track current layer for z-ordering
};

typedef V4pColor v4p_color;

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

// Macro to check if scissor region is not nk_null_rect
// nk_null_rect is defined as {-8192, -8192, 16384, 16384}
#define NK_SCISSOR_NULL(s) ((s).w == 16384 && (s).h == 16384 && (s).x == -8192 && (s).y == -8192)

// Convert Nuklear color (0-255 RGBA) to v4p palette index
// v4p uses a 256-color palette, so we need to find the closest match
// Use alpha channel to dim the RGB channels
static v4p_color nk_v4p_color2int(const struct nk_color c) {
    // Apply alpha to dim RGB channels (alpha=0 is transparent, alpha=255 is opaque)
    unsigned char r = (c.r * c.a) / 255;
    unsigned char g = (c.g * c.a) / 255;
    unsigned char b = (c.b * c.a) / 255;
    return v4p_rgb_to_palette_index(r, g, b);
}

static void
nk_v4p_scissor(struct nk_v4p_context *v4p,
                 const short x,
                 const short y,
                 const unsigned short w,
                 const unsigned short h)
{
    v4p_trace(NUKLEAR, "Setting scissor region: (%d,%d) %d×%d\n", x, y, w, h);
    v4p->scissors.x = x;
    v4p->scissors.y = y;
    v4p->scissors.w = w;
    v4p->scissors.h = h;
}

static void
nk_v4p_stroke_line(const struct nk_v4p_context *v4p_ctx,
    V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1,
    const unsigned int line_thickness, v4p_color col)
{
    v4p_trace(NUKLEAR, "Drawing line from (%d,%d) to (%d,%d), thickness=%u, color=%d\n", x0, y0, x1, y1, line_thickness, col);
    if (line_thickness == 0) {
        // No line to draw
        return;
    }

    // Create a rectangle polygon representing the line
    V4pPolygonP line = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    v4p_trace(NUKLEAR, "Created line polygon %p at layer %d\n", (void*)line, v4p_ctx->current_layer);
    
    if (line_thickness == 1) { // 1px line
        // Use the stroke mode
        v4p_setStroke(line, 1);
        v4p_addJump(line);
        v4p_addPoint(line, x0, y0);
        v4p_addPoint(line, x0, y1);
    } else { // line with thickness > 1
        // v4p doesn't support thick line, so we create a rectangle polygon
        V4pCoord half_thick = line_thickness / 2;
        V4pCoord half_thick2 = half_thick + (line_thickness % 2); // Add 1 if thickness is odd to ensure proper coverage

        if (x0 == x1 || y0 == y1) { // Vertical or horizontal line or even a single point
            // draw a straight rectangle corresponding to the line and its thickness
            v4p_addCorners(line, x0 - half_thick, y0 - half_thick, x0 + half_thick2, y0 + half_thick2);
        } else { // Diagonal line
            // Calculate the rectangle vertices based on the line angle and thickness
            V4pCoord dx = x1 - x0;
            V4pCoord dy = y1 - y0;
            V4pCoord line_length = (V4pCoord)sqrt(dx*dx + dy*dy);
            
            // Calculate perpendicular offset
            V4pCoord px = -dy * half_thick / line_length;
            V4pCoord py = dx * half_thick / line_length;
            V4pCoord px2 = -dy * half_thick2 / line_length;
            V4pCoord py2 = dx * half_thick2 / line_length;

            v4p_addPoint(line, x0 + px2, y0 + py2);
            v4p_addPoint(line, x1 + px2, y1 + py2);
            v4p_addPoint(line, x1 - px, y1 - py);
            v4p_addPoint(line, x0 - px, y0 - py);
        }
    }

    // Apply scissor clipping
    if (! NK_SCISSOR_NULL(v4p_ctx->scissors)) {
        v4p_trace(NUKLEAR, "Applying scissor clipping to line: (%d,%d)-(%d,%d)\n", v4p_ctx->scissors.x,
                  v4p_ctx->scissors.y, v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                  v4p_ctx->scissors.y + v4p_ctx->scissors.h);
        v4p_clip(line, v4p_ctx->scissors.x, v4p_ctx->scissors.y, v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void
nk_v4p_fill_rect(const struct nk_v4p_context *v4p_ctx,
    const V4pCoord x, const V4pCoord y, const V4pCoord w, const V4pCoord h,
    const short r, v4p_color col)
{
    v4p_trace(NUKLEAR, "Filling rectangle: (%d,%d) %d×%d, rounding=%d, color=%d\n", x, y, w, h, r, col);
    
    // Use v4p's rectangle drawing
    // Create a rectangle polygon (4 points) and add to scene
    // Use the current layer for proper z-ordering
    V4pPolygonP rect = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    v4p_trace(NUKLEAR, "Created rectangle polygon %p at layer %d\n", (void*)rect, v4p_ctx->current_layer);
    if (r > 0) {
        // rounded rectangles
        v4p_addCutCorners(rect, x, y, x + w, y + h, r);
        v4p_setRound(rect, true);
    } else {
        v4p_addCorners(rect, x, y, x + w, y + h);
    }
    
    // Apply scissor clipping
    if (!NK_SCISSOR_NULL(v4p_ctx->scissors)) {
        v4p_trace(NUKLEAR, "Applying scissor clipping to rectangle: (%d,%d)-(%d,%d)\n",
                 v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
        v4p_clip(rect, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void nk_v4p_draw_rect(const struct nk_v4p_context* v4p_ctx, const V4pCoord x, const V4pCoord y, const V4pCoord w,
                               const V4pCoord h, const short r, const V4pCoord line_thickness, v4p_color col) {
    v4p_trace(NUKLEAR, "Drawing rectangle: (%d,%d) %d×%d, rounding=%d, line_thickness=%d, color=%d\n", x, y, w, h, r, line_thickness, col);

    if (line_thickness == 0) {
        // No line to draw
        return;
    }

    // Create a polygon and add to scene
    V4pPolygonP rect = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    v4p_trace(NUKLEAR, "Created hollow rectangle polygon %p at layer %d\n", (void*) rect, v4p_ctx->current_layer);

    if (line_thickness == 1) {  // 1px rectangle
        // Use the stroke mode
        v4p_setStroke(rect, 1);
        if (r > 0) { // a rounded rectangle
            v4p_addCutCorners(rect, x, y, x + w, y + h, r);
            v4p_setRound(rect, true);
        } else { // a regular rectangle
            v4p_addCorners(rect, x, y, x + w, y + h);
        }
    } else {  // rectangle with thickness > 1
        // v4p doesn't support closed polylines with thickness > 1px
        // we create two closed rectangle pathes, an external and an internal one, to form a hollow rectangle
        V4pCoord half_thick = line_thickness / 2;
        V4pCoord half_thick2 = half_thick + (line_thickness % 2);

        if (r > 0) { // 2 rounded rectangles
            v4p_addCutCorners(rect, x - half_thick, y - half_thick, x + w + half_thick, y + h + half_thick, r);
            v4p_addJump(rect); // Add a jump to separate the two rectangles
            v4p_addCutCorners(rect, x + half_thick2, y + half_thick2, x + w - half_thick2, y + h - half_thick2, r);
            v4p_setRound(rect, true);
        } else { // 2 regular rectangles
            v4p_addCorners(rect, x - half_thick, y - half_thick, x + w + half_thick, y + h + half_thick);
            v4p_addJump(rect);  // Add a jump to separate the two rectangles
            v4p_addCorners(rect, x + half_thick2, y + half_thick2, x + w - half_thick2, y + h - half_thick2);
        }

        // Apply scissor clipping
        if (! NK_SCISSOR_NULL(v4p_ctx->scissors)) {
            v4p_trace(NUKLEAR, "Applying scissor clipping to rectangle: (%d,%d)-(%d,%d)\n", v4p_ctx->scissors.x,
                      v4p_ctx->scissors.y, v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                      v4p_ctx->scissors.y + v4p_ctx->scissors.h);
            v4p_clip(rect, v4p_ctx->scissors.x, v4p_ctx->scissors.y, v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                     v4p_ctx->scissors.y + v4p_ctx->scissors.h);
        }
    }
}

static void
nk_v4p_fill_triangle(const struct nk_v4p_context *v4p_ctx,
    const V4pCoord x0, const V4pCoord y0, const V4pCoord x1, const V4pCoord y1,
    const V4pCoord x2, const V4pCoord y2, v4p_color col)
{
    v4p_trace(NUKLEAR, "Filling triangle: (%d,%d), (%d,%d), (%d,%d), color=%d\n", x0, y0, x1, y1, x2, y2, col);
    
    // Use v4p's triangle drawing
    // Use the current layer for proper z-ordering
    V4pPolygonP tri = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    v4p_trace(NUKLEAR, "Created triangle polygon %p at layer %d\n", (void*)tri, v4p_ctx->current_layer);
    
    v4p_addPoint(tri, x0, y0);
    v4p_addPoint(tri, x1, y1);
    v4p_addPoint(tri, x2, y2);
    
    // Apply scissor clipping
    if (!NK_SCISSOR_NULL(v4p_ctx->scissors)) {
        v4p_trace(NUKLEAR, "Applying scissor clipping to triangle: (%d,%d)-(%d,%d)\n",
                 v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
        v4p_clip(tri, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void
nk_v4p_fill_circle(const struct nk_v4p_context *v4p_ctx,
    V4pCoord x0, V4pCoord y0, V4pCoord w, V4pCoord h, v4p_color col)
{
    v4p_trace(NUKLEAR, "Filling circle: (%d,%d) %d×%d color=%d\n", x0, y0, w, h, col);
    
    // Use v4p's circle drawing
    V4pPolygonP circle = v4p_sceneAddNewDisk(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer,
                                             x0 + w / 2, y0 + w / 2, w / 2);
    v4p_trace(NUKLEAR, "Created circle polygon %p at layer %d with radius %d\n", (void*)circle, v4p_ctx->current_layer, w/2);
    
    // Apply scissor clipping
    if (!NK_SCISSOR_NULL(v4p_ctx->scissors)) {
        v4p_trace(NUKLEAR, "Applying scissor clipping to circle: (%d,%d)-(%d,%d)\n",
                 v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
        v4p_clip(circle, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void
nk_v4p_draw_text(const struct nk_v4p_context *v4p_ctx,
    const struct nk_user_font *font, short x, short y, short w, short h,
    const char *text, const int len, const float font_height,
    v4p_color color)
{
    v4p_trace(NUKLEAR, "Drawing text: '%.*s', font_height=%f, rect=(%d,%d) %d×%d\n", len, text, font_height, x, y, w, h);
    
    // Use v4p's qfont system for text rendering
    v4p_trace(NUKLEAR, "Text color converted to palette index %d\n", color);
    
    // Create a polygon for the text
    V4pPolygonP text_poly = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, color, v4p_ctx->current_layer);
    v4p_trace(NUKLEAR, "Created text polygon %p at layer %d\n", (void*)text_poly, v4p_ctx->current_layer);

    // Use qfont to define the text polygon
    // Scale font size appropriately based on the custom font height
    V4pCoord char_width = (V4pCoord)(font_height * 0.8f);  // Approximate character width (matches our width calculation)
    V4pCoord char_height = (V4pCoord)font_height;
    V4pCoord interleave = (V4pCoord)(font_height * 0.2f); // Spacing between characters (matches our width calculation)
    v4p_trace(NUKLEAR, "Font metrics: %d×%d interleave=%d\n", char_width, char_height, interleave);
    
    // Create a temporary string with the text (ensure null-terminated)
    char temp_text[256];
    int text_len = len > 255 ? 255 : len;
    strncpy(temp_text, text, text_len);
    temp_text[text_len] = '\0';
    v4p_trace(NUKLEAR, "Text to render: '%s'\n", temp_text);
    
    // Use qfont to define the polygon from the string
    qfontDefinePolygonFromString(temp_text,
                               text_poly,
                               (V4pCoord)x,
                               (V4pCoord)y,
                               char_width,
                               char_height,
                               interleave);
    
    // Apply scissor clipping
    if (!NK_SCISSOR_NULL(v4p_ctx->scissors)) {
        v4p_trace(NUKLEAR, "Applying scissor clipping to text: (%d,%d)-(%d,%d)\n",
                 v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
        v4p_clip(text_poly, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

// Custom font width calculation function using qfont
static float
nk_v4p_font_width(nk_handle handle, float height, const char *text, int len)
{
    (void)handle; // Unused parameter
    v4p_trace(NUKLEAR, "Calculating text width for '%.*s' (len=%d, height=%.1f)\n", len, text, len, height);
    
    // Use qfont metrics to calculate approximate width
    // qfont characters are 4x5 pixels, so we scale accordingly
    V4pCoord char_width = (V4pCoord)(height * 0.8f);  // Approximate character width
    V4pCoord interleave = (V4pCoord)(height * 0.2f); // Spacing between characters
    
    float total_width = 0.0f;
    for (int i = 0; i < len; i++) {
        total_width += char_width;
        if (i < len - 1) {
            total_width += interleave;
        }
    }
    
    v4p_trace(NUKLEAR, "Calculated text width: %.1f\n", total_width);
    return total_width;
}

NK_API int
nk_v4p_handle_event(struct nk_context *ctx, G4pEvent *evt)
{
    if (!ctx) return 0;
    int ctrl_down = g4p_state.buttons[G4P_CTRL];
    static int insert_toggle = 0;

    switch(evt->type)
    {
        case G4P_EVENT_KEY_UP: /* KEYUP & KEYDOWN share same routine */
        case G4P_EVENT_KEY_DOWN:
            {
                int down = evt->type == G4P_EVENT_KEY_DOWN;
                switch(evt->data.key.key)
                {
                    case G4P_KEY_LSHIFT: /* RSHIFT & LSHIFT share same routine */
                    case G4P_KEY_RSHIFT:    nk_input_key(ctx, NK_KEY_SHIFT, down); break;
                    case G4P_KEY_DELETE:    nk_input_key(ctx, NK_KEY_DEL, down); break;

                    case G4P_KEY_RETURN:    nk_input_key(ctx, NK_KEY_ENTER, down); break;

                    case G4P_KEY_TAB:       nk_input_key(ctx, NK_KEY_TAB, down); break;
                    case G4P_KEY_BACKSPACE: nk_input_key(ctx, NK_KEY_BACKSPACE, down); break;
                    case G4P_KEY_HOME:      nk_input_key(ctx, NK_KEY_TEXT_START, down);
                                         nk_input_key(ctx, NK_KEY_SCROLL_START, down); break;
                    case G4P_KEY_END:       nk_input_key(ctx, NK_KEY_TEXT_END, down);
                                         nk_input_key(ctx, NK_KEY_SCROLL_END, down); break;
                    case G4P_KEY_PAGEDOWN:  nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down); break;
                    case G4P_KEY_PAGEUP:    nk_input_key(ctx, NK_KEY_SCROLL_UP, down); break;
                    case 'z':         nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && ctrl_down); break;
                    case 'r':         nk_input_key(ctx, NK_KEY_TEXT_REDO, down && ctrl_down); break;
                    case 'c':         nk_input_key(ctx, NK_KEY_COPY, down && ctrl_down); break;
                    case 'v':         nk_input_key(ctx, NK_KEY_PASTE, down && ctrl_down); break;
                    case 'x':         nk_input_key(ctx, NK_KEY_CUT, down && ctrl_down); break;
                    case 'b':         nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && ctrl_down); break;
                    case 'e':         nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && ctrl_down); break;
                    case G4P_KEY_UP:        nk_input_key(ctx, NK_KEY_UP, down); break;
                    case G4P_KEY_DOWN:      nk_input_key(ctx, NK_KEY_DOWN, down); break;
                    case G4P_KEY_ESCAPE:    nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, down); break;
                    case G4P_KEY_INSERT:
                        if (down) insert_toggle = !insert_toggle;
                        if (insert_toggle) {
                            nk_input_key(ctx, NK_KEY_TEXT_INSERT_MODE, down);
                        } else {
                            nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, down);
                        }
                        break;
                    case 'a':
                        if (ctrl_down)
                            nk_input_key(ctx,NK_KEY_TEXT_SELECT_ALL, down);
                        break;
                    case G4P_KEY_LEFT:
                        if (ctrl_down)
                            nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
                        else nk_input_key(ctx, NK_KEY_LEFT, down);
                        break;
                    case G4P_KEY_RIGHT:
                        if (ctrl_down)
                            nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
                        else nk_input_key(ctx, NK_KEY_RIGHT, down);
                        break;
                }
            }
            return 1;

        case G4P_EVENT_MOUSE_UP: /* MOUSEBUTTONUP & MOUSEBUTTONDOWN share same routine */
        case G4P_EVENT_MOUSE_DOWN:
            {
                int down = evt->type == G4P_EVENT_MOUSE_DOWN;
                V4pCoord x_abs, y_abs;
                v4p_viewToAbsolute(evt->data.mouse.x, evt->data.mouse.y, &x_abs, &y_abs);
                const int x = (int)x_abs, y = (int)y_abs;
                switch(evt->data.mouse.button)
                {
                    case G4P_MOUSE_LEFT:
                        if (evt->data.mouse.clicks > 1)
                            nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
                        nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down); break;
                    case G4P_MOUSE_MIDDLE: nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down); break;
                    case G4P_MOUSE_RIGHT:  nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down); break;
                }
            }
            return 1;

        case G4P_EVENT_MOUSE_MOVE:
            {
                V4pCoord x_abs, y_abs;
                v4p_viewToAbsolute(evt->data.motion.x, evt->data.motion.y, &x_abs, &y_abs);
                if (ctx->input.mouse.grabbed) {
                    int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
                    nk_input_motion(ctx, x + (int)x_abs, y + (int)y_abs);
                }
                else nk_input_motion(ctx, (int)x_abs, (int)y_abs);
            }
            return 1;

        case G4P_EVENT_TEXT_INPUT:
            {
                nk_glyph glyph;
                memcpy(glyph, evt->data.text.text, NK_UTF_SIZE);
                nk_input_glyph(ctx, glyph);
            }
            return 1;

        case G4P_EVENT_MOUSE_WHEEL:
            nk_input_scroll(ctx,nk_vec2(evt->data.wheel.x, evt->data.wheel.y));
            return 1;
    }
    return 0;
}

NK_API struct nk_context*
nk_v4p_init(V4pSceneP scene, V4pCoord width, V4pCoord height)
{
    v4p_trace(NUKLEAR, "Initializing Nuklear v4p backend %dx%d\n", width, height);
    
    struct nk_v4p_context* v4p_ctx;

    v4p_ctx = (struct nk_v4p_context *)malloc(sizeof(struct nk_v4p_context));
    if (!v4p_ctx) {
        v4p_trace(NUKLEAR, "Failed to allocate Nuklear context\n");
        return NULL;
    }

    memset(v4p_ctx, 0, sizeof(struct nk_v4p_context));

    // v4p_setResolution doesn't exist, we'll use default resolution
    v4p_trace(NUKLEAR, "Setting up Nuklear v4p context\n");

    v4p_ctx->scene = scene;
    v4p_ctx->current_layer = 200;  // Start UI layers at 200 to ensure they render above most content
    v4p_trace(NUKLEAR, "Framebuffer: %dx%d\n", width, height);

    // Set up custom font using qfont system
    v4p_trace(NUKLEAR, "Setting up custom qfont-based font\n");
    v4p_ctx->font.userdata.ptr = NULL;  // No custom font data needed for qfont
    v4p_ctx->font.height = 16.0f;       // Default font height
    v4p_ctx->font.width = nk_v4p_font_width;
    
    if (0 == nk_init_default(&v4p_ctx->ctx, &v4p_ctx->font)) {
        v4p_trace(NUKLEAR, "Failed to initialize Nuklear default context\n");
        free(v4p_ctx);
        return NULL;
    }

    v4p_trace(NUKLEAR, "Setting custom font as default\n");
    nk_style_set_font(&v4p_ctx->ctx, &v4p_ctx->font);

    v4p_trace(NUKLEAR, "Nuklear v4p backend initialized successfully with custom font\n");
    return &v4p_ctx->ctx;
}

NK_API void
nk_v4p_render(struct nk_context *ctx)
{
    v4p_trace(NUKLEAR, "Starting Nuklear render pass\n");

    // Cast the generic nk_context to our custom context to access v4p-specific data
    struct nk_v4p_context* v4p = (struct nk_v4p_context*) ctx;

    // Reset layer for each frame
    v4p->current_layer = 200;

    // Reset scissor to null
    nk_v4p_scissor(v4p, nk_null_rect.x, nk_null_rect.y, nk_null_rect.w, nk_null_rect.h);

    const struct nk_command* cmd;
    nk_foreach(cmd, ctx) {
        // v4p_trace(NUKLEAR, "Processing command type %d\n", cmd->type);
        switch (cmd->type) {
        case NK_COMMAND_NOP: 
            v4p_trace(NUKLEAR, "Command: NOP\n");
            break;
        case NK_COMMAND_SCISSOR: {
            const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
            v4p_trace(NUKLEAR, "Command: SCISSOR (%d,%d) %d×%d\n", s->x, s->y, s->w, s->h);
            nk_v4p_scissor(v4p, s->x, s->y, s->w, s->h);
        } break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *l = (const struct nk_command_line *)cmd;
            v4p_trace(NUKLEAR, "Command: LINE from (%d,%d) to (%d,%d), thickness=%d\n", 
                     l->begin.x, l->begin.y, l->end.x, l->end.y, l->line_thickness);
            nk_v4p_stroke_line(v4p, l->begin.x, l->begin.y, l->end.x,
                l->end.y, l->line_thickness, nk_v4p_color2int(l->color));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
            v4p_trace(NUKLEAR, "Command: RECT (%d,%d) %d×%d, rounding=%d, thickness=%d\n", r->x, r->y, r->w, r->h, r->rounding, r->line_thickness);
            nk_v4p_draw_rect(v4p, r->x, r->y, r->w, r->h, (short) r->rounding, r->line_thickness, nk_v4p_color2int(r->color));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
            v4p_trace(NUKLEAR, "Command: RECT_FILLED (%d,%d) %d×%d, rounding=%d\n", 
                     r->x, r->y, r->w, r->h, r->rounding);
            nk_v4p_fill_rect(v4p, r->x, r->y, r->w, r->h,
                (short)r->rounding, nk_v4p_color2int(r->color));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
            v4p_trace(NUKLEAR, "Command: CIRCLE_FILLED (%d,%d) %d×%d\n", 
                     c->x, c->y, c->w, c->h);
            nk_v4p_fill_circle(v4p, c->x, c->y, c->w, c->h,
                                 nk_v4p_color2int(c->color));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
            v4p_trace(NUKLEAR, "Command: TRIANGLE_FILLED (%d,%d), (%d,%d), (%d,%d)\n",
                     t->a.x, t->a.y, t->b.x, t->b.y, t->c.x, t->c.y);
            nk_v4p_fill_triangle(v4p, t->a.x, t->a.y, t->b.x, t->b.y,
                t->c.x, t->c.y, nk_v4p_color2int(t->color));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;
            v4p_trace(NUKLEAR, "Command: TEXT '%.*s' at (%d,%d) %d×%d, height=%.1f\n",
                     t->length, t->string, t->x, t->y, t->w, t->h, t->height);
            
            // Check if text has background color with alpha > 0
            if (t->background.a > 0) {
                v4p_trace(NUKLEAR, "Drawing text background rect: (%d,%d) %d×%d\n", t->x, t->y, t->w, t->h);
                nk_v4p_fill_rect(v4p, t->x, t->y, t->w, t->h, 0, nk_v4p_color2int(t->background));
                v4p->current_layer++;
            }
            
            nk_v4p_draw_text(v4p, t->font, t->x, t->y, t->w, t->h, t->string, t->length, t->height,
                             nk_v4p_color2int(t->foreground));
            v4p->current_layer++;
        } break;
        default: 
            v4p_error("Nuklear Command: UNKNOWN type %d\n", cmd->type);
            break;
        }
    }

    v4p_trace(NUKLEAR, "Nuklear render pass completed, clearing context\n");
    nk_clear(ctx);
}

NK_API void
nk_v4p_shutdown(struct nk_context *ctx)
{
    v4p_trace(NUKLEAR, "Shutting down Nuklear v4p backend\n");
    if (ctx) {
        struct nk_v4p_context *v4p = (struct nk_v4p_context *)ctx;
        nk_free(ctx);
        free(v4p);
    }
}

#endif
