/* SWIG interface file, see http://www.swig.org/ */

%module v4p
%{
#include "v4pi.h"
%}
%apply int { V4pCoord }; 
%apply unsigned { V4pColor,UInt32,V4pProps,V4pFlag,V4pLayer,V4PCollisionMask,V4pCollisionLayer }; 

%include "v4p_ll.h"
%include "v4pi.h"
%include "v4p.h"

// SWIG1.x doesn't accept casted litterals
#undef V4P_STANDARD
#undef V4P_HIDDEN
#undef V4P_TRANSLUCENT
#undef V4P_ABSOLUTE
#undef V4P_RELATIVE
#undef V4P_DISABLED
#undef V4P_IN_DISABLED
#undef V4P_CHANGED
#undef V4P_NIL
%constant unsigned V4P_STANDARD=0;
%constant unsigned V4P_HIDDEN=2;
%constant unsigned V4P_TRANSLUCENT=4;
%constant unsigned V4P_ABSOLUTE=0;
%constant unsigned V4P_RELATIVE=16;
%constant unsigned V4P_DISABLED=32;
%constant unsigned V4P_IN_DISABLED=64;
%constant unsigned V4P_CHANGED=128;
%constant int V4P_NIL=0;
