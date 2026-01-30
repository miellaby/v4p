/**
 * V4P Implementation for Linux DRM (pure DRM, minimal, no framebuffer objects)
 *
 * This is a minimal DRM-only backend that uses:
 * - libdrm for mode setting and display control
 * - DRM dumb buffers for framebuffer memory
 * - Direct memory mapping for rendering
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "_v4pi.h"
#include "v4pi.h"
#include "lowmath.h"

// Global display context
static DRMContext drm_context = { 0 };
V4pDisplayP v4pDisplayDefaultContext = NULL;
V4pDisplayP v4pDisplayContext = NULL;

// Display dimensions
Coord v4pDisplayWidth = 0;
Coord v4pDisplayHeight = 0;

// A 256 color system palette inspired from old Palm Computing Devices
static struct s_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} palette[256] = {
    { 255, 255, 255, 255 }, { 255, 204, 255, 255 }, { 255, 153, 255, 255 }, { 255, 102, 255, 255 },
    { 255, 51, 255, 255 },  { 255, 0, 255, 255 },   { 255, 255, 204, 255 }, { 255, 204, 204, 255 },
    { 255, 153, 204, 255 }, { 255, 102, 204, 255 }, { 255, 51, 204, 255 },  { 255, 0, 204, 255 },
    { 255, 255, 153, 255 }, { 255, 204, 153, 255 }, { 255, 153, 153, 255 }, { 255, 102, 153, 255 },
    { 255, 51, 153, 255 },  { 255, 0, 153, 255 },   { 204, 255, 255, 255 }, { 204, 204, 255, 255 },
    { 204, 153, 255, 255 }, { 204, 102, 255, 255 }, { 204, 51, 255, 255 },  { 204, 0, 255, 255 },
    { 204, 255, 204, 255 }, { 204, 204, 204, 255 }, { 204, 153, 204, 255 }, { 204, 102, 204, 255 },
    { 204, 51, 204, 255 },  { 204, 0, 204, 255 },   { 204, 255, 153, 255 }, { 204, 204, 153, 255 },
    { 204, 153, 153, 255 }, { 204, 102, 153, 255 }, { 204, 51, 153, 255 },  { 204, 0, 153, 255 },
    { 153, 255, 255, 255 }, { 153, 204, 255, 255 }, { 153, 153, 255, 255 }, { 153, 102, 255, 255 },
    { 153, 51, 255, 255 },  { 153, 0, 255, 255 },   { 153, 255, 204, 255 }, { 153, 204, 204, 255 },
    { 153, 153, 204, 255 }, { 153, 102, 204, 255 }, { 153, 51, 204, 255 },  { 153, 0, 204, 255 },
    { 153, 255, 153, 255 }, { 153, 204, 153, 255 }, { 153, 153, 153, 255 }, { 153, 102, 153, 255 },
    { 153, 51, 153, 255 },  { 153, 0, 153, 255 },   { 102, 255, 255, 255 }, { 102, 204, 255, 255 },
    { 102, 153, 255, 255 }, { 102, 102, 255, 255 }, { 102, 51, 255, 255 },  { 102, 0, 255, 255 },
    { 102, 255, 204, 255 }, { 102, 204, 204, 255 }, { 102, 153, 204, 255 }, { 102, 102, 204, 255 },
    { 102, 51, 204, 255 },  { 102, 0, 204, 255 },   { 102, 255, 153, 255 }, { 102, 204, 153, 255 },
    { 102, 153, 153, 255 }, { 102, 102, 153, 255 }, { 102, 51, 153, 255 },  { 102, 0, 153, 255 },
    { 51, 255, 255, 255 },  { 51, 204, 255, 255 },  { 51, 153, 255, 255 },  { 51, 102, 255, 255 },
    { 51, 51, 255, 255 },   { 51, 0, 255, 255 },    { 51, 255, 204, 255 },  { 51, 204, 204, 255 },
    { 51, 153, 204, 255 },  { 51, 102, 204, 255 },  { 51, 51, 204, 255 },   { 51, 0, 204, 255 },
    { 51, 255, 153, 255 },  { 51, 204, 153, 255 },  { 51, 153, 153, 255 },  { 51, 102, 153, 255 },
    { 51, 51, 153, 255 },   { 51, 0, 153, 255 },    { 0, 255, 255, 255 },   { 0, 204, 255, 255 },
    { 0, 153, 255, 255 },   { 0, 102, 255, 255 },   { 0, 51, 255, 255 },    { 0, 0, 255, 255 },
    { 0, 255, 204, 255 },   { 0, 204, 204, 255 },   { 0, 153, 204, 255 },   { 0, 102, 204, 255 },
    { 0, 51, 204, 255 },    { 0, 0, 204, 255 },     { 0, 255, 153, 255 },   { 0, 204, 153, 255 },
    { 0, 153, 153, 255 },   { 0, 102, 153, 255 },   { 0, 51, 153, 255 },    { 0, 0, 153, 255 },
    { 255, 255, 102, 255 }, { 255, 204, 102, 255 }, { 255, 153, 102, 255 }, { 255, 102, 102, 255 },
    { 255, 51, 102, 255 },  { 255, 0, 102, 255 },   { 255, 255, 51, 255 },  { 255, 204, 51, 255 },
    { 255, 153, 51, 255 },  { 255, 102, 51, 255 },  { 255, 51, 51, 255 },   { 255, 0, 51, 255 },
    { 255, 255, 0, 255 },   { 255, 204, 0, 255 },   { 255, 153, 0, 255 },   { 255, 102, 0, 255 },
    { 255, 51, 0, 255 },    { 255, 0, 0, 255 },     { 204, 255, 102, 255 }, { 204, 204, 102, 255 },
    { 204, 153, 102, 255 }, { 204, 102, 102, 255 }, { 204, 51, 102, 255 },  { 204, 0, 102, 255 },
    { 204, 255, 51, 255 },  { 204, 204, 51, 255 },  { 204, 153, 51, 255 },  { 204, 102, 51, 255 },
    { 204, 51, 51, 255 },   { 204, 0, 51, 255 },    { 204, 255, 0, 255 },   { 204, 204, 0, 255 },
    { 204, 153, 0, 255 },   { 204, 102, 0, 255 },   { 204, 51, 0, 255 },    { 204, 0, 0, 255 },
    { 153, 255, 102, 255 }, { 153, 204, 102, 255 }, { 153, 153, 102, 255 }, { 153, 102, 102, 255 },
    { 153, 51, 102, 255 },  { 153, 0, 102, 255 },   { 153, 255, 51, 255 },  { 153, 204, 51, 255 },
    { 153, 153, 51, 255 },  { 153, 102, 51, 255 },  { 153, 51, 51, 255 },   { 153, 0, 51, 255 },
    { 153, 255, 0, 255 },   { 153, 204, 0, 255 },   { 153, 153, 0, 255 },   { 153, 102, 0, 255 },
    { 153, 51, 0, 255 },    { 153, 0, 0, 255 },     { 102, 255, 102, 255 }, { 102, 204, 102, 255 },
    { 102, 153, 102, 255 }, { 102, 102, 102, 255 }, { 102, 51, 102, 255 },  { 102, 0, 102, 255 },
    { 102, 255, 51, 255 },  { 102, 204, 51, 255 },  { 102, 153, 51, 255 },  { 102, 102, 51, 255 },
    { 102, 51, 51, 255 },   { 102, 0, 51, 255 },    { 102, 255, 0, 255 },   { 102, 204, 0, 255 },
    { 102, 153, 0, 255 },   { 102, 102, 0, 255 },   { 102, 51, 0, 255 },    { 102, 0, 0, 255 },
    { 51, 255, 102, 255 },  { 51, 204, 102, 255 },  { 51, 153, 102, 255 },  { 51, 102, 102, 255 },
    { 51, 51, 102, 255 },   { 51, 0, 102, 255 },    { 51, 255, 51, 255 },   { 51, 204, 51, 255 },
    { 51, 153, 51, 255 },   { 51, 102, 51, 255 },   { 51, 51, 51, 255 },    { 51, 0, 51, 255 },
    { 51, 255, 0, 255 },    { 51, 204, 0, 255 },    { 51, 153, 0, 255 },    { 51, 102, 0, 255 },
    { 51, 51, 0, 255 },     { 51, 0, 0, 255 },      { 0, 255, 102, 255 },   { 0, 204, 102, 255 },
    { 0, 153, 102, 255 },   { 0, 102, 102, 255 },   { 0, 51, 102, 255 },    { 0, 0, 102, 255 },
    { 0, 255, 51, 255 },    { 0, 204, 51, 255 },    { 0, 153, 51, 255 },    { 0, 102, 51, 255 },
    { 0, 51, 51, 255 },     { 0, 0, 51, 255 },      { 0, 255, 0, 255 },     { 0, 204, 0, 255 },
    { 0, 153, 0, 255 },     { 0, 102, 0, 255 },     { 0, 51, 0, 255 },      { 0, 0, 0, 255 }
};

// Standard colors (V4P uses 8-bit color indices, not RGB values)
const Color gray = 192;
const Color maroon = 128;
const Color purple = 129;
const Color green = 32;
const Color cyan = 255;
const Color black = 0;
const Color red = 1;
const Color blue = 2;
const Color yellow = 3;
const Color dark = 128;
const Color olive = 67;
const Color fluo = 48;

// Display context structure
typedef struct v4pDisplay_s {
    int width;
    int height;
    uint8_t* framebuffer;
    int stride;
    int bpp;
} v4pDisplay_s;

// Check if DRM device supports dumb buffers
static int check_dumb_buffer_support() {
    uint64_t capability = 0;

    if (drmGetCap(drm_context.fd, DRM_CAP_DUMB_BUFFER, &capability) < 0 || ! capability) {
        v4pi_error("DRM device does not support dumb buffers");
        return -1;
    }

    return 0;
}

// Helper function to find a connected connector
static drmModeConnector* find_connected_connector(drmModeRes* resources) {
    for (int i = 0; i < resources->count_connectors; i++) {
        drmModeConnector* connector = drmModeGetConnector(drm_context.fd, resources->connectors[i]);
        if (connector && connector->connection == DRM_MODE_CONNECTED
            && connector->count_modes > 0) {
            return connector;
        }
        drmModeFreeConnector(connector);
    }
    return NULL;
}

// Helper function to find an encoder for a connector
static drmModeEncoder* find_encoder_for_connector(drmModeRes* resources,
                                                  drmModeConnector* connector) {
    if (connector->encoder_id) {
        for (int i = 0; i < resources->count_encoders; i++) {
            if (resources->encoders[i] == connector->encoder_id) {
                return drmModeGetEncoder(drm_context.fd, connector->encoder_id);
            }
        }
    }
    return NULL;
}

// Create a dumb buffer (simple framebuffer)
static int create_dumb_buffer(uint32_t width,
                              uint32_t height,
                              uint32_t bpp,
                              uint32_t* handle,
                              uint32_t* pitch,
                              uint32_t* size) {
    struct drm_mode_create_dumb create = { 0 };

    create.width = width;
    create.height = height;
    create.bpp = bpp;

    if (drmIoctl(drm_context.fd, DRM_IOCTL_MODE_CREATE_DUMB, &create) < 0) {
        v4pi_error("Failed to create dumb buffer");
        return -1;
    }

    *handle = create.handle;
    *pitch = create.pitch;
    *size = create.size;

    return 0;
}

// Map dumb buffer to userspace
static uint8_t* map_dumb_buffer(uint32_t handle, uint32_t size) {
    struct drm_mode_map_dumb map = { 0 };
    uint8_t* memory;

    map.handle = handle;
    if (drmIoctl(drm_context.fd, DRM_IOCTL_MODE_MAP_DUMB, &map) < 0) {
        v4pi_error("Failed to map dumb buffer");
        return NULL;
    }

    memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_context.fd, map.offset);
    if (memory == MAP_FAILED) {
        v4pi_error("Failed to mmap dumb buffer");
        return NULL;
    }

    return memory;
}

// Initialize DRM with dumb buffer
static int init_drm_dumb_buffer() {
    uint32_t handle, pitch, size;

    // Use the display mode dimensions
    uint32_t width = drm_context.mode.hdisplay;
    uint32_t height = drm_context.mode.vdisplay;

    // Create dumb buffer
    if (create_dumb_buffer(width, height, 32, &handle, &pitch, &size) < 0) {
        return -1;
    }

    // Map buffer to userspace
    uint8_t* memory = map_dumb_buffer(handle, size);
    if (! memory) {
        struct drm_mode_destroy_dumb destroy = { handle };
        drmIoctl(drm_context.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
        return -1;
    }

    drm_context.fb_width = width;
    drm_context.fb_height = height;
    drm_context.fb_stride = pitch;
    drm_context.fb_handle = handle;
    drm_context.fb_memory = memory;
    drm_context.fb_size = size;

    v4pDisplayWidth = drm_context.fb_width;
    v4pDisplayHeight = drm_context.fb_height;

    return 0;
}

// Set DRM mode (connect framebuffer to CRT controller)
static int set_drm_mode() {
    // For dumb buffers, we need to create a framebuffer object to use with
    // SetCrtc This is the minimal required framebuffer setup
    uint32_t fb_id;
    uint32_t handles[4] = { drm_context.fb_handle };
    uint32_t pitches[4] = { drm_context.fb_stride };
    uint32_t offsets[4] = { 0 };

    if (drmModeAddFB2(drm_context.fd,
                      drm_context.fb_width,
                      drm_context.fb_height,
                      DRM_FORMAT_XRGB8888,
                      handles,
                      pitches,
                      offsets,
                      &fb_id,
                      0)
        < 0) {
        v4pi_error("Failed to create DRM framebuffer for mode setting");
        return -1;
    }

    // Set the mode
    if (drmModeSetCrtc(drm_context.fd,
                       drm_context.crtc_id,
                       fb_id,
                       0,
                       0,
                       &drm_context.connector->connector_id,
                       1,
                       &drm_context.mode)
        < 0) {
        v4pi_error("Failed to set CRTC mode");
        drmModeRmFB(drm_context.fd, fb_id);
        return -1;
    }

    // Store the framebuffer ID for cleanup
    drm_context.fb_id = fb_id;

    return 0;
}

// Initialize DRM
static int init_drm() {
    // Open DRM device
    drm_context.fd = open("/dev/dri/card0", O_RDWR);
    if (drm_context.fd < 0) {
        v4pi_error("Failed to open DRM device /dev/dri/card0");
        return -1;
    }

    // Check for dumb buffer support
    if (check_dumb_buffer_support() < 0) {
        close(drm_context.fd);
        return -1;
    }

    // Get DRM resources
    drm_context.resources = drmModeGetResources(drm_context.fd);
    if (! drm_context.resources) {
        v4pi_error("Failed to get DRM resources");
        close(drm_context.fd);
        return -1;
    }

    // Find connected connector
    drm_context.connector = find_connected_connector(drm_context.resources);
    if (! drm_context.connector) {
        v4pi_error("No connected connector found");
        drmModeFreeResources(drm_context.resources);
        close(drm_context.fd);
        return -1;
    }

    // Find encoder for connector
    drm_context.encoder = find_encoder_for_connector(drm_context.resources, drm_context.connector);
    if (! drm_context.encoder) {
        v4pi_error("No encoder found for connector");
        drmModeFreeConnector(drm_context.connector);
        drmModeFreeResources(drm_context.resources);
        close(drm_context.fd);
        return -1;
    }

    // Use first mode
    drm_context.mode = drm_context.connector->modes[0];
    drm_context.crtc_id = drm_context.encoder->crtc_id;

    // Create dumb buffer
    if (init_drm_dumb_buffer() < 0) {
        drmModeFreeEncoder(drm_context.encoder);
        drmModeFreeConnector(drm_context.connector);
        drmModeFreeResources(drm_context.resources);
        close(drm_context.fd);
        return -1;
    }

    // Set the mode (connect framebuffer to display)
    if (set_drm_mode() < 0) {
        // Cleanup framebuffer
        if (drm_context.fb_memory) {
            munmap(drm_context.fb_memory, drm_context.fb_size);
        }
        if (drm_context.fb_handle) {
            struct drm_mode_destroy_dumb destroy = { drm_context.fb_handle };
            drmIoctl(drm_context.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
        }

        drmModeFreeEncoder(drm_context.encoder);
        drmModeFreeConnector(drm_context.connector);
        drmModeFreeResources(drm_context.resources);
        close(drm_context.fd);
        return -1;
    }

    drm_context.initialized = true;
    return 0;
}

// Cleanup DRM
static void cleanup_drm() {
    if (! drm_context.initialized) {
        return;
    }

    // Restore original CRTC configuration
    if (drm_context.crtc_id) {
        drmModeSetCrtc(drm_context.fd, drm_context.crtc_id, 0, 0, 0, NULL, 0, NULL);
    }

    // Cleanup framebuffer
    if (drm_context.fb_id) {
        drmModeRmFB(drm_context.fd, drm_context.fb_id);
    }

    if (drm_context.fb_memory) {
        munmap(drm_context.fb_memory, drm_context.fb_size);
    }

    if (drm_context.fb_handle) {
        struct drm_mode_destroy_dumb destroy = { drm_context.fb_handle };
        drmIoctl(drm_context.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
    }

    if (drm_context.encoder) {
        drmModeFreeEncoder(drm_context.encoder);
    }

    if (drm_context.connector) {
        drmModeFreeConnector(drm_context.connector);
    }

    if (drm_context.resources) {
        drmModeFreeResources(drm_context.resources);
    }

    if (drm_context.fd >= 0) {
        close(drm_context.fd);
    }

    memset(&drm_context, 0, sizeof(drm_context));
}

// V4P Interface Functions

Boolean v4pDisplayInit(int quality, Boolean fullscreen) {
    (void) quality;
    (void) fullscreen;  // Unused for now

    if (init_drm() < 0) {
        return false;
    }

    // Create default display context
    v4pDisplayDefaultContext = malloc(sizeof(v4pDisplay_s));
    if (! v4pDisplayDefaultContext) {
        cleanup_drm();
        return false;
    }

    v4pDisplayDefaultContext->width = drm_context.fb_width;
    v4pDisplayDefaultContext->height = drm_context.fb_height;
    v4pDisplayDefaultContext->framebuffer = drm_context.fb_memory;
    v4pDisplayDefaultContext->stride = drm_context.fb_stride;
    v4pDisplayDefaultContext->bpp = 32;

    v4pDisplayContext = v4pDisplayDefaultContext;

    return true;
}

V4pDisplayP v4pDisplayNewContext(int width, int height) {
    // For now, we only support the default context
    (void) width;
    (void) height;
    return v4pDisplayDefaultContext;
}

V4pDisplayP v4pi_setContext(V4pDisplayP ctx) {
    V4pDisplayP old = v4pDisplayContext;
    v4pDisplayContext = ctx;
    return old;
}

void v4pDisplayFreeContext(V4pDisplayP ctx) {
    if (ctx && ctx != v4pDisplayDefaultContext) {
        free(ctx);
    }
}

Boolean v4pi_start() {
    return true;
}

Boolean v4pi_slice(Coord y, Coord x0, Coord x1, Color c) {
    if (! v4pDisplayContext || ! v4pDisplayContext->framebuffer) {
        return false;
    }

    // Simple implementation: draw horizontal line using palette colors
    // Convert 8-bit color index to 32-bit XRGB8888 format
    if (y >= 0 && y < v4pDisplayContext->height && x0 >= 0 && x1 < v4pDisplayContext->width) {
        uint32_t* pixel
            = (uint32_t*) (v4pDisplayContext->framebuffer + y * v4pDisplayContext->stride);
        uint32_t color = (palette[c].r << 16) | (palette[c].g << 8) | palette[c].b;

        for (int x = x0; x <= x1; x++) {
            pixel[x] = color;
        }
    }

    return true;
}

Boolean v4pi_end() {
    // For dumb buffers, changes are immediately visible
    return true;
}

Boolean v4pi_error(char* s, ...) {
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    return false;
}

Boolean v4pi_collide(ICollide i1,
                     ICollide i2,
                     Coord py,
                     Coord x1,
                     Coord x2,
                     PolygonP p1,
                     PolygonP p2) {
    // Stub implementation
    (void) i1;
    (void) i2;
    (void) py;
    (void) x1;
    (void) x2;
    (void) p1;
    (void) p2;
    return false;
}

void v4pDisplayQuit() {
    if (v4pDisplayDefaultContext) {
        free(v4pDisplayDefaultContext);
        v4pDisplayDefaultContext = NULL;
    }
    v4pDisplayContext = NULL;
    cleanup_drm();
}