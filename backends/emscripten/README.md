# Emscripten Backends

Emscripten/WebAssembly backends for V4P. For the fun.

## Bitmap
- **File**: `bitmap/v4pi.c`
- **Renders**: To in-memory RGBA bitmap, transfers to canvas
- **Performance**: Fastest (reduced JS/C interface, in-memory operations)
- **Build**: `make TARGET=emscripten BACKEND=bitmap`

## Canvas
- **File**: `canvas/v4pi.c`
- **Renders**: HTML5 Canvas 2D context
- **Performance**: Fast (drawing directly into canvas without a temporary bitmap)
- **Build**: `make TARGET=emscripten BACKEND=canvas`

## DOM
- **File**: `dom/v4pi.c`
- **Renders**: Using DIV elements for pixels
- **Performance**: Extremly Slow (Massive amount of Garbage Collected DOM Elements)
- **Build**: `make TARGET=emscripten BACKEND=dom`
