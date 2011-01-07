#ifdef TESTU
#include "gamemachine.h"
#endif
#include "v4p.h"
#include "v4pi.h"


#define AA0 "1111 1111 1111 111  1111 1111 1111 1  1 111  1111 1  1 1    "
#define AA1 "1  1 1  1 1    1  1 1    1    1    1  1  1      1 1 1  1    "
#define AA2 "1111 111  1    1  1 111  111  1 11 1111  1      1 11   1    "
#define AA3 "1  1 1  1 1    1  1 1    1    1  1 1  1  1   1  1 1 1  1    "
#define AA4 "1  1 1111 1111 111  1111 1    1111 1  1 111  111  1  1 1111 "

#define AB0 "111  1  1 1111 111  1111 111  1111 1111 1  1 1  1 1 11 1  1 "
#define AB1 "1111 11 1 1  1 1  1 1  1 1  1 1       1 1  1 1  1 1 11 1  1 "
#define AB2 "11 1 1 11 1  1 111  1  1 111  1111    1 1  1 1  1 1 11  11  "
#define AB3 "11 1 1  1 1  1 1    1 11 1  1    1    1 1  1 1 1  1111 1  1 "
#define AB4 "11 1 1  1 1111 1    1111 1  1 1111    1 1111 11   111  1  1 "

#define AC0 "1  1 1111 "
#define AC1 "1  1    1 "
#define AC2 " 111  11  "
#define AC3 "   1 1    "
#define AC4 "  1  1111 "

#define NA0 " 111   1  1111 1111 1  1 1111 1111 1111 1111 1111 "
#define NA1 " 1 1  11     1    1 1  1 1    1       1 1  1 1  1 "
#define NA2 " 1 1   1  1111 1111 1111 1111 1111   1  1111 1111 "
#define NA3 " 1 1   1  1       1    1    1 1  1  1   1  1    1 "
#define NA4 " 111  111 1111 1111    1 1111 1111  1   1111 1111 "

// !/?,"'.:;&*+-<=>@$
#define PA0 " 11     1  11  1 1    1        11   11   11  1  1   1  "
#define PA1 " 11    11 1  1 1 1    1        11   11  1  1  11    1  "
#define PA2 " 11   11    1  1 1   1                   1   1111  111 "
#define PA3 "     11                   11   11    1  1 1   11    1  "
#define PA4 " 11  1      1             11   11   1    111 1  1   1  "

#define PB0 "       11      11    11   111 "
#define PB1 "      11  1111  11  1  1 111  "
#define PB2 "1111 11          11   11 1111 "
#define PB3 "      11  1111  11   1 1  111 "
#define PB4 "       11      11    11  111  "


#define S0 " " AA0 AB0 AC0 NA0 PA0 PB0
#define S1 " " AA1 AB1 AC1 NA1 PA1 PB1
#define S2 " " AA2 AB2 AC2 NA2 PA2 PB2
#define S3 " " AA3 AB3 AC3 NA3 PA3 PB3
#define S4 " " AA4 AB4 AC4 NA4 PA4 PB4

char* qfont[5] = {
   S0,
   S1,
   S2,
   S3,
   S4
};

#define CHAR_WIDTH 4
#define CHAR_HEIGHT 5

PolygonP qfontDefinePolygonFromChar(char c, PolygonP poly,
    Coord x, Coord y, Coord width, Coord height) {
  int ichar = 0;
  char* s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!/?,\"'.:;&*+-<=>@$";
  if (!c || c == ' ') return poly;
  
  while (c != s[ichar] && s[ichar]) ichar++;
  if (!s[ichar]) ichar = 45; // *
  
  int i, j, down, is;
  for (i = 0; i <= CHAR_WIDTH ; i++) {
    down = 0;
    //v4pPolygonAddJump(poly);
    for (j = 0; j < CHAR_HEIGHT ; j++) {
      is = ichar * (1 + CHAR_WIDTH) + i;
      if (!down && qfont[j][is] != qfont[j][1 + is]) {
        v4pPolygonAddPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
        down = 1;
      } else if (down && qfont[j][is] == qfont[j][1 + is]) {
        // dubed point to left the pen up
        v4pPolygonAddPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
        v4pPolygonAddJump(poly);
        down = 0;
      }
    }
    if (down) {
        v4pPolygonAddPoint(poly, x + i * width / CHAR_WIDTH, y + j * height / CHAR_HEIGHT);
        v4pPolygonAddJump(poly);
    }
  }
  return poly;
}

PolygonP qfontDefinePolygonFromString(char* s, PolygonP poly,
    Coord x, Coord y, Coord width, Coord height, Coord interleave) {
  char c;
  int i;
  for (i = 0; c = s[i]; i++) {
    qfontDefinePolygonFromChar(c, poly, x, y, width, height);
    x += width + interleave;
  }
  return poly;
}

#ifdef TESTU
#define STRESS_AMOUNT 10
PolygonP pCol;
PolygonP pColMatrix[STRESS_AMOUNT][STRESS_AMOUNT];

int iu = 0;
int diu = STRESS_AMOUNT;
int liu  = 3;


Boolean gmOnInit() {
  int j, k;

  v4pDisplayInit(1, 0);
  v4pInit();

  v4pSetBGColor(blue);
 
  pCol = v4pPolygonNew(absolute, red, 10);
  qfontDefinePolygonFromString("HELLO", pCol,
    -v4pDisplayWidth / 4, -v4pDisplayWidth / 16,
     v4pDisplayWidth / 8, v4pDisplayWidth / 8,
     5);
  qfontDefinePolygonFromString("WORLD", pCol,
    -v4pDisplayWidth / 4, v4pDisplayWidth / 16 + 5,
     v4pDisplayWidth / 8, v4pDisplayWidth / 8,
     5);

  for (j= 0; j < STRESS_AMOUNT; j++) {
    for (k = 0; k < STRESS_AMOUNT; k++) {
      pColMatrix[j][k] = v4pPolygonClone(pCol);
      v4pPolygonTransformClone(pCol, pColMatrix[j][k], v4pDisplayWidth * (2 + 2 * k - STRESS_AMOUNT) / 2, v4pDisplayWidth * (1 + j - STRESS_AMOUNT/2)/2, 0, 10);
    }
  }
  return success;
}

Boolean gmOnIterate() {
	int i = iu, j, k;
	if (diu>0 && i >128 * STRESS_AMOUNT) diu=-diu;
	if (diu<0 && i + diu < -100) {
	  diu=-diu;
	  liu--;
	}
    v4pSetView(-v4pDisplayWidth * i / 256, -v4pDisplayHeight * i / 256, v4pDisplayWidth + v4pDisplayWidth * i / 256, v4pDisplayHeight + v4pDisplayHeight * i / 256);
    
    if (0) // dead code, not compatible with qfont because of lacking horizontal edges
      for (j= 0; j < STRESS_AMOUNT; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
          v4pPolygonTransformClone(pCol, pColMatrix[j][k], v4pDisplayWidth * (1 + 2 * k - STRESS_AMOUNT/2) / 2, v4pDisplayWidth * (1 + j - STRESS_AMOUNT/2)/2, (j * k) + i / 16, 0);
        }
      }
      
  iu+=diu;
  return (liu < 0);
}

Boolean gmOnFrame() {
  v4pRender();
  return success;
}

void gmOnQuit() {
  v4pDisplayQuit();
}

int main(int argc, char** argv) {
  return gmMain(argc, argv);
}  
  
#endif
