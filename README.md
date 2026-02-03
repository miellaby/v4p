
# V4P  - Vectors For Pocket -  Minimal Vector Graphics Engine

**Ultra-lightweight 2D vector graphics engine** for resource-constrained devices.

## Key Features

- **Tiny footprint**: ~1000 instructions, no dependencies
- **Integer-only math**: No floating point, no hardware requirements
- **Embeddable**: Easy integration into any GUI/game project
- **Efficient algorithm**: Bresenham-like scanline polygon conversion
- **Collision detection**: Nice side-effect
- **Memory ultra-efficient**: No Z-buffer/S-buffer needed

## Demo

[![V4P Demo Video](https://img.youtube.com/vi/W7DSjBT2Jfk/0.jpg)](https://www.youtube.com/watch?v=W7DSjBT2Jfk)

## Integration

Simply add V4P C files to your project and implement a horizontal line drawing function (e.g., memset-like video buffer operation).

## Technical Details

- **Algorithm**: Scanline-based polygon rendering with incremental active edge computation
- **Sorting**: Quick-sorted lists for efficient rendering
- **Collision**: Bit-based computation for pixel-perfect detection
- **Platforms**: Originally developed for Palm OS, easily adaptable to embedded linux and tiny devices

## Limitations

- No anti-aliasing (downscaling workaround possible)
- No translucent polygons (requires algorithm changes)
- No curve support (polygons and discs only)
- Limited to 16 layers (bit-based operations)
- Basic approximated trigonometry functions
- No partial scene refresh optimizations

