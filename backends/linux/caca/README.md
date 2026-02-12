This backend provides support for displaying graphics through the libcaca
library, which handles the rendering of graphical content to ASCII art format suitable
for terminal display.

The backend uses bitmap dithering for high-quality rendering:
- 8-bit framebuffer stores V4P palette indices efficiently
- Dithering with V4P's 256-color palette
- Full terminal size display with block characters for better resolution
- Optimized rendering using memset for slice operations

WARNING: No Key Release in libcaca + ncurses : not compatible with G4P interactive demos/apps

Key Release Workaround: The g4pi layer implements automatic key release by tracking the last pressed key and releasing it after one poll cycle. This provides basic key release functionality for simple interactive applications.
