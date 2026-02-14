#!/bin/bash

# V4P Automated Screenshot Capture v2
# Handles blocking executables properly
# Usage: ./autocapture_screenshot_v2.sh demo_name output_file [delay_seconds]

set -e

# Check dependencies
check_dependencies() {
    local missing=()
    
    for cmd in xdotool xwininfo xwd timeout; do
        if ! command -v "$cmd" &> /dev/null; then
            missing+=("$cmd")
        fi
    done
    
    if [ ${#missing[@]} -gt 0 ]; then
        echo "Error: Missing dependencies: ${missing[*]}"
        echo "On Debian/Ubuntu, install with: sudo apt-get install xdotool x11-utils coreutils"
        exit 1
    fi
}

# Run demo in background and get its window
run_demo_and_get_window() {
    local demo_name="$1"
    local demo_path="demos/${demo_name}"
    
    # Run demo in background using timeout to limit execution
    # We'll kill it after capture anyway
    # Redirect stdout/stderr to /dev/null to avoid mixing with our output
    timeout 10s "$demo_path" >/dev/null 2>&1 &
    local demo_pid=$!
    
    # Wait for window to appear
    local window_id=""
    
    # Wait a moment for the window to appear
    sleep 1
    
    # Get the most recently created V4P window
    window_id=$(xdotool search --name "$demo_name" 2>/dev/null | tail -n 1)
    
    if [ -z "$window_id" ]; then
        echo "Error: Could not find window"
        kill $demo_pid 2>/dev/null || true
        exit 0
    fi

    echo "$demo_pid:$window_id"
}

# Capture screenshot
capture_screenshot() {
    local window_id="$1"
    local output_file="$2"
    
    echo "Capturing screenshot of window $window_id..."
    
    # Use xwd to capture the window
    xwd -id "$window_id" -out "$output_file.xwd"
    
    if [ $? -eq 0 ]; then
        echo "Screenshot captured: $output_file.xwd"
        
        # Convert to PNG if possible
        if command -v convert &> /dev/null; then
            convert "$output_file.xwd" "$output_file.png"
            echo "PNG version saved: $output_file.png"
            rm "$output_file.xwd"  # Remove intermediate file
            echo "✓ Screenshot saved to: $output_file.png"
        else
            echo "✓ Screenshot saved to: $output_file.xwd"
            echo "Install ImageMagick to get PNG output: sudo apt-get install imagemagick"
        fi
    else
        echo "Error: Failed to capture screenshot"
        return 1
    fi
}

# Clean up
cleanup() {
    local demo_pid="$1"
    
    echo "Cleaning up..."
    
    # Try to kill the demo process gently first
    kill $demo_pid 2>/dev/null || true
    
    # Wait a bit for it to terminate
    sleep 0.2
    
    # Force kill if still running
    kill -9 $demo_pid 2>/dev/null || true
    
    echo "✓ Demo process terminated"
}

# Main function
main() {
    check_dependencies
    
    if [ $# -lt 2 ]; then
        echo "Usage: $0 demo_name output_file [delay_seconds]"
        echo "Example: $0 square screenshots/square"
        exit 1
    fi
    
    local demo_name="$1"
    local output_file="$2"
    local delay_seconds="${3:-2}"  # Default 2 seconds delay
    
    # Create output directory if needed
    mkdir -p "$(dirname "$output_file")"
    
    echo "V4P Automated Screenshot Capture v2"
    echo "===================================="
    echo "Demo: $demo_name"
    echo "Output: $output_file"
    echo "Delay: $delay_seconds seconds"
    echo ""
    
    # Run demo and get window info
    local process_info=$(run_demo_and_get_window "$demo_name")
    local demo_pid=$(echo "$process_info" | cut -d':' -f1)
    local window_id=$(echo "$process_info" | cut -d':' -f2)
    
    # Wait for the specified delay (let demo initialize)
    echo "Waiting $delay_seconds seconds for demo $demo_name ($demo_pid on window $window_id) to initialize..."
    sleep "$delay_seconds"
    
    # Capture screenshot
    capture_screenshot "$window_id" "$output_file"
    
    # Clean up
    cleanup "$demo_pid"
    
    echo ""
    echo "✓ Screenshot capture complete!"
}

main "$@"