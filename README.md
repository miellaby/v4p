
## V4P - Vectors For Pocket - : a minimalistic layered polygons scan-conversion engine.

## Demo Video

https://www.youtube.com/watch?v=W7DSjBT2Jfk

### Introduction

V4P is a ridiculously short piece of C code. Its main engine weights ~1000 instructions.

V4P targets very light platforms without any advanced hardware resources.

V4P may be easily embedeed in any GUI or Game development library or application.

V4P has no technical dependencies. It even brings its own integer-only maths routines.

To integrate V4P in your projet, just code the horizontal line drawing function. A memset()-like function in a video buffer will do the job in most cases.

Beside rendering vectorized graphic scene, V4P may help you detect collisions (WIP).

### How to use

V4P is provided as a set of C files that you should directly add to your project.

### Technical overview

The underlying algorithm of V4P may be sumed up:*Bresenham-like iterative scanline and active edge cross-over computation based polygon scan conversion algorithm*
 
Additionaly,
* most V4P sorted lists are quick-sorted.
* Bit-based computation helps V4P to find collides and the top polygon under every pixel.
* V4P is very memory efficient. It doesn't relay on a Z-buffer, not even an "S-buffer" (as depicted here: http://www.gamedev.net/reference/articles/article668.asp).

V4P was developped with Palm OS 4 gcc based SDK and tested on an ancient PDA . It has been proven to be adaptable after few tweak on any embedeed linux devices.

###  What V4P can't do
* anti-aliasing (a naive implementation through down-scaling)
* transparent polygons (needs a deep change in the pixel parent election code)
* curves
* deep z depth range (only 16 layers for now because of bit-based operations)
* accurate cos/sin/atan/dist routines
* scene partial refresh and related optimisations

### Palm OS 4 specific info
makefiles have been written to deal with Palm OS 4 gcc based compilation.

Palmphi projects are provided. Palmphi was an IDE running on Windows with cygwin so to simplify the authoring of Palm-OS based applications. See: http://www.digital-kingdoms.net/palmphi/

* palmphi-toto : A V4P based top-view car game (WIP).
* palmphi-totoe : A V4P based graphic editor.

To compile these 2 projects, put the content of "src" directory into the path /usr/src/v4p/ of your cygwin distribution.

Contact: sylvain.garden@gmail.com
