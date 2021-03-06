/* V4P Implementation for Linux + SDL
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/times.h>
#include <vga.h>

#include "v4pi.h"
#include "lowmath.h"

/* A 256 color system palette inspired from old Palm Computing Devices.
** TODO not working
*/
static int
  palette[256 * 3] =
  {
    255, 255,255, 255, 204,255, 255, 153,255, 255, 102,255,
    255,  51,255, 255,   0,255, 255, 255,204, 255, 204,204,
    255, 153,204, 255, 102,204, 255,  51,204, 255,   0,204,
    255, 255,153, 255, 204,153, 255, 153,153, 255, 102,153,
    255,  51,153, 255,   0,153, 204, 255,255, 204, 204,255,
    204, 153,255, 204, 102,255, 204,  51,255, 204,   0,255,
    204, 255,204, 204, 204,204, 204, 153,204, 204, 102,204,
    204,  51,204, 204,   0,204, 204, 255,153, 204, 204,153,
    204, 153,153, 204, 102,153, 204,  51,153, 204,   0,153,
    153, 255,255, 153, 204,255, 153, 153,255, 153, 102,255,
    153,  51,255, 153,   0,255, 153, 255,204, 153, 204,204,
    153, 153,204, 153, 102,204, 153,  51,204, 153,   0,204,
    153, 255,153, 153, 204,153, 153, 153,153, 153, 102,153,
    153,  51,153, 153,   0,153, 102, 255,255, 102, 204,255,
    102, 153,255, 102, 102,255, 102,  51,255, 102,   0,255,
    102, 255,204, 102, 204,204, 102, 153,204, 102, 102,204,
    102,  51,204, 102,   0,204, 102, 255,153, 102, 204,153,
    102, 153,153, 102, 102,153, 102,  51,153, 102,   0,153,
     51, 255,255,  51, 204,255,  51, 153,255,  51, 102,255,
     51,  51,255,  51,   0,255,  51, 255,204,  51, 204,204,
     51, 153,204,  51, 102,204,  51,  51,204,  51,   0,204,
     51, 255,153,  51, 204,153,  51, 153,153,  51, 102,153,
     51,  51,153,  51,   0,153,   0, 255,255,   0, 204,255,
      0, 153,255,   0, 102,255,   0,  51,255,   0,   0,255,
      0, 255,204,   0, 204,204,   0, 153,204,   0, 102,204,
      0,  51,204,   0,   0,204,   0, 255,153,   0, 204,153,
      0, 153,153,   0, 102,153,   0,  51,153,   0,   0,153,
    255, 255,102, 255, 204,102, 255, 153,102, 255, 102,102,
    255,  51,102, 255,   0,102, 255, 255, 51, 255, 204, 51,
    255, 153, 51, 255, 102, 51, 255,  51, 51, 255,   0, 51,
    255, 255,  0, 255, 204,  0, 255, 153,  0, 255, 102,  0,
    255,  51,  0, 255,   0,  0, 204, 255,102, 204, 204,102,
    204, 153,102, 204, 102,102, 204,  51,102, 204,   0,102,
    204, 255, 51, 204, 204, 51, 204, 153, 51, 204, 102, 51,
    204,  51, 51, 204,   0, 51, 204, 255,  0, 204, 204,  0,
    204, 153,  0, 204, 102,  0, 204,  51,  0, 204,   0,  0,
    153, 255,102, 153, 204,102, 153, 153,102, 153, 102,102,
    153,  51,102, 153,   0,102, 153, 255, 51, 153, 204, 51,
    153, 153, 51, 153, 102, 51, 153,  51, 51, 153,   0, 51,
    153, 255,  0, 153, 204,  0, 153, 153,  0, 153, 102,  0,
    153,  51,  0, 153,   0,  0, 102, 255,102, 102, 204,102,
    102, 153,102, 102, 102,102, 102,  51,102, 102,   0,102,
    102, 255, 51, 102, 204, 51, 102, 153, 51, 102, 102, 51,
    102,  51, 51, 102,   0, 51, 102, 255,  0, 102, 204,  0,
    102, 153,  0, 102, 102,  0, 102,  51,  0, 102,   0,  0,
     51, 255,102,  51, 204,102,  51, 153,102,  51, 102,102,
     51,  51,102,  51,   0,102,  51, 255, 51,  51, 204, 51,
     51, 153, 51,  51, 102, 51,  51,  51, 51,  51,   0, 51,
     51, 255,  0,  51, 204,  0,  51, 153,  0,  51, 102,  0,
     51,  51,  0,  51,   0,  0,   0, 255,102,   0, 204,102,
      0, 153,102,   0, 102,102,   0,  51,102,   0,   0,102,
      0, 255, 51,   0, 204, 51,   0, 153, 51,   0, 102, 51,
      0,  51, 51,   0,   0, 51,   0, 255,  0,   0, 204,  0,
      0, 153,  0,   0, 102,  0,   0,  51,  0,  17,  17, 17,
     34,  34, 34,  68,  68, 68,  85,  85, 85, 119, 119,119,
    136, 136,136, 170, 170,170, 187, 187,187, 221, 221,221,
    238, 238,238, 192, 192,192, 128,   0,  0, 128,   0,128,
      0, 128,  0,   0, 128,128,   0,   0,  0,   0,   0,  0,
      0,   0,  0,   0,   0,  0,   0,   0,  0,   0,   0,  0,
      0,   0,  0,   0,   0,  0,   0,   0,  0,   0,   0,  0,
      0,   0,  0,   0,   0,  0,   0,   0,  0,   0,   0,  0,
      0,   0,  0,   0,   0,  0,   0,   0,  0,   0,   0,  0,
      0,   0,  0,   0,   0,  0,   0,   0,  0,   0,   0,  0,
      0,   0,  0,   0,   0,  0,   0,   0,  0,   0,   0,  0
  };

// Some constants linking to basic colors;
const Color
   gray=225, marron=226, purple=227, green=228, cyan=229,
   black=215, red=125, blue=95, yellow=120, dark=217, oliver=58,
   fluo=48;

// Default window/screen width & heigth
const Coord defaultScreenWidth = 640, defaultScreenHeight = 480;

// A display context
typedef struct v4pDisplay_s {
    // TODO manage offscreen buffer
  unsigned int width;
  unsigned int height;
} V4pDisplayS;

// Global variable hosting the default V4P contex
V4pDisplayS v4pDisplayDefaultContextS;
V4pDisplayP v4pDisplayDefaultContext = &v4pDisplayDefaultContextS;

// Variables hosting current context and related properties
V4pDisplayP   v4pDisplayContext = &v4pDisplayDefaultContextS;
Coord         v4pDisplayWidth;
Coord         v4pDisplayHeight;


/***************************/
/* collide computing stuff */
/***************************/
typedef struct collide_s {
   Coord x ;
   Coord y ;
   UInt16 q ;
   PolygonP poly ;
} Collide ;

Collide collides[16] ;

/*****************/
/* metrics stuff */
/*****************/
static Int32 t1;
static Int32 laps[4] = {0, 0, 0, 0};
static Int32 tlaps=0;

static Int32 getTicks() {
    static struct tms buf;
    static int clk_ticks = 0;
    if (!clk_ticks) {
        clk_ticks = sysconf(_SC_CLK_TCK);
    }

    Int32 t = times(&buf) * 1000 / clk_ticks;
    return t;
}


// debug logging helper
static FILE* traceFile = NULL;
#ifdef DEBUG
static void initTraceFile() {
   if (traceFile) return;
   char buffer[255];
   snprintf(buffer, 255, "/tmp/traceV4PI.%d", getpid());
   printf("Trace will go into %s\n", buffer);
   traceFile = fopen(buffer, "a");
}

void v4pDisplayDebug(char *formatString, ...)
{ va_list args ;
  va_start(args, formatString) ;
  initTraceFile();
  vfprintf(traceFile, formatString, args) ;
  va_end(args);
}
#endif

// error logging helper
Boolean v4pDisplayError(char *formatString, ...) {
  va_list args ;
  va_start(args, formatString) ;
  initTraceFile();
  vfprintf(traceFile, formatString, args) ;
  va_end(args);

  return success;
}

// record collides
Boolean v4pDisplayCollide(ICollide i1, ICollide i2, Coord py, Coord x1, Coord x2, PolygonP p1, PolygonP p2) {
 int l, dx, dy ;
 l = x2 - x1 ;
 dx = x1 * l + (l + 1) * l / 2 ;
 dy = l * py ;
 collides[i1].q += l ;
 collides[i1].x += dx ;
 collides[i1].y += dy ;
 collides[i1].poly = p2 ;
 collides[i2].q += l ;
 collides[i2].x += dx ;
 collides[i2].y += dy ;
 collides[i2].poly = p1 ;
 return success ;
}

// prepare things before V4P engine scanline loop
Boolean v4pDisplayStart() {
  // remember start time
  t1 = getTicks();

  //Init collides
  int i;
  for (i = 0; i < 16; i++) {
    collides[i].q = 0;
    collides[i].x = 0;
    collides[i].y = 0;
    collides[i].poly = NULL;
  }

  return success;
}

// finalize things after V4P engine scanline loop
Boolean v4pDisplayEnd() {
   int i ;
   static int j = 0;

   // Get end time and compute average rendering time
   UInt32 t2 = getTicks();
   tlaps -= laps[j % 4];
   tlaps += laps[j % 4] = t2 - t1;
   j++;
   if (!(j % 100)) v4pDisplayDebug("v4pDisplayEnd, average time = %dms\n", tlaps / 4);

   // sumarize collides
   for (i = 0 ; i < 16 ; i++) {
      if (!collides[i].q) continue ;
      collides[i].x /= collides[i].q ;
      collides[i].y /= collides[i].q ;
   }

   return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pDisplaySlice(Coord y, Coord x0, Coord x1, Color c) {
 int l = x1 - x0;
 if (l <= 0) return success;
 vga_setcolor(c);
 vga_drawline(x0, y, x1, y);
 return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pDisplayInit(int quality, Boolean fullscreen) {
  // Initialize Svgalib
   vga_init();

  // Set up a suitable video mode depending on wanted quality
  int screenWidth = defaultScreenWidth * 2 / (3 - quality);
  int screenHeight = defaultScreenHeight * 2 / (3 - quality);
v4pDisplayDebug("v4pDisplayInit, desired resolution is %dx%d\n", screenWidth, screenHeight);
  vga_setmode(screenWidth == 640 && screenHeight == 480
              ? G640x480x256
              : (screenWidth > 640 ? G800x600x256 : G320x200x256 ));


  // Set a default ugly but portable 256 palette 8 bits pixel
  vga_setpalvec(0, 256, palette);

  // The default context holds the main screen/window
  // TODO get actual screen geometry
  vga_modeinfo *i = vga_getmodeinfo(vga_getcurrentmode());

  v4pDisplayDefaultContextS.width = i->width;
  v4pDisplayDefaultContextS.height = i->height;
  v4pDisplayDebug("Default Context Width x height = %d x %d\n", v4pDisplayDefaultContextS.width, v4pDisplayDefaultContextS.height);
  v4pDisplaySetContext(v4pDisplayDefaultContext);

  return success;
}

// Create a new buffer-like V4P context
V4pDisplayP v4pDisplayNewContext(int width, int height) {
  V4pDisplayP c = (V4pDisplayP)malloc(sizeof(V4pDisplayS));
  if (!c) return NULL;
  // TODO offscren buffer
  c->width = width;
  c->height = height;
  return c;
}

// free a V4P context
void v4pDisplayFreeContext(V4pDisplayP c) {
  if (!c || c == v4pDisplayDefaultContext)
    return;

  free(c);

  // One can't let a pointer to a freed context.
  if (v4pDisplayContext == c)
    v4pDisplayContext = v4pDisplayDefaultContext;
}

// Change the current V4P context
V4pDisplayP v4pDisplaySetContext(V4pDisplayP c) {
  v4pDisplayContext = c;
  v4pDisplayWidth = c->width;
  v4pDisplayHeight = c->height;
  return c;
}

// clean things before quitting
void v4pDisplayQuit() {
  vga_setmode(TEXT);
  v4pDisplayDebug("w=%d h=%d\n", v4pDisplayDefaultContextS.width, v4pDisplayDefaultContextS.height);
}
