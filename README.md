
# V4P  - Vectors For Pocket -  Minimal Vector Graphics Engine

**Ultra-lightweight 2D vector graphics engine** for resource-constrained devices.

## Demo

Play the emscripten WASM build of the Asteroids clone made with V4P and its addons.
[![Asteroids Clone](screenshots/asteroids.png)](http://miellaby.github.io/v4p/asteroids.html)
[![Starfield](screenshots/starfield_3d.png)](http://miellaby.github.io/v4p/starfield_3d.html)

## Key Features

- **Tiny footprint**: ~1000 instructions, no dependencies
- **Integer-only math**: No floating point, no hardware requirements
- **Embeddable**: Easy integration into any GUI/game project
- **Efficient algorithm**: Bresenham-like scanline polygon conversion
- **Collision detection**: Nice side-effect
- **Memory ultra-efficient**: No Z-buffer/S-buffer needed

## Integration

Copy V4P C files into your project and implement the horizontal line drawing function (e.g. memset into a video buffer).

## Technical Details

- **Algorithm**: Scanline-based polygon rendering with incremental active edge computation
- **Sorting**: Quick-sorted lists for efficient rendering
- **Collision**: Bit-based computation for pixel-perfect detection
- **Platforms**: Originally developed for Palm OS, easily adaptable to embedded linux and tiny devices

## Limitations

- No anti-aliasing (downscaling workaround possible)
- No translucent polygons (requires algorithm changes)
- No curve support (polygons and discs only)
- Limited to 32 layers (bit-based operations)
- Basic approximated trigonometry functions
- No partial scene refresh optimizations

# Similar Projects

Other software 2D-rasterizers for tiny devices
- [µGUI](https://embeddedlightning.com/ugui/)
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [u8g2](https://github.com/olikraus/u8g2)
- [HAGL](https://github.com/tuupola/hagl)
- [MCUFont](https://github.com/mcufont/mcufont)
- [EmberGL](https://github.com/EmberGL-org/EmberGL)
- [PlutoVG](https://github.com/sammycage/plutovg)
- [ctx.graphics](https://ctx.graphics/internals/).
- [NanoX](https://github.com/ghaerr/microwindows)
- [ThorVG](https://github.com/thorvg/thorvg) /  [LVGL](https://github.com/lvgl/lvgl)
- Adafruit-GFX-Library
- Slint
- ugfx.io
- Moddable Poco engine
- GxEPD2
- and many more...

3D-rasterizers for embedded devices:
- [TinyGL](https://bellard.org/TinyGL/).
- [EmberGL](https://github.com/EmberGL-org/EmberGL)
- [TGX](https://github.com/vindar/tgx)
- [Pico3D](https://github.com/bernhardstrobl/Pico3D/tree/main)
- and many more...

Low-dependencies 2D-rasterizers (GPU free)
- [AGG Anti-Grain Geometry](https://en.wikipedia.org/wiki/Anti-Grain_Geometry)
- [Blend2D](https://blend2d.com/)
- [canvas_ity](https://github.com/a-e-k/canvas_ity)
- [LinaVG](https://github.com/inanevin/LinaVG)
- [NanoVG](https://github.com/memononen/nanovg)
- [SDL2_gfx](http://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/)
- and many more...

The big players
- Cairo
- Skia
- QPainter
- and more...


# Comparison

While other libraries offer much more features and better performance on modern hardware,
[V4P](https://github.com/miellaby/v4p)'s focus on minimal dependencies and resource consumption
makes it uniquely suited for projects where resources are extremly limited.

Its hierarchical scene tree API simplifies the creation of animated GUIs and games. Also, the
scanline algorithm provided collision detection for free.

V4P is ideal when:
- Targeting embedded systems with extremely limited memory, resources and processing abilities, like microcontrollers
- Requiring hierarchical scene management and a built-in collision detection

Other libraries are better when:
- You're working with modern desktop/mobile platforms with GPU acceleration
- You need State-Of-The-Art graphic primitives, anti-aliasing and color blending

