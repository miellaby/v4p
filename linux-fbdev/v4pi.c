/**
 * V4P Implementation for Linux Framebuffer
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/vt.h>
#include <linux/kd.h>

#include "v4pi.h"
#include "lowmath.h"

/**
 * A 256 color system palette inspired from old Palm Computing Devices.
 */
static struct fb_cmap palette;

static void init_palette() {
    palette.start = 0;
    palette.len = 256;
    palette.red = malloc(256 * sizeof(__u16));
    palette.green = malloc(256 * sizeof(__u16));
    palette.blue = malloc(256 * sizeof(__u16));
    palette.transp = malloc(256 * sizeof(__u16));

    if (! palette.red || ! palette.green || ! palette.blue || ! palette.transp) {
        fprintf(stderr, "Failed to allocate palette memory\n");
        exit(1);
    }

    // Initialize palette colors
    int colors[256][4]
        = { { 255, 255, 255, 0 }, { 255, 204, 255, 0 }, { 255, 153, 255, 0 }, { 255, 102, 255, 0 },
            { 255, 51, 255, 0 },  { 255, 0, 255, 0 },   { 255, 255, 204, 0 }, { 255, 204, 204, 0 },
            { 255, 153, 204, 0 }, { 255, 102, 204, 0 }, { 255, 51, 204, 0 },  { 255, 0, 204, 0 },
            { 255, 255, 153, 0 }, { 255, 204, 153, 0 }, { 255, 153, 153, 0 }, { 255, 102, 153, 0 },
            { 255, 51, 153, 0 },  { 255, 0, 153, 0 },   { 204, 255, 255, 0 }, { 204, 204, 255, 0 },
            { 204, 153, 255, 0 }, { 204, 102, 255, 0 }, { 204, 51, 255, 0 },  { 204, 0, 255, 0 },
            { 204, 255, 204, 0 }, { 204, 204, 204, 0 }, { 204, 153, 204, 0 }, { 204, 102, 204, 0 },
            { 204, 51, 204, 0 },  { 204, 0, 204, 0 },   { 204, 255, 153, 0 }, { 204, 204, 153, 0 },
            { 204, 153, 153, 0 }, { 204, 102, 153, 0 }, { 204, 51, 153, 0 },  { 204, 0, 153, 0 },
            { 153, 255, 255, 0 }, { 153, 204, 255, 0 }, { 153, 153, 255, 0 }, { 153, 102, 255, 0 },
            { 153, 51, 255, 0 },  { 153, 0, 255, 0 },   { 153, 255, 204, 0 }, { 153, 204, 204, 0 },
            { 153, 153, 204, 0 }, { 153, 102, 204, 0 }, { 153, 51, 204, 0 },  { 153, 0, 204, 0 },
            { 153, 255, 153, 0 }, { 153, 204, 153, 0 }, { 153, 153, 153, 0 }, { 153, 102, 153, 0 },
            { 153, 51, 153, 0 },  { 153, 0, 153, 0 },   { 102, 255, 255, 0 }, { 102, 204, 255, 0 },
            { 102, 153, 255, 0 }, { 102, 102, 255, 0 }, { 102, 51, 255, 0 },  { 102, 0, 255, 0 },
            { 102, 255, 204, 0 }, { 102, 204, 204, 0 }, { 102, 153, 204, 0 }, { 102, 102, 204, 0 },
            { 102, 51, 204, 0 },  { 102, 0, 204, 0 },   { 102, 255, 153, 0 }, { 102, 204, 153, 0 },
            { 102, 153, 153, 0 }, { 102, 102, 153, 0 }, { 102, 51, 153, 0 },  { 102, 0, 153, 0 },
            { 51, 255, 255, 0 },  { 51, 204, 255, 0 },  { 51, 153, 255, 0 },  { 51, 102, 255, 0 },
            { 51, 51, 255, 0 },   { 51, 0, 255, 0 },    { 51, 255, 204, 0 },  { 51, 204, 204, 0 },
            { 51, 153, 204, 0 },  { 51, 102, 204, 0 },  { 51, 51, 204, 0 },   { 51, 0, 204, 0 },
            { 51, 255, 153, 0 },  { 51, 204, 153, 0 },  { 51, 153, 153, 0 },  { 51, 102, 153, 0 },
            { 51, 51, 153, 0 },   { 51, 0, 153, 0 },    { 0, 255, 255, 0 },   { 0, 204, 255, 0 },
            { 0, 153, 255, 0 },   { 0, 102, 255, 0 },   { 0, 51, 255, 0 },    { 0, 0, 255, 0 },
            { 0, 255, 204, 0 },   { 0, 204, 204, 0 },   { 0, 153, 204, 0 },   { 0, 102, 204, 0 },
            { 0, 51, 204, 0 },    { 0, 0, 204, 0 },     { 0, 255, 153, 0 },   { 0, 204, 153, 0 },
            { 0, 153, 153, 0 },   { 0, 102, 153, 0 },   { 0, 51, 153, 0 },    { 0, 0, 153, 0 },
            { 255, 255, 102, 0 }, { 255, 204, 102, 0 }, { 255, 153, 102, 0 }, { 255, 102, 102, 0 },
            { 255, 51, 102, 0 },  { 255, 0, 102, 0 },   { 255, 255, 51, 0 },  { 255, 204, 51, 0 },
            { 255, 153, 51, 0 },  { 255, 102, 51, 0 },  { 255, 51, 51, 0 },   { 255, 0, 51, 0 },
            { 255, 255, 0, 0 },   { 255, 204, 0, 0 },   { 255, 153, 0, 0 },   { 255, 102, 0, 0 },
            { 255, 51, 0, 0 },    { 255, 0, 0, 0 },     { 204, 255, 102, 0 }, { 204, 204, 102, 0 },
            { 204, 153, 102, 0 }, { 204, 102, 102, 0 }, { 204, 51, 102, 0 },  { 204, 0, 102, 0 },
            { 204, 255, 51, 0 },  { 204, 204, 51, 0 },  { 204, 153, 51, 0 },  { 204, 102, 51, 0 },
            { 204, 51, 51, 0 },   { 204, 0, 51, 0 },    { 204, 255, 0, 0 },   { 204, 204, 0, 0 },
            { 204, 153, 0, 0 },   { 204, 102, 0, 0 },   { 204, 51, 0, 0 },    { 204, 0, 0, 0 },
            { 153, 255, 102, 0 }, { 153, 204, 102, 0 }, { 153, 153, 102, 0 }, { 153, 102, 102, 0 },
            { 153, 51, 102, 0 },  { 153, 0, 102, 0 },   { 153, 255, 51, 0 },  { 153, 204, 51, 0 },
            { 153, 153, 51, 0 },  { 153, 102, 51, 0 },  { 153, 51, 51, 0 },   { 153, 0, 51, 0 },
            { 153, 255, 0, 0 },   { 153, 204, 0, 0 },   { 153, 153, 0, 0 },   { 153, 102, 0, 0 },
            { 153, 51, 0, 0 },    { 153, 0, 0, 0 },     { 102, 255, 102, 0 }, { 102, 204, 102, 0 },
            { 102, 153, 102, 0 }, { 102, 102, 102, 0 }, { 102, 51, 102, 0 },  { 102, 0, 102, 0 },
            { 102, 255, 51, 0 },  { 102, 204, 51, 0 },  { 102, 153, 51, 0 },  { 102, 102, 51, 0 },
            { 102, 51, 51, 0 },   { 102, 0, 51, 0 },    { 102, 255, 0, 0 },   { 102, 204, 0, 0 },
            { 102, 153, 0, 0 },   { 102, 102, 0, 0 },   { 102, 51, 0, 0 },    { 102, 0, 0, 0 },
            { 51, 255, 102, 0 },  { 51, 204, 102, 0 },  { 51, 153, 102, 0 },  { 51, 102, 102, 0 },
            { 51, 51, 102, 0 },   { 51, 0, 102, 0 },    { 51, 255, 51, 0 },   { 51, 204, 51, 0 },
            { 51, 153, 51, 0 },   { 51, 102, 51, 0 },   { 51, 51, 51, 0 },    { 51, 0, 51, 0 },
            { 51, 255, 0, 0 },    { 51, 204, 0, 0 },    { 51, 153, 0, 0 },    { 51, 102, 0, 0 },
            { 51, 51, 0, 0 },     { 51, 0, 0, 0 },      { 0, 255, 102, 0 },   { 0, 204, 102, 0 },
            { 0, 153, 102, 0 },   { 0, 102, 102, 0 },   { 0, 51, 102, 0 },    { 0, 0, 102, 0 },
            { 0, 255, 51, 0 },    { 0, 204, 51, 0 },    { 0, 153, 51, 0 },    { 0, 102, 51, 0 },
            { 0, 51, 51, 0 },     { 0, 0, 51, 0 },      { 0, 255, 0, 0 },     { 0, 204, 0, 0 },
            { 0, 153, 0, 0 },     { 0, 102, 0, 0 },     { 0, 51, 0, 0 },      { 17, 17, 17, 0 },
            { 34, 34, 34, 0 },    { 68, 68, 68, 0 },    { 85, 85, 85, 0 },    { 119, 119, 119, 0 },
            { 136, 136, 136, 0 }, { 170, 170, 170, 0 }, { 187, 187, 187, 0 }, { 221, 221, 221, 0 },
            { 238, 238, 238, 0 }, { 192, 192, 192, 0 }, { 128, 0, 0, 0 },     { 128, 0, 128, 0 },
            { 0, 128, 0, 0 },     { 0, 128, 128, 0 },   { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 } };

    for (int i = 0; i < 256; i++) {
        palette.red[i] = (colors[i][0] << 8) | colors[i][0];
        palette.green[i] = (colors[i][1] << 8) | colors[i][1];
        palette.blue[i] = (colors[i][2] << 8) | colors[i][2];
        palette.transp[i] = (colors[i][3] << 8) | colors[i][3];
    }
}

static void cleanup_palette() {
    if (palette.red)
        free(palette.red);
    if (palette.green)
        free(palette.green);
    if (palette.blue)
        free(palette.blue);
    if (palette.transp)
        free(palette.transp);
}

// Some constants linking to basic colors;
const Color gray = 225, maroon = 226, purple = 227, green = 228, cyan = 229, black = 215, red = 125,
            blue = 95, yellow = 120, dark = 217, olive = 58, fluo = 48;

// Default window/screen width & heigth
const Coord defaultScreenWidth = 640, defaultScreenHeight = 480;

// A display context
typedef struct v4pDisplay_s {
    unsigned char* surface;  // Pointer to framebuffer memory
    int fd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    size_t screensize;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;  // Bytes per pixel
    unsigned int line_length;  // Bytes per line
} V4pDisplayS;

// Global variable hosting the default V4P contex
V4pDisplayS v4pDisplayDefaultContextS;
V4pDisplayP v4pDisplayDefaultContext = &v4pDisplayDefaultContextS;

// Variables hosting current context and related properties
V4pDisplayP v4pDisplayContext = &v4pDisplayDefaultContextS;
Coord v4pDisplayWidth;
Coord v4pDisplayHeight;
// private properties of current context
static unsigned char* currentBuffer;
static int iBuffer;

/**
 * Collide computing stuff
 */
typedef struct collide_s {
    Coord x;
    Coord y;
    UInt16 q;
    PolygonP poly;
} Collide;

Collide collides[16];

/**
 * Metrics stuff
 */
static UInt32 t1;
static UInt32 laps[4] = { 0, 0, 0, 0 };
static UInt32 tlaps = 0;

// debug logging helper
#ifdef DEBUG
void v4pi_debug(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    vprintf(formatString, args);
    va_end(args);
}
#endif

// error logging helper
Boolean v4pi_error(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    vfprintf(stderr, formatString, args);
    va_end(args);

    return success;
}

// record collides
Boolean v4pi_collide(ICollide i1,
                     ICollide i2,
                     Coord py,
                     Coord x1,
                     Coord x2,
                     PolygonP p1,
                     PolygonP p2) {
    int l, dx, dy;
    l = x2 - x1;
    dx = x1 * l + (l + 1) * l / 2;
    dy = l * py;
    collides[i1].q += l;
    collides[i1].x += dx;
    collides[i1].y += dy;
    collides[i1].poly = p2;
    collides[i2].q += l;
    collides[i2].x += dx;
    collides[i2].y += dy;
    collides[i2].poly = p1;
    return success;
}

// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    t1 = 0;  // TODO: implement proper timing

    // Reset buffer pointer used by v4pDisplaySplice()
    iBuffer = 0;

    // Init collides
    int i;
    for (i = 0; i < 16; i++) {
        collides[i].q = 0;
        collides[i].x = 0;
        collides[i].y = 0;
        collides[i].poly = NULL;
    }

    return success;
}

// finalize things after V4P engine scanline loop
Boolean v4pi_end() {
    int i;
    static int j = 0;

    // Get end time and compute average rendering time
    // TODO: implement proper timing
    UInt32 t2 = 0;
    tlaps -= laps[j % 4];
    tlaps += laps[j % 4] = t2 - t1;
    j++;
    if (! (j % 100))
        v4pi_debug("v4p_displayEnd, average time = %dms\n", tlaps / 4);

    // sumarize collides
    for (i = 0; i < 16; i++) {
        if (! collides[i].q)
            continue;
        collides[i].x /= collides[i].q;
        collides[i].y /= collides[i].q;
    }

    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(Coord y, Coord x0, Coord x1, Color c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;

    // Calculate position in framebuffer
    int location = x0 * (v4pDisplayContext->bpp) + y * v4pDisplayContext->line_length;

    // For 8-bit framebuffer, we can directly write the color
    if (v4pDisplayContext->vinfo.bits_per_pixel == 8) {
        memset(&currentBuffer[location], c, l);
    } else {
        // For higher bit depths, convert the 8-bit palette index to RGB
        // Extract RGB values from our palette
        unsigned char red_val = (palette.red[c] >> 8) & 0xFF;
        unsigned char green_val = (palette.green[c] >> 8) & 0xFF;
        unsigned char blue_val = (palette.blue[c] >> 8) & 0xFF;

        unsigned char* dest = &currentBuffer[location];
        for (int x = 0; x < l; x++) {
            *dest++ = blue_val;  // Blue component
            *dest++ = green_val;  // Green component
            *dest++ = red_val;  // Red component
            if (v4pDisplayContext->bpp == 4)
                *dest++ = 0;  // Alpha
        }
    }

    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pDisplayInit(int quality, Boolean fullscreen) {
    // Initialize palette
    init_palette();

    int console_fd = open("/dev/console", O_RDWR);
    int vt_num = 1;

    if (console_fd == -1) {
        v4pi_error("Error opening /dev/console");
        return failure;
    }

    if (ioctl(console_fd, VT_ACTIVATE, vt_num) == -1) {
        v4pi_error("Error switching virtual terminal");
        close(console_fd);
        return failure;
    }

    if (ioctl(console_fd, VT_WAITACTIVE, vt_num) == -1) {
        v4pi_error("Error waiting for virtual terminal switch");
        close(console_fd);
        return failure;
    }

    // Set the console to graphics mode
    if (ioctl(console_fd, KDSETMODE, KD_GRAPHICS) == -1) {
        v4pi_error("Error setting console to graphics mode");
        close(console_fd);
        return failure;
    }
    close(console_fd);

    // Open the framebuffer device
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        v4pi_error("v4pDisplayInit failed, cannot open framebuffer device\n");
        return failure;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &v4pDisplayDefaultContextS.vinfo)) {
        v4pi_error("v4pDisplayInit failed, cannot get variable screen info\n");
        close(fbfd);
        return failure;
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &v4pDisplayDefaultContextS.finfo)) {
        v4pi_error("v4pDisplayInit failed, cannot get fixed screen info\n");
        close(fbfd);
        return failure;
    }

    // Try to set 8-bit color mode if possible
    if (v4pDisplayDefaultContextS.vinfo.bits_per_pixel != 8) {
        struct fb_var_screeninfo original_vinfo = v4pDisplayDefaultContextS.vinfo;
        v4pDisplayDefaultContextS.vinfo.bits_per_pixel = 8;
        if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &v4pDisplayDefaultContextS.vinfo)) {
            // Can't set 8-bit mode, revert to original
            v4pDisplayDefaultContextS.vinfo = original_vinfo;
            v4pi_error("Warning: Cannot set 8-bit color mode, using %d-bit\n",
                       v4pDisplayDefaultContextS.vinfo.bits_per_pixel);
        }
    }

    // Set up context properties
    v4pDisplayDefaultContextS.fd = fbfd;
    v4pDisplayDefaultContextS.width = v4pDisplayDefaultContextS.vinfo.xres;
    v4pDisplayDefaultContextS.height = v4pDisplayDefaultContextS.vinfo.yres;
    v4pDisplayDefaultContextS.bpp = v4pDisplayDefaultContextS.vinfo.bits_per_pixel / 8;
    v4pDisplayDefaultContextS.line_length = v4pDisplayDefaultContextS.finfo.line_length;
    v4pDisplayDefaultContextS.screensize
        = v4pDisplayDefaultContextS.height * v4pDisplayDefaultContextS.line_length;

    // Map the framebuffer to memory
    v4pDisplayDefaultContextS.surface = mmap(0,
                                             v4pDisplayDefaultContextS.screensize,
                                             PROT_READ | PROT_WRITE,
                                             MAP_SHARED,
                                             fbfd,
                                             0);
    if (v4pDisplayDefaultContextS.surface == MAP_FAILED) {
        v4pi_error("v4pDisplayInit failed, cannot map framebuffer\n");
        close(fbfd);
        return failure;
    }

    // Set palette if we're in 8-bit mode
    if (v4pDisplayDefaultContextS.vinfo.bits_per_pixel == 8) {
        if (ioctl(fbfd, FBIOPUTCMAP, &palette)) {
            v4pi_error("Warning: Cannot set palette\n");
        }
    }

    // The default context holds the main screen/window
    v4pi_setContext(v4pDisplayDefaultContext);

    return success;
}

// Create a new buffer-like V4P context
V4pDisplayP v4pDisplayNewContext(int width, int height) {
    V4pDisplayP c = (V4pDisplayP) malloc(sizeof(V4pDisplayS));
    if (! c)
        return NULL;

    c->width = width;
    c->height = height;
    c->bpp = v4pDisplayDefaultContext->bpp;
    c->line_length = width * c->bpp;
    c->screensize = height * c->line_length;
    c->surface = malloc(c->screensize);
    c->fd = -1;  // Not a real framebuffer

    if (! c->surface) {
        free(c);
        return NULL;
    }

    // Clear the buffer
    memset(c->surface, 0, c->screensize);

    return c;
}

// free a V4P context
void v4pDisplayFreeContext(V4pDisplayP c) {
    if (! c || c == v4pDisplayDefaultContext)
        return;

    if (c->fd >= 0) {
        // It's a real framebuffer context
        if (c->surface)
            munmap(c->surface, c->screensize);
        if (c->fd >= 0)
            close(c->fd);
    } else {
        // It's a memory buffer context
        free(c->surface);
    }
    free(c);

    // One can't let a pointer to a freed context.
    if (v4pDisplayContext == c)
        v4pDisplayContext = v4pDisplayDefaultContext;
}

// Change the current V4P context
V4pDisplayP v4pi_setContext(V4pDisplayP c) {
    v4pDisplayContext = c;
    v4pDisplayWidth = c->width;
    v4pDisplayHeight = c->height;
    currentBuffer = c->surface;
    return c;
}

// clean things before quitting
void v4pDisplayQuit() {
    if (v4pDisplayDefaultContextS.surface) {
        munmap(v4pDisplayDefaultContextS.surface, v4pDisplayDefaultContextS.screensize);
    }
    if (v4pDisplayDefaultContextS.fd >= 0) {
        close(v4pDisplayDefaultContextS.fd);
    }

    // Reopen the console to switch back to text mode
    int console_fd = open("/dev/console", O_RDWR);
    if (console_fd == -1) {
        v4pi_error("Error reopening /dev/console");
    } else {
        // Set the console back to text mode
        if (ioctl(console_fd, KDSETMODE, KD_TEXT) == -1) {
            v4pi_error("Error setting console to text mode");
            close(console_fd);
        }
        close(console_fd);
    }

    cleanup_palette();
}