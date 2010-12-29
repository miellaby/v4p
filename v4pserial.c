#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "v4p.h"
#include "v4pserial.h"

extern int v4pXToD(char c) ;

// add points to a polygon with coordinates decoded from a c-string
PolygonP v4pPolygonDecodePoints(PolygonP p, char *s, int scale) {
   int l, j;
   long xs, ys, xs1, ys1;
   Boolean sep, psep;
   char c ;

   psep = false;
   for (j = 0 ; s[j] ; j++) {
      c = s[j] ;
      if (c == ' ')
         continue ;

      if (c == '.') {
         sep = true;
         if (psep)
            v4pPolygonAddPoint(p, xs1 * (long)scale / 256, ys1 * (long)scale / 256);
         continue;
      }

      sep = false;

      xs = (v4pXToD(c) << 4) + v4pXToD(s[++j]);
      ys = (v4pXToD(s[++j]) << 4) + v4pXToD(s[++j]);
      
      v4pPolygonAddPoint(p, xs * (long)scale / 256, ys * (long)scale / 256);

      if (sep) {
         xs1 = xs ;
         ys1 = ys ;
         psep = true;
      }
  }
  if (psep)
     v4pPolygonAddPoint(p, xs1 * (long)scale / 256, ys1 * (long)scale / 256);

  return p;
}

// create a polygon by adding all points encoded in a c-string
PolygonP v4pQuickPolygon(PolygonProps t, Color col, ILayer z, char* s, int scale) {
  return v4pPolygonDecodePoints(v4pPolygonNew(t, col, z), s, scale);
}

// encode every point of a polygon into a single c-string
char *v4pPolygonEncodePoints(PolygonP p, int scale) {
  static char *t = "0123456789ABCDEF" ;
  char *s;
  PointP s1, m, pm;
  int i, l ;
  Coord v;

  s = (char *)malloc(32 * sizeof(char)) ;
  l = 0;
  s1 = v4pPolygonGetPoints(p) ;
  m = s1;
  pm = NULL;
  while (m) {
     if (pm && m == s1) {
        s[l++] = '.';
        pm = NULL;
        s1 = m->next;
        m = s1;
     } else {
        for (i = 0; i <= 1; i++) {
           if (!i) v = m->x;
              else v = m->y;
           v = v * 256 / scale;
           s[l++] = t[v & 15];
           s[l++] = t[(v >> 4) & 15];
         }
         pm = m;
         m = m->next;
     }
     if (l % 32 >= 28) {
        s = (char *)realloc(s, (64 + l - l % 32) * sizeof(char)) ;
        if (!s) { v4pDisplayError("full Heap") ; return NULL ; }
     }
  }
  s[l] = '\0';
  return s;
}

PolygonP v4pDecodePolygon(char *s, int scale) {
   ILayer z ;
   PolygonProps t ;
   Color col ;
   PolygonP p ;
   int i = 0 ;
   if (strlen(s) < 6) return NULL ;
   t = v4pXToD(s[i]) << 4 + v4pXToD(s[i + 1]) ;
   i = 2 ;
   col = v4pXToD(s[i]) << 4 + v4pXToD(s[i + 1]) ;
   i = 4 ;
   z = v4pXToD(s[i]) << 4 + v4pXToD(s[i + 1]) ;
   p = v4pPolygonNew(t, col, z) ;
   return v4pPolygonDecodePoints(p, s + 6, scale) ;
}

char *v4pEncodePolygon(PolygonP p, int scale) {
  const char *t = "0123456789ABCDEF" ;
  UInt16 i, v ;
  char *s, *ss, *sss ;

  s = (char *)malloc(7) ;
  ss = s ;
  for (i = 0 ; i <= 2 ; i++) {
   if (i == 0) v = v4pPolygonPutProp(p,0) ;
   else if (i == 1) v = v4pPolygonGetColor(p) ;
   else if (i == 2) v = v4pPolygonGetZ(p) ;
   *ss = t[v & 15] ;
   ss++ ;
   v = v >> 4 ;
   *ss = t[v & 15] ;
   ss++ ;
  }
  *ss = '\0' ;

  sss = v4pPolygonEncodePoints(p, scale);
  if (!sss) {
     free(s) ;
     return NULL ;
  } else {
     s = (char *)realloc(s, 7 + strlen(sss)) ;
     strcpy(ss, sss) ;
     free(sss) ;
     return s ;
  }
}


// add points to a polygon with coordinates decoded from a c-string
PolygonP v4pPolygonDecodeSVGPath(PolygonP p, char *s, int scale) {
   int j;
   Boolean toBeClosed = false, knowFirstPoint = false, nextIsRelative=false;
   char c ;
   enum e_status { INIT, MOVE, LINE, NEXT } status = INIT;
   float param_1, xs = 0, xs1;
   float param_2, ys = 0, ys1;
   int offset;
   
   for (j = 0 ; s[j] ; j++) {
      c = s[j] ;
      if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
         continue ;
         
      switch (status) {
          case NEXT: // way to add more points
             if (c >= '0' && c <= '9') {
               if (sscanf(&s[j], "%f,%f%n", &param_1, &param_2, &offset) >= 2) {
				  j += offset-1;
			      if (nextIsRelative) {
				     xs += param_1 ;
				     ys += param_2 ;
		          } else {
		              xs = param_1;
				      ys = param_2;
		          }
			      v4pPolygonAddPoint(p, xs * scale / 256, ys * scale / 256);
			   if (!knowFirstPoint) {
				   xs1 = xs ; ys1 = ys ; knowFirstPoint = true;
			   }
			   continue; // iterate for loop
			 } }
			 // there is no break here, NEXT case also handled hereafter
		  case INIT:
		     if ((c == 'M' || c == 'm') && knowFirstPoint)
			     v4pPolygonAddJump(p);
	         if (c == 'L' || c == 'l' ) {
	            status = LINE;
	         } else if (c == 'M' || c == 'm') {
	             status = MOVE;
	         } else if (c == 'C' || c == 'c' || c == 'Q' || c == 'q') {
	             status = LINE; // Curves are not supported
	         } else if (c == 'z') {
		 		 toBeClosed = true;
	 		     status = NEXT;
		     }
		     nextIsRelative = (c == 'm' || c == 'l');
	         break;
	      case LINE: case MOVE:
	         if (sscanf(&s[j], "%f,%f%n", &param_1, &param_2, &offset) >= 2) {
			   j+= offset -1;
			   if (toBeClosed && knowFirstPoint) {
                  v4pPolygonAddPoint(p, xs1 * scale / 256, ys1 * scale / 256);
                  toBeClosed = knowFirstPoint = false;
               }
			   if (nextIsRelative) {
				   xs += param_1 ;
				   ys += param_2 ;
		       } else {
		           xs = param_1;
				   ys = param_2;
		       }
			   v4pPolygonAddPoint(p, xs * scale / 256, ys * scale / 256);
			   if (!knowFirstPoint) {
				   xs1 = xs ; ys1 = ys ; knowFirstPoint = true;
			   }
			   status = NEXT;
			 }
	         break;
       } // switch status
   } // j
   if (toBeClosed && knowFirstPoint) {
      v4pPolygonAddPoint(p, xs1 * scale / 256, ys1 * scale / 256);
      toBeClosed = knowFirstPoint = false;
   }
  return p;
}
