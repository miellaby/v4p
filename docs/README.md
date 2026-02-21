# V4P Web Demos

This directory contains web demos of the V4P vector graphics engine compiled to WebAssembly using Emscripten.

## Available Demos

- **[Asteroids Game](asteroids.html)** - Classic asteroids game with full physics and particle effects
- **[3D Star field](starfield_3D.html)** - 3D starfield effect with twinkling stars

## How to Play Asteroids

- **Arrow Keys**: Rotate and thrust your ship
- **Space**: Fire bullets
- **Long Press Space**: Start or restart the game
- **ESC**: Exit the game

## Technical Details

- **Engine**: V4P vector graphics engine
- **Backend**: Emscripten Bitmap (fastest WASM rendering)
- **Language**: C compiled to WebAssembly
- **Controls**: Keyboard input via Emscripten

## Performance

The demo includes:
- Real-time FPS counter
- Adjustable framerate controls
- Memory growth for smooth gameplay
- Asyncify for responsive input handling