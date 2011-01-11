#include <directfb.h>
#include "v4pi.h"

typedef struct v4pDisplay_s {
  IDirectFBSurface *surface;
  unsigned int width;
  unsigned int height;
} V4pDisplayS;


IDirectFBSurface*  currentSurface;
