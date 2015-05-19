# Introduction #

V4P - Vectors For Pocket - is a minimalistic software-only dependency-free scanline-rendering based 2D graphics engine. Its purpose is to render a 2D image out of a set of layered polygons.

<a href='http://www.youtube.com/watch?feature=player_embedded&v=W7DSjBT2Jfk' target='_blank'><img src='http://img.youtube.com/vi/W7DSjBT2Jfk/0.jpg' width='425' height=344 /></a>

The basis of the algorithm is introduced by Wikipedia here: http://en.wikipedia.org/wiki/Scanline_rendering. This algorithm has been merged with Bresenham and quicksort algorithms, plus bit-based layers management and poor-man integer only mathematics routines picked here and there.

V4P is extra light. It consists in a ridiculously short piece of C code. See by yourself:
```
grep -c ';' *.c
lowmath.c:72
quickheap.c:66
quicktable.c:28
sortable.c:64
v4p.c:519
```

V4P targets very light platforms without any form of hardware acceleration: neither FGU nor FPU. RAM usage is negligible since neither Z-buffer nor "S-buffer" (as depicted here: http://www.gamedev.net/reference/articles/article668.asp) are involved. Plus V4P has no dependency with any other technology. It even brings its own integer only maths routines.

It means you could easily embed V4P in almost any software or hardware. Just provide it with a "span" drawing function. A memset()-like function should work well in most cases. For you information, V4P has been initially developed on an antique PDA.

# Backends #
V4P sources includes few backends: SDL, XLib, and palmOS 4 API (!)

Those are simple backends. One might imagine much more clever ones, with complex color structures, anti-aliasing and such.

# Extra libs #

V4P sources also includes several extra pieces of C code for your pleasure:
  1. a "game engine", that is an "iterate & render" loop with constant frame rate.
  1. a "pixel font renderer" to output scores and debug messages.
  1. a SWIG interface file to build an extension module for your prefered language, says Python.
  1. a working LUA-powered game engine waiting for you to write a nice game leveraging on V4P and LUA abilities.
  1. a minimalist serialization library to quickly define scene out of strings.
  1. a prototype of a SVG path parsing library. It allows to draw assets with an SVG editor and import them in your code.

