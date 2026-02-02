/**
 * V4P Implementation for Linux DRM (modern libdrm backend)
 * 
 * This is a modern DRM backend that follows best practices from:
 * https://linuxembedded.fr/2025/05/introduction-au-direct-rendering-manager-et-a-libdrm
 *
 * Features:
 * - Proper error handling and resource management
 * - Clean separation of concerns
 * - Efficient DRM resource usage
 * - No plane manipulation (as requested)
 * - Simplified dumb buffer approach
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
#include <errno.h>

#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "v4pi.h"

// Define the v4pi_context_s structure that was forward-declared in v4pi.h
typedef struct v4pi_context_s {
    int fd;                  // DRM file descriptor
    drmModeRes* resources;    // DRM resources
    drmModeConnector* connector;  // Connected display
    drmModeEncoder* encoder;      // Encoder for display
    drmModeModeInfo mode;         // Display mode
    uint32_t crtc_id;            // CRT controller ID
    
    // Framebuffer info
    uint32_t fb_id;              // DRM framebuffer ID
    uint32_t fb_width;           // Framebuffer width
    uint32_t fb_height;          // Framebuffer height
    uint32_t fb_stride;          // Framebuffer stride (pitch)
    uint32_t fb_handle;          // GEM handle for the buffer
    uint8_t* fb_memory;          // Mapped framebuffer memory
    uint32_t fb_size;            // Framebuffer size in bytes
    
    // Current state
    Boolean initialized;
} v4pi_context_s;

// DRM-specific types (alias for compatibility)
typedef v4pi_context_s DRMContext;

// Global display context
static v4pi_context_s drm_context = { 0 };

V4piContextP v4pi_defaultContext = NULL;
V4piContextP v4pi_context = NULL;

// Display dimensions
V4pCoord v4p_displayWidth = 0;
V4pCoord v4p_displayHeight = 0;

// Error handling macro
#define CHECK_DRM_ERROR(cond, msg) \
    do { \
        if (cond) { \
            v4p_error("DRM error: %s - %s", msg, strerror(errno)); \
            return -1; \
        } \
    } while (0)



// Helper function to find a connected connector
static drmModeConnector* find_connected_connector(drmModeRes* resources) {
    v4pi_debug("Searching for connected connectors (%d available)", resources->count_connectors);
    
    for (int i = 0; i < resources->count_connectors; i++) {
        drmModeConnector* connector = drmModeGetConnector(drm_context.fd, resources->connectors[i]);
        if (!connector) {
            v4pi_debug("Failed to get connector %d", resources->connectors[i]);
            continue;
        }
        
        v4pi_debug("Connector %d: type=%d, connection=%d, modes=%d",
                   connector->connector_id, connector->connector_type,
                   connector->connection, connector->count_modes);
        
        if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0) {
            v4pi_debug("Found suitable connector %d with %d modes",
                       connector->connector_id, connector->count_modes);
            return connector;
        }
        
        // Also try disconnected connectors that have modes (for virtual displays)
        if (connector->count_modes > 0) {
            v4pi_debug("Trying connector %d even though it's not marked as connected",
                       connector->connector_id);
            return connector;
        }
        
        drmModeFreeConnector(connector);
    }
    
    v4pi_debug("No suitable connector found");
    return NULL;
}

// Helper function to find an encoder for a connector
static drmModeEncoder* find_encoder_for_connector(drmModeRes* resources,
                                                  drmModeConnector* connector) {
    v4pi_debug("Looking for encoder for connector %d (encoder_id=%d)",
               connector->connector_id, connector->encoder_id);
    v4pi_debug("Available encoders: %d", resources->count_encoders);
    
    if (connector->encoder_id) {
        v4pi_debug("Connector has preferred encoder_id: %d", connector->encoder_id);
        for (int i = 0; i < resources->count_encoders; i++) {
            v4pi_debug("Checking encoder %d (id=%d)", i, resources->encoders[i]);
            if (resources->encoders[i] == connector->encoder_id) {
                drmModeEncoder* encoder = drmModeGetEncoder(drm_context.fd, connector->encoder_id);
                if (encoder) {
                    v4pi_debug("Found matching encoder: %d", encoder->encoder_id);
                    return encoder;
                }
            }
        }
    }
    
    // Try to find any compatible encoder
    v4pi_debug("No preferred encoder found, trying to find any compatible encoder");
    for (int i = 0; i < resources->count_encoders; i++) {
        drmModeEncoder* encoder = drmModeGetEncoder(drm_context.fd, resources->encoders[i]);
        if (encoder) {
            v4pi_debug("Found encoder %d, checking compatibility", encoder->encoder_id);
            // In a real implementation, we would check encoder->possible_crtcs here
            // For now, return the first available encoder
            return encoder;
        }
    }
    
    v4pi_debug("No compatible encoder found");
    return NULL;
}

// Create a dumb buffer (simple framebuffer)
static int create_dumb_buffer(uint32_t width, uint32_t height, uint32_t bpp,
                              uint32_t* handle, uint32_t* pitch, uint32_t* size) {
    struct drm_mode_create_dumb create = { 0 };

    create.width = width;
    create.height = height;
    create.bpp = bpp;

    if (drmIoctl(drm_context.fd, DRM_IOCTL_MODE_CREATE_DUMB, &create) < 0) {
        v4p_error("Failed to create dumb buffer: %s", strerror(errno));
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
        v4p_error("Failed to map dumb buffer: %s", strerror(errno));
        return NULL;
    }

    memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_context.fd, map.offset);
    if (memory == MAP_FAILED) {
        v4p_error("Failed to mmap dumb buffer: %s", strerror(errno));
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
    if (!memory) {
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

    return 0;
}

// Set DRM mode (connect framebuffer to CRT controller)
static int set_drm_mode() {
    uint32_t fb_id;
    uint32_t handles[4] = { drm_context.fb_handle };
    uint32_t pitches[4] = { drm_context.fb_stride };
    uint32_t offsets[4] = { 0 };

    // Create framebuffer object
    if (drmModeAddFB2(drm_context.fd,
                      drm_context.fb_width,
                      drm_context.fb_height,
                      DRM_FORMAT_XRGB8888,
                      handles,
                      pitches,
                      offsets,
                      &fb_id,
                      0) < 0) {
        v4p_error("Failed to create DRM framebuffer: %s", strerror(errno));
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
                       &drm_context.mode) < 0) {
        v4p_error("Failed to set CRTC mode: %s", strerror(errno));
        
        // Try without specifying connector (some DRM drivers prefer this)
        v4pi_debug("Trying to set CRTC mode without connector specification");
        if (drmModeSetCrtc(drm_context.fd,
                           drm_context.crtc_id,
                           fb_id,
                           0,
                           0,
                           NULL,
                           1,
                           &drm_context.mode) < 0) {
            v4p_error("Failed to set CRTC mode (alternative method): %s", strerror(errno));
            drmModeRmFB(drm_context.fd, fb_id);
            return -1;
        }
    }

    // Store the framebuffer ID for cleanup
    drm_context.fb_id = fb_id;

    return 0;
}

// Helper function to find available DRM device
static int find_drm_device() {
    char device_path[20];
    int fd = -1;
    
    // Try multiple card devices
    for (int i = 0; i < 4; i++) {
        snprintf(device_path, sizeof(device_path), "/dev/dri/card%d", i);
        fd = open(device_path, O_RDWR);
        if (fd >= 0) {
            v4pi_debug("Using DRM device: %s", device_path);
            return fd;
        }
    }
    
    return -1;
}

// Initialize DRM
static int init_drm() {
    // Open DRM device
    drm_context.fd = find_drm_device();
    CHECK_DRM_ERROR(drm_context.fd < 0, "Failed to open any DRM device /dev/dri/card*");

    // Get DRM resources
    drm_context.resources = drmModeGetResources(drm_context.fd);
    CHECK_DRM_ERROR(!drm_context.resources, "Failed to get DRM resources");

    // Find connected connector
    drm_context.connector = find_connected_connector(drm_context.resources);
    CHECK_DRM_ERROR(!drm_context.connector, "No connected connector found");

    // Find encoder for connector
    drm_context.encoder = find_encoder_for_connector(drm_context.resources, drm_context.connector);
    CHECK_DRM_ERROR(!drm_context.encoder, "No encoder found for connector");

    // Use first mode
    drm_context.mode = drm_context.connector->modes[0];
    drm_context.crtc_id = drm_context.encoder->crtc_id;

    // Create dumb buffer
    if (init_drm_dumb_buffer() < 0) {
        goto cleanup_encoder;
    }

    // Set the mode (connect framebuffer to display)
    if (set_drm_mode() < 0) {
        goto cleanup_dumb_buffer;
    }

    drm_context.initialized = true;
    return 0;

cleanup_dumb_buffer:
    if (drm_context.fb_memory) {
        munmap(drm_context.fb_memory, drm_context.fb_size);
    }
    if (drm_context.fb_handle) {
        struct drm_mode_destroy_dumb destroy = { drm_context.fb_handle };
        drmIoctl(drm_context.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
    }

cleanup_encoder:
    drmModeFreeEncoder(drm_context.encoder);
    drmModeFreeConnector(drm_context.connector);
    drmModeFreeResources(drm_context.resources);
    close(drm_context.fd);
    return -1;
}

// Cleanup DRM
static void cleanup_drm() {
    if (!drm_context.initialized) {
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

Boolean v4pi_init(int quality, Boolean fullscreen) {
    (void) quality;
    (void) fullscreen;  // Unused for now
    
    if (init_drm() != 0) {
        return failure;
    }

    // Allocate default context
    v4pi_defaultContext = malloc(sizeof(v4pi_context_s));
    if (!v4pi_defaultContext) {
        cleanup_drm();
        return failure;
    }
    
    // Copy the DRM context to the default context
    memcpy(v4pi_defaultContext, &drm_context, sizeof(v4pi_context_s));
    
    v4pi_setContext(v4pi_defaultContext);
    return success;
}

V4piContextP v4pi_newContext(int width, int height) {
    // For now, we only support the default context
    (void) width;
    (void) height;
    return v4pi_defaultContext;
}

V4piContextP v4pi_setContext(V4piContextP ctx) {
    if (!ctx) {
        return NULL;
    }
    
    v4pi_context = ctx;
    v4p_displayWidth = ctx->fb_width;
    v4p_displayHeight = ctx->fb_height;
    return ctx;
}

void v4pi_destroyContext(V4piContextP ctx) {
    if (ctx && ctx != v4pi_defaultContext) {
        free(ctx);
    }
}

Boolean v4pi_start() {
    return success;
}

Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    if (!v4pi_context || !v4pi_context->fb_memory) {
        return failure;
    }
    
    if (y >= 0 && y < v4pi_context->fb_height && x0 >= 0 && x1 <= v4pi_context->fb_width) {
        uint32_t* pixel = (uint32_t*) (v4pi_context->fb_memory + y * v4pi_context->fb_stride);
        // Convert 8-bit color index to 32-bit XRGB8888 format
        uint32_t color = (V4P_PALETTE_R(c) << 16) | (V4P_PALETTE_G(c) << 8) | V4P_PALETTE_B(c);
        for (int x = x0; x < x1; x++) {
            pixel[x] = color;
        }
    }

    return success;
}

Boolean v4pi_end() {
    // For dumb buffers, changes are immediately visible
    return success;
}

void v4pi_destroy() {
    if (v4pi_defaultContext) {
        free(v4pi_defaultContext);
        v4pi_defaultContext = NULL;
    }
    v4pi_context = NULL;
    cleanup_drm();
}