Here are few things which could be done in future releases.

## new backends ##
  1. frame buffer backend
  1. directFB backend
  1. etc.

## targets ##
  1. Nintendo DS (homebrew)
  1. Android

## V4P based libraries and frameworks ##
  1. transform V4P+LUA experiment into a browser plugin.
  1. use V4P as a basis for a full animated GUI stack

## new features ##
### anti-aliasing ###
A new span-drawing function may handle it.
### curves ###
We need facilities to convert curves into vectors.
### import/export ###
A real SVG parser would be handy.
### deeper z range, transparent polygons ###
Only 16 layers for now. A balanced binary tree of spans may remote this limitation and allow polygons combining (transparency and such).
### accurate math routines ###
Byte-length values are really inaccurate!
### dirty rectangles ###
A library on top of V4P may avoid unchanged scene area rendering.