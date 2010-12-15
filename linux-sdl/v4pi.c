/* V4P Implementation for Linux + SDL
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <SDL/SDL.h>

#include "v4pi.h"
#include "lowmath.h"

/* A 256 color system palette inspired from old Palm Computing Devices.
**
*/
static SDL_Color
  palette[256] =
  {
    {255, 255,255, 0}, {255, 204,255, 0}, {255, 153,255, 0}, {255, 102,255, 0},
    {255,  51,255, 0}, {255,   0,255, 0}, {255, 255,204, 0}, {255, 204,204, 0},
    {255, 153,204, 0}, {255, 102,204, 0}, {255,  51,204, 0}, {255,   0,204, 0},
    {255, 255,153, 0}, {255, 204,153, 0}, {255, 153,153, 0}, {255, 102,153, 0},
    {255,  51,153, 0}, {255,   0,153, 0}, {204, 255,255, 0}, {204, 204,255, 0},
    {204, 153,255, 0}, {204, 102,255, 0}, {204,  51,255, 0}, {204,   0,255, 0},
    {204, 255,204, 0}, {204, 204,204, 0}, {204, 153,204, 0}, {204, 102,204, 0},
    {204,  51,204, 0}, {204,   0,204, 0}, {204, 255,153, 0}, {204, 204,153, 0},
    {204, 153,153, 0}, {204, 102,153, 0}, {204,  51,153, 0}, {204,   0,153, 0},
    {153, 255,255, 0}, {153, 204,255, 0}, {153, 153,255, 0}, {153, 102,255, 0},
    {153,  51,255, 0}, {153,   0,255, 0}, {153, 255,204, 0}, {153, 204,204, 0},
    {153, 153,204, 0}, {153, 102,204, 0}, {153,  51,204, 0}, {153,   0,204, 0},
    {153, 255,153, 0}, {153, 204,153, 0}, {153, 153,153, 0}, {153, 102,153, 0},
    {153,  51,153, 0}, {153,   0,153, 0}, {102, 255,255, 0}, {102, 204,255, 0},
    {102, 153,255, 0}, {102, 102,255, 0}, {102,  51,255, 0}, {102,   0,255, 0},
    {102, 255,204, 0}, {102, 204,204, 0}, {102, 153,204, 0}, {102, 102,204, 0},
    {102,  51,204, 0}, {102,   0,204, 0}, {102, 255,153, 0}, {102, 204,153, 0},
    {102, 153,153, 0}, {102, 102,153, 0}, {102,  51,153, 0}, {102,   0,153, 0},
    { 51, 255,255, 0}, { 51, 204,255, 0}, { 51, 153,255, 0}, { 51, 102,255, 0},
    { 51,  51,255, 0}, { 51,   0,255, 0}, { 51, 255,204, 0}, { 51, 204,204, 0},
    { 51, 153,204, 0}, { 51, 102,204, 0}, { 51,  51,204, 0}, { 51,   0,204, 0},
    { 51, 255,153, 0}, { 51, 204,153, 0}, { 51, 153,153, 0}, { 51, 102,153, 0},
    { 51,  51,153, 0}, { 51,   0,153, 0}, {  0, 255,255, 0}, {  0, 204,255, 0},
    {  0, 153,255, 0}, {  0, 102,255, 0}, {  0,  51,255, 0}, {  0,   0,255, 0},
    {  0, 255,204, 0}, {  0, 204,204, 0}, {  0, 153,204, 0}, {  0, 102,204, 0},
    {  0,  51,204, 0}, {  0,   0,204, 0}, {  0, 255,153, 0}, {  0, 204,153, 0},
    {  0, 153,153, 0}, {  0, 102,153, 0}, {  0,  51,153, 0}, {  0,   0,153, 0},
    {255, 255,102, 0}, {255, 204,102, 0}, {255, 153,102, 0}, {255, 102,102, 0},
    {255,  51,102, 0}, {255,   0,102, 0}, {255, 255, 51, 0}, {255, 204, 51, 0},
    {255, 153, 51, 0}, {255, 102, 51, 0}, {255,  51, 51, 0}, {255,   0, 51, 0},
    {255, 255,  0, 0}, {255, 204,  0, 0}, {255, 153,  0, 0}, {255, 102,  0, 0},
    {255,  51,  0, 0}, {255,   0,  0, 0}, {204, 255,102, 0}, {204, 204,102, 0},
    {204, 153,102, 0}, {204, 102,102, 0}, {204,  51,102, 0}, {204,   0,102, 0},
    {204, 255, 51, 0}, {204, 204, 51, 0}, {204, 153, 51, 0}, {204, 102, 51, 0},
    {204,  51, 51, 0}, {204,   0, 51, 0}, {204, 255,  0, 0}, {204, 204,  0, 0},
    {204, 153,  0, 0}, {204, 102,  0, 0}, {204,  51,  0, 0}, {204,   0,  0, 0},
    {153, 255,102, 0}, {153, 204,102, 0}, {153, 153,102, 0}, {153, 102,102, 0},
    {153,  51,102, 0}, {153,   0,102, 0}, {153, 255, 51, 0}, {153, 204, 51, 0},
    {153, 153, 51, 0}, {153, 102, 51, 0}, {153,  51, 51, 0}, {153,   0, 51, 0},
    {153, 255,  0, 0}, {153, 204,  0, 0}, {153, 153,  0, 0}, {153, 102,  0, 0},
    {153,  51,  0, 0}, {153,   0,  0, 0}, {102, 255,102, 0}, {102, 204,102, 0},
    {102, 153,102, 0}, {102, 102,102, 0}, {102,  51,102, 0}, {102,   0,102, 0},
    {102, 255, 51, 0}, {102, 204, 51, 0}, {102, 153, 51, 0}, {102, 102, 51, 0},
    {102,  51, 51, 0}, {102,   0, 51, 0}, {102, 255,  0, 0}, {102, 204,  0, 0},
    {102, 153,  0, 0}, {102, 102,  0, 0}, {102,  51,  0, 0}, {102,   0,  0, 0},
    { 51, 255,102, 0}, { 51, 204,102, 0}, { 51, 153,102, 0}, { 51, 102,102, 0},
    { 51,  51,102, 0}, { 51,   0,102, 0}, { 51, 255, 51, 0}, { 51, 204, 51, 0},
    { 51, 153, 51, 0}, { 51, 102, 51, 0}, { 51,  51, 51, 0}, { 51,   0, 51, 0},
    { 51, 255,  0, 0}, { 51, 204,  0, 0}, { 51, 153,  0, 0}, { 51, 102,  0, 0},
    { 51,  51,  0, 0}, { 51,   0,  0, 0}, {  0, 255,102, 0}, {  0, 204,102, 0},
    {  0, 153,102, 0}, {  0, 102,102, 0}, {  0,  51,102, 0}, {  0,   0,102, 0},
    {  0, 255, 51, 0}, {  0, 204, 51, 0}, {  0, 153, 51, 0}, {  0, 102, 51, 0},
    {  0,  51, 51, 0}, {  0,   0, 51, 0}, {  0, 255,  0, 0}, {  0, 204,  0, 0},
    {  0, 153,  0, 0}, {  0, 102,  0, 0}, {  0,  51,  0, 0}, { 17,  17, 17, 0},
    { 34,  34, 34, 0}, { 68,  68, 68, 0}, { 85,  85, 85, 0}, {119, 119,119, 0},
    {136, 136,136, 0}, {170, 170,170, 0}, {187, 187,187, 0}, {221, 221,221, 0},
    {238, 238,238, 0}, {192, 192,192, 0}, {128,   0,  0, 0}, {128,   0,128, 0},
    {  0, 128,  0, 0}, {  0, 128,128, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0},
    {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0},
    {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0},
    {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0},
    {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0},
    {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0},
    {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}, {  0,   0,  0, 0}
  };

const Color
   gray=225, marron=226, purple=227, green=228, cyan=229,
   black=215, red=125, blue=95, yellow=120, dark=217, oliver=58,
   fluo=48;

const Coord defaultScreenWidth = 640, defaultScreenHeight = 400;


typedef struct v4pDisplay_s {
  SDL_Surface* screenSurface;
  unsigned int width;
  unsigned int height;
} V4pDisplayS;

//struct v4pDisplay_s* V4pDisplayP;

V4pDisplayS v4pDisplayDefaultContextS;
V4pDisplayP v4pDisplayDefaultContext = &v4pDisplayDefaultContextS;

V4pDisplayP v4pDisplayContext = &v4pDisplayDefaultContextS;
Coord       v4pDisplayWidth;
Coord       v4pDisplayHeight;

static Coord bytesBetweenLines;
static const SDL_VideoInfo* vi;
static Uint8* currentBuffer;
static int iBuffer;

typedef struct collide_s {
   Coord x ;
   Coord y ;
   UInt16 q ;
   PolygonP poly ;
} Collide ;

Collide collides[16] ;

void v4pDisplayDebug(char *formatString, ...)
{ va_list args ; char text[0x100] ;
  va_start(args, formatString) ;
  //vprintf(formatString, args) ;
  va_end(args);
}

Boolean v4pDisplayError(char *formatString, ...) {
  va_list args ;
  va_start(args, formatString) ;
  vfprintf(stderr, formatString, args) ;
  va_end(args);
}

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

static Uint32 t1 ;
static Uint32 laps[4] = {0,0,0,0}, tlaps=0 ;

Boolean v4pDisplayStart() {
  int i ;
  t1 = SDL_GetTicks();
  iBuffer = 0;

  //Init collides
  for (i = 0 ; i < 16 ; i++) {
    collides[i].q = 0 ;
    collides[i].x = 0 ;
    collides[i].y = 0 ;
    collides[i].poly = NULL ;
  }

  if (SDL_MUSTLOCK(v4pDisplayContext->screenSurface) && SDL_LockSurface(v4pDisplayContext->screenSurface) < 0)
    return failure;
  else
    return success ;
}

Boolean v4pDisplayEnd() {

   int i ;
   static int j=0;
   Uint32 t2=SDL_GetTicks();
   tlaps-=laps[j % 4];
   tlaps+=laps[j % 4]=t2-t1;
   j++;
   v4pDisplayDebug("%d", tlaps);

   // Bilan des collides
   for (i = 0 ; i < 16 ; i++) {
      if (!collides[i].q) continue ;
      collides[i].x /= collides[i].q ;
      collides[i].y /= collides[i].q ;
   }

   if (SDL_MUSTLOCK(v4pDisplayContext->screenSurface))
     SDL_UnlockSurface(v4pDisplayContext->screenSurface);
   SDL_Flip(v4pDisplayContext->screenSurface);

   return success ;
}

Boolean v4pDisplaySlice(Coord y, Coord x0, Coord x1, Color c) {
 int l = x1 - x0;
 if (l <= 0) return success ;
 
 //WinSetForeColor((IndexedColorType)c);
 //WinDrawLine(x0 + 10, y+10, x1+9, y+10);
 SDL_memset(&currentBuffer[iBuffer], (char)c, l) ;
 iBuffer+= l ;
// if (!x0 && y)
//    iBuffer+= bytesBetweenLines ;

 return success ;
}

Boolean v4pDisplayInit(int quality, Boolean fullscreen) {
  /* Initialisation de SDL */
  int screenWidth = defaultScreenWidth * 2 / (3 - quality);
  int screenHeight = defaultScreenHeight * 2 / (3 - quality);

  if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
	fprintf(stderr,"Erreur SDL: %s\n", SDL_GetError());
	exit(1);
  }

  vi = SDL_GetVideoInfo();

  /* Initialise un mode vidéo idéal pour cette image */
  v4pDisplayDefaultContextS.screenSurface = SDL_SetVideoMode(screenWidth, screenHeight, 
                                   8, (fullscreen ? SDL_FULLSCREEN : 0) |SDL_HWSURFACE);


  SDL_SetColors(v4pDisplayDefaultContextS.screenSurface, palette, 0, 256);

  v4pDisplayDefaultContextS.width = screenWidth;
  v4pDisplayDefaultContextS.height = screenHeight;
  v4pDisplaySetContext(v4pDisplayDefaultContext);
}

V4pDisplayP v4pDisplayNewContext(int width, int height) {
  V4pDisplayP c = (V4pDisplayP)malloc(sizeof(V4pDisplayS)); 
  if (!c) return 0 ;

  c->width = width;
  c->height = height;
  c->screenSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,8,0,0,0,0);
  return c;
}

void v4pDisplayFreeContext(V4pDisplayP c) {
  if (!c || c == v4pDisplayDefaultContext)
    return;

  SDL_FreeSurface(c->screenSurface);
  free(c);
  if (v4pDisplayContext == c)
    v4pDisplayContext = v4pDisplayDefaultContext;
}

V4pDisplayP v4pDisplaySetContext(V4pDisplayP c) {
  v4pDisplayContext = c;
  v4pDisplayWidth = c->width;
  v4pDisplayHeight = c->height;
  currentBuffer = c->screenSurface->pixels;
  bytesBetweenLines = 0;
  return c;
}

void v4pDisplayQuit() {
  SDL_Quit();
}
