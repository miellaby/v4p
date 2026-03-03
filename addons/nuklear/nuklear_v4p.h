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
#include "addons/qfont/qfont.h"

struct nk_context;

struct v4p_pl {
    unsigned char bytesPerPixel;
    unsigned char rshift, gshift, bshift, ashift;
    unsigned char rloss, gloss, bloss, aloss;
};

/* All functions are thread-safe */
NK_API struct nk_context *nk_v4p_init(V4pSceneP scene, V4pCoord width, V4pCoord height, const struct v4p_pl pl);
NK_API void                  nk_v4p_render(struct nk_context *ctx);
NK_API void                  nk_v4p_shutdown(struct nk_context *ctx);
NK_API void                  nk_v4p_resize(struct nk_context *ctx, V4pCoord width, V4pCoord height, const struct v4p_pl pl);

#endif
/*
 * ==============================================================
 *                          IMPLEMENTATION
 * ===============================================================
 */
#ifdef NK_V4P_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

struct v4p_image {
    V4pPolygonP polygon;
    int w, h;
    struct v4p_pl pl;
};

struct nk_v4p_context {
    struct nk_context ctx; // must be first for proper casting to nk_context*
    V4pSceneP scene;  // Reference to a v4p scene
    struct nk_rect scissors;
    struct v4p_image fb;
    struct v4p_image font_tex;
    struct nk_font_atlas atlas;
    V4pLayer current_layer;  // Track current layer for z-ordering
};

typedef V4pColor v4p_color;

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

static v4p_color
nk_v4p_color2int(const struct nk_color c, const struct v4p_pl *pl)
{
    // Convert Nuklear color (0-255 RGBA) to v4p palette index
    // v4p uses a 256-color palette, so we need to find the closest match
    // Ignore alpha channel as v4p doesn't support transparency in palette
    return v4p_rgb_to_palette_index(c.r, c.g, c.b);
}





static void
nk_v4p_scissor(struct nk_v4p_context *v4p,
                 const float x,
                 const float y,
                 const float w,
                 const float h)
{
    v4p->scissors.x = (V4pCoord)x;
    v4p->scissors.y = (V4pCoord)y;
    v4p->scissors.w = (V4pCoord)w;
    v4p->scissors.h = (V4pCoord)h;
}

static void
nk_v4p_stroke_line(const struct nk_v4p_context *v4p_ctx,
    V4pCoord x0, V4pCoord y0, V4pCoord x1, V4pCoord y1,
    const unsigned int line_thickness, v4p_color col)
{
    // v4p doesn't support 1px lines, so we create a thin rectangle polygon
    // For simplicity, we'll create a rectangle that approximates the line
    // This is a simplified approach using axis-aligned rectangles
    
    V4pCoord thickness = line_thickness > 0 ? line_thickness : 1;
    
    // Create a rectangle polygon representing the line
    V4pPolygonP line = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    
    // Apply scissor clipping
    if (v4p_ctx->scissors.w > 0 && v4p_ctx->scissors.h > 0) {
        v4p_clip(line, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
    
    // For horizontal/vertical lines, this is straightforward
    if (x0 == x1) {
        // Vertical line
        V4pCoord half_thick = thickness / 2;
        v4p_addPoint(line, x0 - half_thick, y0);
        v4p_addPoint(line, x0 + half_thick, y0);
        v4p_addPoint(line, x0 + half_thick, y1);
        v4p_addPoint(line, x0 - half_thick, y1);
    } else if (y0 == y1) {
        // Horizontal line
        V4pCoord half_thick = thickness / 2;
        v4p_addPoint(line, x0, y0 - half_thick);
        v4p_addPoint(line, x1, y0 - half_thick);
        v4p_addPoint(line, x1, y0 + half_thick);
        v4p_addPoint(line, x0, y0 + half_thick);
    } else {
        // Diagonal line - use a simple approximation with a rectangle
        // This creates a rectangle from (x0,y0) to (x1,y1) with given thickness
        V4pCoord dx = x1 - x0;
        V4pCoord dy = y1 - y0;
        V4pCoord line_length = (V4pCoord)sqrt(dx*dx + dy*dy);
        
        if (line_length == 0) {
            // Single point - draw a small square
            V4pCoord half_thick = thickness / 2;
            v4p_addPoint(line, x0 - half_thick, y0 - half_thick);
            v4p_addPoint(line, x0 + half_thick, y0 - half_thick);
            v4p_addPoint(line, x0 + half_thick, y0 + half_thick);
            v4p_addPoint(line, x0 - half_thick, y0 + half_thick);
        } else {
            // For diagonal lines, we'll use a simpler approach
            // Create a rectangle that covers the line area
            V4pCoord half_thick = thickness / 2;
            
            // Calculate perpendicular offset
            V4pCoord px = -dy * half_thick / line_length;
            V4pCoord py = dx * half_thick / line_length;
            
            v4p_addPoint(line, x0 + px, y0 + py);
            v4p_addPoint(line, x1 + px, y1 + py);
            v4p_addPoint(line, x1 - px, y1 - py);
            v4p_addPoint(line, x0 - px, y0 - py);
        }
    }
}

static void
nk_v4p_fill_rect(const struct nk_v4p_context *v4p_ctx,
    const V4pCoord x, const V4pCoord y, const V4pCoord w, const V4pCoord h,
    const short r, v4p_color col)
{
    // Use v4p's rectangle drawing
    // Create a rectangle polygon (4 points) and add to scene
    // Use the current layer for proper z-ordering
    V4pPolygonP rect = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    v4p_addPoint(rect, x, y);
    v4p_addPoint(rect, x + w, y);
    v4p_addPoint(rect, x + w, y + h);
    v4p_addPoint(rect, x, y + h);
    
    // Apply scissor clipping
    if (v4p_ctx->scissors.w > 0 && v4p_ctx->scissors.h > 0) {
        v4p_clip(rect, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void
nk_v4p_fill_triangle(const struct nk_v4p_context *v4p_ctx,
    const V4pCoord x0, const V4pCoord y0, const V4pCoord x1, const V4pCoord y1,
    const V4pCoord x2, const V4pCoord y2, v4p_color col)
{
    // Use v4p's triangle drawing
    // Use the current layer for proper z-ordering
    V4pPolygonP tri = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer);
    v4p_addPoint(tri, x0, y0);
    v4p_addPoint(tri, x1, y1);
    v4p_addPoint(tri, x2, y2);
    
    // Apply scissor clipping
    if (v4p_ctx->scissors.w > 0 && v4p_ctx->scissors.h > 0) {
        v4p_clip(tri, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void
nk_v4p_fill_circle(const struct nk_v4p_context *v4p_ctx,
    V4pCoord x0, V4pCoord y0, V4pCoord w, V4pCoord h, v4p_color col)
{
    // Use v4p's circle drawing
    V4pPolygonP circle = v4p_sceneAddNewDisk(v4p_ctx->scene, V4P_ABSOLUTE, col, v4p_ctx->current_layer, x0, y0, w/2);
    
    // Apply scissor clipping
    if (v4p_ctx->scissors.w > 0 && v4p_ctx->scissors.h > 0) {
        v4p_clip(circle, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

static void
nk_v4p_draw_text(const struct nk_v4p_context *v4p_ctx,
    const struct nk_user_font *font, const struct nk_rect rect,
    const char *text, const int len, const float font_height,
    const struct nk_color fg)
{
    // Use v4p's qfont system for text rendering
    // Convert Nuklear color to v4p palette index
    v4p_color color = nk_v4p_color2int(fg, &v4p_ctx->fb.pl);
    
    // Create a polygon for the text
    V4pPolygonP text_poly = v4p_sceneAddNewPoly(v4p_ctx->scene, V4P_ABSOLUTE, color, v4p_ctx->current_layer);

    // Use qfont to define the text polygon
    // Scale font size appropriately
    V4pCoord char_width = (V4pCoord)(font_height * 0.6f);  // Approximate character width
    V4pCoord char_height = (V4pCoord)font_height;
    V4pCoord interleave = (V4pCoord)(font_height * 0.2f); // Spacing between characters
    
    // Create a temporary string with the text (ensure null-terminated)
    char temp_text[256];
    int text_len = len > 255 ? 255 : len;
    strncpy(temp_text, text, text_len);
    temp_text[text_len] = '\0';
    
    // Use qfont to define the polygon from the string
    qfontDefinePolygonFromString(temp_text,
                               text_poly,
                               (V4pCoord)rect.x,
                               (V4pCoord)rect.y,
                               char_width,
                               char_height,
                               interleave);
    
    // Apply scissor clipping
    if (v4p_ctx->scissors.w > 0 && v4p_ctx->scissors.h > 0) {
        v4p_clip(text_poly, v4p_ctx->scissors.x, v4p_ctx->scissors.y,
                 v4p_ctx->scissors.x + v4p_ctx->scissors.w,
                 v4p_ctx->scissors.y + v4p_ctx->scissors.h);
    }
}

NK_API struct nk_context*
nk_v4p_init(V4pSceneP scene, V4pCoord width, V4pCoord height, const struct v4p_pl pl)
{
    struct nk_v4p_context* v4p_ctx;
    const void *tex;

    v4p_ctx = (struct nk_v4p_context *)malloc(sizeof(struct nk_v4p_context));
    if (!v4p_ctx)
        return NULL;

    memset(v4p_ctx, 0, sizeof(struct nk_v4p_context));

    // v4p_setResolution doesn't exist, we'll use default resolution

    // Set up font texture
    v4p_ctx->scene = scene;
    v4p_ctx->font_tex.polygon = NULL;
    v4p_ctx->font_tex.pl = pl;
    v4p_ctx->font_tex.w = v4p_ctx->font_tex.h = 0;

    v4p_ctx->fb.w = width;
    v4p_ctx->fb.h = height;
    v4p_ctx->fb.pl = pl;
    v4p_ctx->current_layer = 200;  // Start UI layers at 200 to ensure they render above most content

    if (0 == nk_init_default(&v4p_ctx->ctx, 0)) {
        free(v4p_ctx);
        return NULL;
    }

    nk_font_atlas_init_default(&v4p_ctx->atlas);
    nk_font_atlas_begin(&v4p_ctx->atlas);
    tex = nk_font_atlas_bake(&v4p_ctx->atlas, &v4p_ctx->font_tex.w, &v4p_ctx->font_tex.h, NK_FONT_ATLAS_ALPHA8);
    if (!tex) {
        free(v4p_ctx);
        return NULL;
    }

    // v4p->font_tex.pitch = v4p->font_tex.w * 1;
    // Note: In a full implementation, we would create a texture from this data
    nk_font_atlas_end(&v4p_ctx->atlas, nk_handle_ptr(NULL), NULL);
    if (v4p_ctx->atlas.default_font)
        nk_style_set_font(&v4p_ctx->ctx, &v4p_ctx->atlas.default_font->handle);
    nk_style_load_all_cursors(&v4p_ctx->ctx, v4p_ctx->atlas.cursors);
    nk_v4p_scissor(v4p_ctx, 0, 0, v4p_ctx->fb.w, v4p_ctx->fb.h);

    return &v4p_ctx->ctx;
}

NK_API void
nk_v4p_render(struct nk_context *ctx)
{
    struct nk_v4p_context *v4p = (struct nk_v4p_context *)ctx;
    const struct v4p_pl *pl = &v4p->fb.pl;
    const struct nk_command *cmd;

    // Reset layer for each frame
    v4p->current_layer = 200;

    nk_foreach(cmd, ctx) {
        switch (cmd->type) {
        case NK_COMMAND_NOP: break;
        case NK_COMMAND_SCISSOR: {
            const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
            nk_v4p_scissor(v4p, s->x, s->y, s->w, s->h);
        } break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *l = (const struct nk_command_line *)cmd;
            nk_v4p_stroke_line(v4p, l->begin.x, l->begin.y, l->end.x,
                l->end.y, l->line_thickness, nk_v4p_color2int(l->color, pl));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
            // For now, just draw filled rect (simplified)
            nk_v4p_fill_rect(v4p, r->x, r->y, r->w, r->h,
                0, nk_v4p_color2int(r->color, pl));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
            nk_v4p_fill_rect(v4p, r->x, r->y, r->w, r->h,
                (short)r->rounding, nk_v4p_color2int(r->color, pl));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
            nk_v4p_fill_circle(v4p, c->x, c->y, c->w, c->h,
                                 nk_v4p_color2int(c->color, pl));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
            nk_v4p_fill_triangle(v4p, t->a.x, t->a.y, t->b.x, t->b.y,
                t->c.x, t->c.y, nk_v4p_color2int(t->color, pl));
            v4p->current_layer++;
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;
            nk_v4p_draw_text(v4p, t->font, nk_rect(t->x, t->y, t->w, t->h),
                t->string, t->length, t->height, t->foreground);
            v4p->current_layer++;
        } break;
        default: break;
        }
    } 
    
    nk_clear(ctx);
}

NK_API void
nk_v4p_shutdown(struct nk_context *ctx)
{
    if (ctx) {
        struct nk_v4p_context *v4p = (struct nk_v4p_context *)ctx;
        nk_free(ctx);
        free(v4p);
    }
}

NK_API void
nk_v4p_resize(struct nk_context *ctx, V4pCoord width, V4pCoord height, const struct v4p_pl pl)
{
    struct nk_v4p_context *v4p = (struct nk_v4p_context *)ctx;
    v4p->fb.w = width;
    v4p->fb.h = height;
    v4p->fb.pl = pl;
    // v4p_setResolution doesn't exist, resolution is handled by the backend
}

#endif
