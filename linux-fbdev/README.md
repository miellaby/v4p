# Linux Framebuffer Backend for V4P

**Created by Devstral - Modern Linux backend for the V4P vector rasterizer**

This is a modern Linux Framebuffer (fbdev) backend for the V4P vector image rasterizer.

Linux Framebuffer was chosen for several key advantages:

1. **Minimal Dependencies**: Only requires standard Linux kernel headers
2. **Low-level Bit Mapping**: Direct access to video memory via `/dev/fb0`
3. **Lightweight**: No external libraries needed (unlike SDL, X11, Wayland)
4. **Universal**: Available on most Linux systems including embedded devices
5. **Simple API**: Easy to implement the v4p callback interface

## Comparison with Existing Backends

| Feature | SDL 1.2 | Xlib | Framebuffer |
|---------|---------|------|-------------|
| **Dependencies** | SDL library | X11 libraries | None (kernel only) |
| **Complexity** | Medium | High | Low |
| **Embedded Support** | Limited | No | Excellent |
| **Performance** | Good | Good | Excellent |
| **Memory Usage** | Medium | High | Low |
| **Modern Linux** | Legacy | Legacy | Native |

## Implementation Details

### Files

- `v4pi.h` - Header file with v4p interface
- `v4pi.c` - Main implementation (complete callback-based backend)
- `v4p_ll.h` - Low-level type definitions
- `Makefile` - Build system
- `test_fbdev.c` - Test program

### Core Functions

- `v4pDisplayInit()` - Opens framebuffer, sets up 8-bit mode, loads palette
- `v4pDisplaySlice()` - Draws horizontal slices directly to framebuffer memory
- `v4pDisplayNewContext()` - Creates off-screen buffers
- `v4pDisplaySetContext()` - Switches between contexts
- `v4pDisplayQuit()` - Cleans up resources

## Technical Highlights

1. **Palette Management**: Dynamic allocation and initialization of 256-color palette
2. **Color Conversion**: Handles both 8-bit and higher bit-depth framebuffers
3. **Memory Mapping**: Uses `mmap()` for efficient framebuffer access
4. **Context Switching**: Seamless switching between main display and buffers
5. **Resource Cleanup**: Proper cleanup of memory maps, file descriptors, and palette

## Usage Example

```c
// Initialize framebuffer backend
v4pDisplayInit(V4P_QUALITY_NORMAL, false);

// Create a rendering context
V4pDisplayP ctx = v4pDisplayNewContext(320, 240);
v4pDisplaySetContext(ctx);

// Render using v4p engine
v4pDisplayStart();
v4pDisplaySlice(y, x0, x1, red); // Called by v4p engine
v4pDisplayEnd();

// Cleanup
v4pDisplayFreeContext(ctx);
v4pDisplayQuit();
```

## Building and Testing

### Compilation

```bash
cd linux-fbdev
make
```

### Testing

```bash
make test
./test_fbdev
```

### Cleanup

```bash
make clean
```

## Integration with V4P

To use this backend with the main V4P engine:

1. Include the `v4pi.h` header
2. Call `v4pDisplayInit()` before using V4P functions
3. The V4P engine will automatically use the framebuffer backend through the callback interface
4. Call `v4pDisplayQuit()` when done

## Compatibility

- **Linux Kernel**: Requires framebuffer device support (`/dev/fb0`)
- **Color Depth**: Works best with 8-bit framebuffers, but supports higher depths
- **Hardware**: Compatible with most graphics hardware supported by Linux framebuffer
