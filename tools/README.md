# V4P Automated Screenshot System

## Overview

This system automatically captures screenshots of V4P demos by:
1. Building the demo with Xlib backend
2. Running the demo in the background  
3. Detecting the window automatically
4. Capturing the screenshot
5. Cleaning up the demo process

## Requirements

- `xwd` (X Window Dump) - for capturing windows
- `xdotool` - for window detection
- `timeout` - for process management
- `convert` (optional) - for PNG conversion

Install on Debian/Ubuntu:
```bash
sudo apt-get install x11-utils xdotool coreutils imagemagick
```

## Usage

### Basic Usage

```bash
# Capture screenshot for any demo
make screenshots/demo_name.png

# Examples:
make screenshots/square.png
make screenshots/asteroids.png
make screenshots/stars.png
```

### Custom Delay

By default, the system waits 2 seconds for the demo to initialize. You can specify a different delay:

```bash
# Capture with 5 second delay for complex demos
./tools/autocapture_screenshot.sh demo_name output_file 5
```

### Available Demos

To see what demos are available:
```bash
ls demos/*.c | sed 's|demos/||; s|\.c||g'
```

## How It Works

1. **Build**: Compiles the demo with Xlib backend
2. **Run**: Executes the demo in background with timeout
3. **Detect**: Finds the V4P window automatically
4. **Capture**: Uses `xwd` to capture the window
5. **Convert**: Converts XWD to PNG (if ImageMagick available)
6. **Cleanup**: Terminates the demo process

## Examples

```bash
# Capture square demo
make screenshots/square.png

# Capture multiple demos
make screenshots/square.png screenshots/stars.png screenshots/asteroids.png

# Custom output filename
./tools/autocapture_screenshot.sh square screenshots/my_awesome_demo
```

## Troubleshooting

- **Window not found**: Make sure no other V4P windows are open
- **Permission denied**: Ensure `screenshots/` directory exists and is writable
- **Missing dependencies**: Install the required packages as shown above
- **Demo fails to build**: Check that the demo name is correct
