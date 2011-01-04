/* SWIG interface file, see http://www.swig.org/ */

%module v4p
%{
#include "v4pi.h"
%}
%apply int { Coord }; 
%apply unsigned { Color,UInt16,PolygonProps,Flags,ILayer,ICollide }; 

%include "v4p_ll.h"
%include "v4pi.h"
%include "v4p.h"

// SWIG1.x doesn't accept casted litterals
#undef standard
#undef complement
#undef invisible
#undef translucent
#undef absolute
#undef relative
#undef V4P_DISABLED
#undef V4P_IN_DISABLED
#undef V4P_CHANGED
#undef JUMP_COORD
%constant unsigned standard=0;
%constant unsigned complement=1;
%constant unsigned invisible=2;
%constant unsigned translucent=4;
%constant unsigned absolute=0;
%constant unsigned relative=16;
%constant unsigned V4P_DISABLED=32;
%constant unsigned V4P_IN_DISABLED=64;
%constant unsigned V4P_CHANGED=128;
%constant int JUMP_COORD=0;
