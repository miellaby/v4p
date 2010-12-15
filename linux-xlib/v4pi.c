/* V4P Implementation for Linux + SDL
**
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/times.h>

#include <X11/Xutil.h>
#include "_v4pi.h"
#include "lowmath.h"

/* A 256 color system palette inspired from old Palm Computing Devices.
**
*/
static struct s_color {
	unsigned short r;
	unsigned short g;
	unsigned short b;
	unsigned short youDontWantToKnow; }
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

static char* applicationName = "v4pX";
static char* applicationClass = "V4pX";
static char* fakeArgv[] = { "v4pX" };

static unsigned long xColorsTab[256];
const Color
   gray=225, marron=226, purple=227, green=228, cyan=229,
   black=215, red=125, blue=95, yellow=120, dark=217, oliver=58,
   fluo=48;

Coord defaultScreenWidth = 640, defaultScreenHeight = 400;

static const int borderWidth=1;


//struct v4pDisplay_s* V4pDisplayP;

V4pDisplayS v4pDisplayDefaultContextS;
V4pDisplayP v4pDisplayDefaultContext = &v4pDisplayDefaultContextS;

V4pDisplayP v4pDisplayContext = &v4pDisplayDefaultContextS;
Coord       v4pDisplayWidth;
Coord       v4pDisplayHeight;

// Data buffer
static XGCValues values;
static struct tms tmsBuffer;

typedef struct collide_s {
    Coord x ;
    Coord y ;
    UInt16 q ;
    PolygonP poly ;
} Collide ;

Collide collides[16] ;

void v4pDisplayDebug(char *formatString, ...) {
    va_list args ; char text[0x100] ;
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

static clock_t t1 ;
static clock_t laps[4] = {0,0,0,0}, tlaps=0 ;

Boolean v4pDisplayStart() {
  int i ;
  
  t1 = times(&tmsBuffer); 

  //Init collides
  for (i = 0 ; i < 16 ; i++) {
    collides[i].q = 0 ;
    collides[i].x = 0 ;
    collides[i].y = 0 ;
    collides[i].poly = NULL ;
  }

  return success ;
}

Boolean v4pDisplayEnd() {

    int i ;
    static int j=0;
    clock_t t2 = times(&tmsBuffer);
    tlaps -= laps[j % 4];
    tlaps += laps[j % 4]=t2 - t1;
    j++;
    v4pDisplayDebug("%d", tlaps);

    // Bilan des collides
    for (i = 0 ; i < 16 ; i++) {
       if (!collides[i].q) continue ;
       collides[i].x /= collides[i].q ;
       collides[i].y /= collides[i].q ;
    }

    XFlush(currentDisplay);
      
    return success ;
}

Boolean v4pDisplaySlice(Coord y, Coord x0, Coord x1, Color c) {
    int l = x1 - x0;
    if (l <= 0) return success ;
    XSetForeground(currentDisplay, currentGC, xColorsTab[c]);
    XFillRectangle(currentDisplay, currentWindow, currentGC, x0, y, l, 1);
    return success ;
}

static void createWindow(V4pDisplayP vd, int width, int height) {
	Display* d = vd->d;
	int      s = vd->s;
	Window   w;
    GC       gc;	
    
   /* Miscellaneous X variables */
    XSizeHints *  size_hints;
    XWMHints   *  wm_hints;
    XClassHint *  class_hints;
    XTextProperty windowName, iconName;
    XEvent        report;
  
    w = /* create window */
       XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, borderWidth,
       BlackPixel(d, s), WhitePixel(d, s));


    /* Allocate memory */
    if ( !( size_hints  = XAllocSizeHints() ) || 
	 !( wm_hints    = XAllocWMHints()   ) ||
	 !( class_hints = XAllocClassHint() )    ) {
	v4pDisplayError("couldn't allocate memory.\n");
	exit(EXIT_FAILURE);
    }

    /*  Set hints for window manager */
    if ( XStringListToTextProperty(&applicationName, 1, &windowName) == 0
        || XStringListToTextProperty(&applicationName, 1, &iconName) == 0 ) {
	    v4pDisplayError("xlib structure allocation failed.\n");
	exit(EXIT_FAILURE);
    }

    size_hints->flags       = PPosition | PSize | PMinSize;
    size_hints->min_width   = width;
    size_hints->min_height  = height;

    wm_hints->flags         = StateHint | InputHint;
    wm_hints->initial_state = NormalState;
    wm_hints->input         = True;

    class_hints->res_name   = applicationName;
    class_hints->res_class  = applicationClass;

    XSetWMProperties(d, w, &windowName, &iconName, (char**)&fakeArgv, 0,
		     size_hints, wm_hints, class_hints);

    /*  Choose which events we want to handle  */
    XSelectInput(d, w, ExposureMask | KeyPressMask |
		 ButtonPressMask | ButtonReleaseMask | ButtonMotionMask
         | PointerMotionHintMask |
		 StructureNotifyMask);

    gc = /* create graphic context */
         XCreateGC(d, w, 0, &values);
    XSetForeground(d, gc, BlackPixel(d, s));

    vd->width = width;
    vd->height = height;
    vd->w  = w;
    vd->gc = gc;
    
      
    /*  Display Window  */
    XMapWindow(d, w);
}


Boolean v4pDisplayInit(int quality, Boolean fullscreen) {

   /* connect to X server */
   Display *d = XOpenDisplay(NULL);
   if (d == NULL) {
     v4pDisplayError("Cannot open display\n");
     exit(EXIT_FAILURE);
   }
   int s     = DefaultScreen(d);

   // Prepare "tablette"
   Colormap cmap = DefaultColormap (d, s);
   int i;
   XColor c;
   c.flags = DoRed|DoGreen|DoBlue;

   for (i = 0; i < 256; i++) {
	   c.red = palette[i].r;
	   c.green = palette[i].g;
	   c.blue = palette[i].b;
	   XAllocColor(d, cmap, &c);
	   xColorsTab[i] = c.pixel;
	   printf("%d\n", (int)c.pixel);
   }
   
   v4pDisplayDefaultContextS.d = d;
   v4pDisplayDefaultContextS.s = s;

   /* Get screen size from display structure macro */
   defaultScreenWidth = DisplayWidth(d, s);
   defaultScreenHeight = DisplayHeight(d, s);
   int winWidth = defaultScreenWidth * 2 / (3 - quality);
   int winHeight = defaultScreenHeight * 2 / (3 - quality);
   
   createWindow(v4pDisplayDefaultContext, winWidth, winHeight);

   v4pDisplaySetContext(v4pDisplayDefaultContext);
}

V4pDisplayP v4pDisplayNewContext(int width, int height) {
  V4pDisplayP c = (V4pDisplayP)malloc(sizeof(V4pDisplayS)); 
  if (!c) return 0 ;

  c->d = v4pDisplayDefaultContextS.d;
  c->s = v4pDisplayDefaultContextS.s;
  
  c->width = width;
  c->height = height;

  createWindow(c, width, height); 
  return c;
}

void v4pDisplayFreeContext(V4pDisplayP c) {
  if (!c || c == v4pDisplayDefaultContext)
    return;

  XFreeGC(c->d, c->gc);
  free(c);
  if (v4pDisplayContext == c)
    v4pDisplayContext = v4pDisplayDefaultContext;
}

V4pDisplayP v4pDisplaySetContext(V4pDisplayP c) {
  v4pDisplayContext = c;
  v4pDisplayWidth = c->width;
  v4pDisplayHeight = c->height;
  currentDisplay = c->d;
  currentWindow = c->w;
  currentScreen = c->s;
  currentGC = c->gc;
  return c;
}

void v4pDisplayQuit() {
   /* close connection to server */
   XCloseDisplay(v4pDisplayDefaultContext->d);
}
