#include <unistd.h>
#include <vga.h>
#include <sys/times.h>
#include "gamemachine.h"

#ifdef TESTU_GM_LINUX_SVGA
#include <stdio.h>
#endif

Int32 gmGetTicks() {
    static struct tms buf;
    static int clk_ticks = 0;
    if (!clk_ticks) {
        clk_ticks = sysconf(_SC_CLK_TCK);
#ifdef TESTU_GM_LINUX_SVGA
        printf("clk_ticks = %d\n", clk_ticks) ;
#endif
}

    Int32 t = times(&buf) * 1000 / clk_ticks;
    return t;
}

// pause execution
void gmDelay(Int32 d) {
    usleep(d * 1000);
}

// poll user events
int gmPollEvents() {
  static int firstRun = 1;

  int rc = 0; // return code

  if (firstRun) {
    firstRun = 0;
  }

  char alph = vga_getkey();

  while (alph)  { // key loop
    switch (alph) {
       case 'q':
       case 'Q':
       case 27: {
         rc = 1;
         break;
       }
       default: // TODO : Mouse not supported yet
         break;
    } // switch
    alph = vga_getkey();
  } // loop
 return rc;
}

#ifdef TESTU_GM_LINUX_SVGA
#include <stdio.h>
#include "gamemachine.h"
Boolean gmOnInit() {
    return success;
}

void gmOnQuit() {
}

Boolean gmOnIterate() {
    return !success;
}

Boolean gmOnFrame() {
    return success;
}

int main(int argc, char** argv) {
    Int32 t0 = gmGetTicks();
    gmDelay(1000);
    Int32 t1 = gmGetTicks();
    printf("Waited 1s from %d to %d\n", (int)t0, (int)t1);
    return gmMain(argc, argv);
}
#endif
