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
#include "v4p_color.h"
#include "lowmath.h"

// A display context
typedef struct v4pi_context_s {
    unsigned char* surface;  // Pointer to framebuffer memory
    int fd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    size_t screensize;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;  // Bytes per pixel
    unsigned int line_length;  // Bytes per line
} FbdevContext;

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
    palette.transp = NULL;

    if (! palette.red || ! palette.green || ! palette.blue) {
        v4p_error("Failed to allocate palette memory\n");
        exit(1);
    }

    for (int i = 0; i < 256; i++) {
        palette.red[i] = V4P_PALETTE_R(i);
        palette.green[i] = V4P_PALETTE_G(i);
        palette.blue[i] = V4P_PALETTE_B(i);
    }
}

static void cleanup_palette() {
    if (palette.red)
        free(palette.red);
    if (palette.green)
        free(palette.green);
    if (palette.blue)
        free(palette.blue);
}

// Default window/screen width & heigth
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640, V4P_DEFAULT_SCREEN_HEIGHT = 480;

// Global variable hosting the default V4P contex
FbdevContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
V4pCoord v4p_displayWidth;
V4pCoord v4p_displayHeight;
// private properties of current context
static unsigned char* currentBuffer;
static int iBuffer;

/**
 * Metrics stuff
 */
static UInt32 t1;
static UInt32 laps[4] = { 0, 0, 0, 0 };
static UInt32 tlaps = 0;

// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    t1 = 0;  // TODO: implement proper timing

    // Reset buffer pointer used by v4pDisplaySplice()
    iBuffer = 0;

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

    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;

    // Calculate position in framebuffer
    int location = x0 * (v4pi_context->bpp) + y * v4pi_context->line_length;

    // For 8-bit framebuffer, we can directly write the color
    if (v4pi_context->vinfo.bits_per_pixel == 8) {
        memset(&currentBuffer[location], c, l);
    } else {
        // Extract RGB values from our palette
        unsigned char red_val = palette.red[c];
        unsigned char green_val = palette.green[c];
        unsigned char blue_val = palette.blue[c];

        unsigned char* dest = &currentBuffer[location];
        for (int x = 0; x < l; x++) {
            *dest++ = blue_val;  // Blue component
            *dest++ = green_val;  // Green component
            *dest++ = red_val;  // Red component
            if (v4pi_context->bpp == 4)
                *dest++ = 0;  // Alpha
        }
    }

    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pi_init(int quality, Boolean fullscreen) {
    // Initialize palette
    init_palette();

    int console_fd = open("/dev/console", O_RDWR);
    int vt_num = 1;

    if (console_fd == -1) {
        v4p_error("Error opening /dev/console");
        return failure;
    }

    if (ioctl(console_fd, VT_ACTIVATE, vt_num) == -1) {
        v4p_error("Error switching virtual terminal");
        close(console_fd);
        return failure;
    }

    if (ioctl(console_fd, VT_WAITACTIVE, vt_num) == -1) {
        v4p_error("Error waiting for virtual terminal switch");
        close(console_fd);
        return failure;
    }

    // Set the console to graphics mode
    if (ioctl(console_fd, KDSETMODE, KD_GRAPHICS) == -1) {
        v4p_error("Error setting console to graphics mode");
        close(console_fd);
        return failure;
    }
    close(console_fd);

    // Open the framebuffer device
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        v4p_error("v4pi_init failed, cannot open framebuffer device\n");
        return failure;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &v4pi_defaultContextSingleton.vinfo)) {
        v4p_error("v4pi_init failed, cannot get variable screen info\n");
        close(fbfd);
        return failure;
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &v4pi_defaultContextSingleton.finfo)) {
        v4p_error("v4pi_init failed, cannot get fixed screen info\n");
        close(fbfd);
        return failure;
    }

    // Try to set 8-bit color mode if possible
    if (v4pi_defaultContextSingleton.vinfo.bits_per_pixel != 8) {
        struct fb_var_screeninfo original_vinfo = v4pi_defaultContextSingleton.vinfo;
        v4pi_defaultContextSingleton.vinfo.bits_per_pixel = 8;
        if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &v4pi_defaultContextSingleton.vinfo)) {
            // Can't set 8-bit mode, revert to original
            v4pi_defaultContextSingleton.vinfo = original_vinfo;
            v4p_error("Warning: Cannot set 8-bit color mode, using %d-bit\n",
                       v4pi_defaultContextSingleton.vinfo.bits_per_pixel);
        }
    }

    // Set up context properties
    v4pi_defaultContextSingleton.fd = fbfd;
    v4pi_defaultContextSingleton.width = v4pi_defaultContextSingleton.vinfo.xres;
    v4pi_defaultContextSingleton.height = v4pi_defaultContextSingleton.vinfo.yres;
    v4pi_defaultContextSingleton.bpp = v4pi_defaultContextSingleton.vinfo.bits_per_pixel / 8;
    v4pi_defaultContextSingleton.line_length = v4pi_defaultContextSingleton.finfo.line_length;
    v4pi_defaultContextSingleton.screensize
        = v4pi_defaultContextSingleton.height * v4pi_defaultContextSingleton.line_length;

    // Map the framebuffer to memory
    v4pi_defaultContextSingleton.surface = mmap(0,
                                             v4pi_defaultContextSingleton.screensize,
                                             PROT_READ | PROT_WRITE,
                                             MAP_SHARED,
                                             fbfd,
                                             0);
    if (v4pi_defaultContextSingleton.surface == MAP_FAILED) {
        v4p_error("v4pi_init failed, cannot map framebuffer\n");
        close(fbfd);
        return failure;
    }

    // Set palette if we're in 8-bit mode
    if (v4pi_defaultContextSingleton.vinfo.bits_per_pixel == 8) {
        if (ioctl(fbfd, FBIOPUTCMAP, &palette)) {
            v4p_error("Warning: Cannot set palette\n");
        }
    }

    // The default context holds the main screen/window
    v4pi_setContext(v4pi_defaultContext);

    return success;
}

// Create a new buffer-like V4P context
V4piContextP v4pi_newContext(int width, int height) {
    V4piContextP c = (V4piContextP) malloc(sizeof(FbdevContext));
    if (! c)
        return NULL;

    c->width = width;
    c->height = height;
    c->bpp = v4pi_defaultContext->bpp;
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
void v4pi_destroyContext(V4piContextP c) {
    if (! c || c == v4pi_defaultContext)
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
    if (v4pi_context == c)
        v4pi_context = v4pi_defaultContext;
}

// Change the current V4P context
V4piContextP v4pi_setContext(V4piContextP c) {
    v4pi_context = c;
    v4p_displayWidth = c->width;
    v4p_displayHeight = c->height;
    currentBuffer = c->surface;
    return c;
}

// clean things before quitting
void v4pi_destroy() {
    if (v4pi_defaultContextSingleton.surface) {
        munmap(v4pi_defaultContextSingleton.surface, v4pi_defaultContextSingleton.screensize);
    }
    if (v4pi_defaultContextSingleton.fd >= 0) {
        close(v4pi_defaultContextSingleton.fd);
    }

    // Reopen the console to switch back to text mode
    int console_fd = open("/dev/console", O_RDWR);
    if (console_fd == -1) {
        v4p_error("Error reopening /dev/console");
    } else {
        // Set the console back to text mode
        if (ioctl(console_fd, KDSETMODE, KD_TEXT) == -1) {
            v4p_error("Error setting console to text mode");
            close(console_fd);
        }
        close(console_fd);
    }

    cleanup_palette();
}