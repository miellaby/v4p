/**
 * V4P Implementation for Linux DRM - Internal
 *
 * This is a pure DRM-only backend with no dependencies on GBM, Mesa, or
 * framebuffer devices. It uses DRM IOCTLs to create and manage framebuffers
 * directly.
 */

#ifndef _V4PI_H
#define _V4PI_H

#include "v4p_ll.h"
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// DRM-specific types
typedef struct {
    int fd;  // DRM file descriptor (/dev/dri/card0)
    drmModeRes* resources;  // DRM resources
    drmModeConnector* connector;  // Connected display
    drmModeEncoder* encoder;  // Encoder for display
    drmModeCrtc* crtc;  // CRT controller
    drmModeModeInfo mode;  // Display mode
    uint32_t crtc_id;  // CRT controller ID

    // DRM framebuffer info
    uint32_t fb_id;  // DRM framebuffer ID
    uint32_t fb_width;  // Framebuffer width
    uint32_t fb_height;  // Framebuffer height
    uint32_t fb_stride;  // Framebuffer stride (pitch)
    uint32_t fb_handle;  // GEM handle for the buffer
    uint8_t* fb_memory;  // Mapped framebuffer memory
    uint32_t fb_size;  // Framebuffer size in bytes

    // Double buffering
    uint32_t front_fb_id;  // Front buffer FB ID
    uint32_t back_fb_id;  // Back buffer FB ID
    uint8_t* back_buffer;  // Back buffer memory

    // Current state
    Boolean initialized;
} DRMContext;

#endif