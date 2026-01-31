# Asteroids Game

A simple Asteroids clone built with the V4P vector graphics engine.

## Controls

- **Left Arrow**: Rotate ship counter-clockwise
- **Right Arrow**: Rotate ship clockwise
- **Up Arrow**: Thrust (move forward)
- **Space**: Fire bullets
- **ESC**: Exit game

## Gameplay

- Destroy asteroids by shooting them with bullets
- Avoid colliding with asteroids
- Each asteroid destroyed gives 100 points
- You start with 3 lives
- Game ends when all lives are lost

## Building

The game is built automatically with the other demos:

```bash
make
```

Or specifically:

```bash
cd demos/asteroids
make
```

## Implementation Details

- Uses the V4P vector graphics engine
- Implements collision detection using the game engine's collision system
- Objects are created using SVG path decoding for complex shapes
- Game state is managed with simple arrays and counters