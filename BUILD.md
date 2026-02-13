# V4P Build Guide

## Quick Start

```bash
# Basic build (Linux with SDL1.2 backend)
make

# Debug build with verbose output
make DEBUG=1 V=1

# Build with different backend (default SDL)
make BACKEND=xlib

# Install to system
make install
```

### Backend-Specific Dependencies

#### SDL Backend (default)
- **SDL 1.2**: `libsdl1.2-dev` (Debian/Ubuntu) or `SDL-devel` (RHEL/Fedora)

#### Xlib Backend
- **X11 Development Libraries**: `libx11-dev` (Debian/Ubuntu) or `libX11-devel` (RHEL/Fedora)

#### Framebuffer Backend (fbdev)
- **Linux Framebuffer**: Typically available in standard Linux kernel

#### DRM Backend
- **libdrm**: `libdrm-dev` (Debian/Ubuntu) or `libdrm-devel` (RHEL/Fedora)

#### libcaca Backend
- **libcaca**: `libcaca-dev` (Debian/Ubuntu) or `caca-devel` (RHEL/Fedora)

### Optional Dependencies

- **Lua 5.1**: For Lua bindings (`liblua5.1-dev` on Debian/Ubuntu)
- **PalmOS SDK**: For PalmOS target builds
- **ESP32 Toolchain**: For ESP32 target builds

## Installation on Major Distributions

### Debian/Ubuntu

```bash
# Install core build tools
sudo apt update
sudo apt install build-essential make gcc

# Install SDL backend dependencies (default)
sudo apt install libsdl1.2-dev

# For other backends (optional)
sudo apt install libx11-dev libdrm-dev libcaca-dev lua5.1 liblua5.1-0-dev
```

### RHEL/Fedora/CentOS

```bash
# Install core build tools
sudo dnf install gcc make

# Install SDL backend dependencies (default)
sudo dnf install SDL-devel

# For other backends (optional)
sudo dnf install libX11-devel libdrm-devel caca-devel lua-devel
```

### Arch Linux

```bash
# Install core build tools
sudo pacman -S base-devel gcc make

# Install SDL backend dependencies (default)
sudo pacman -S sdl

# For other backends (optional)
sudo pacman -S libx11 libdrm libcaca lua
```

### macOS (Homebrew)

```bash
# Install core build tools
brew install gcc make

# Install SDL backend dependencies (default)
brew install sdl

# For other backends (optional)
brew install libx11 libdrm libcaca lua
```

## Build Options

### Basic Build Commands

```bash
# Release build (default)
make

# Debug build with symbols
make DEBUG=1

# Debug build with AddressSanitizer
make DEBUG=1 ASAN=1

# Verbose output (recommended for development)
make V=1
```

### Backend Selection

```bash
# SDL backend (default)
make BACKEND=sdl

# Xlib backend
make BACKEND=xlib

# Framebuffer backend
make BACKEND=fbdev

# DRM backend
make BACKEND=drm

# libcaca backend
make BACKEND=caca
```

### Platform Targets

```bash
# Linux (default)
make TARGET=linux

# PalmOS (broken)
make TARGET=palmos

# ESP32 (future)
make TARGET=esp32
```

### Installation Options

```bash
# Install to default location (/usr/local)
make install

# Install to custom prefix
make PREFIX=/opt/v4p install

# Uninstall
make uninstall
```

## Build Targets

- `all`: Build everything (default)
- `libv4p.a`: Build core library only
- `addons`: Build all addon libraries
- `demos`: Build all demonstration programs
- `tests`: Build all test programs
- `clean`: Clean build artifacts
- `install`: Install to system
- `uninstall`: Remove installed files
- `help`: Show usage information

## Building Specific Components

### Core Library

```bash
make libv4p.a
```

### Addon Libraries

```bash
# Build all addons
make addons

# Build specific addon
make libg4p.a      # Game engine
make libqfont.a    # Font rendering
make libv4pserial.a # Polygon (de)serializer
make libparticles.a # Particle system
make libdebug.a    # Debug utilities
```

### Demos

```bash
# Build all demos
make demos

# Build specific demo
make demos/demo_name
```

### Tests

```bash
# Build all tests
make tests

# Build specific test
make tests/test_name
```

## Advanced Build Configuration

### Environment Variables

You can override default tools:

```bash
# Use different compiler
make CC=clang

# Use different archiver
make AR=llvm-ar
```

### Trace System

Enable detailed tracing for debugging:

```bash
# Enable all trace tags
make TRACE=all

# Enable specific trace tags
make TRACE="CIRCLE POLYGON RENDER"
```

Available trace tags: `CIRCLE`, `POLYGON`, `SCAN`, `EDGE`, `SHIFT`, `OPEN`, `COLLISION`, `RENDER`, `TRANSFORM`, `G4P`

## Development Workflow

### Recommended Build

```bash
# Debug build with verbose output
make DEBUG=1 V=1
```

### Clean Build

```bash
make clean
make DEBUG=1 V=1
```

## Troubleshooting

### Missing Dependencies

If you get linker errors about missing libraries:

1. Install the required development packages for your backend
2. Ensure the libraries are in your library path
3. Check that header files are in standard include paths

### Compiler Warnings

V4P uses strict compiler flags. Use `make V=1` to see full compilation commands.

### Backend-Specific Issues

**SDL Backend**: Ensure SDL 1.2 is installed (not SDL 2.0)

**Xlib Backend**: Ensure X11 development headers are installed

**DRM Backend**: May require root permissions for some operations

## Cross-Compilation

### PalmOS

```bash
# Install PalmOS SDK and toolchain
make TARGET=palmos CC=m68k-palmos-gcc AR=m68k-palmos-ar
```

### ESP32

```bash
# Install ESP32 toolchain
make TARGET=esp32 CC=xtensa-esp32-elf-gcc AR=xtensa-esp32-elf-ar
```

## File Structure

```
.
├── addons/          # Optional addon libraries
├── backends/        # Platform-specific backends
├── bindings/        # Bindings (see hereafter)
├── demos/           # Demonstration programs
├── tests/           # Test programs
├── quick/           # Utility data structures
├── v4p.c, v4p.h     # Core library
├── Makefile         # Build configuration
└── BUILD.md         # This file
```

## Bindings

The `bindings` folder contains a SWIG interface file to integrate V4P in languages like Python or lua. `make -C bindings help` for info.

## Best Practices

1. **Always use verbose mode** when developing: `make V=1`
2. **Use debug builds** during development: `make DEBUG=1`
3. **Clean builds** when switching backends: `make clean && make BACKEND=new_backend`
4. **Check compiler warnings** - V4P uses strict compilation flags
5. **Use AddressSanitizer** for memory debugging: `make DEBUG=1 ASAN=1`
