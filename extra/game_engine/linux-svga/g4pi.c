#include <unistd.h>
#include <vga.h>
#include <sys/times.h>
#include "game_engine.h"
#include "g4pi.h"

#ifdef TESTU_G4P_LINUX_SVGA
    #include <stdio.h>
#endif

Int32 g4pGetTicks() {
    static struct tms buf;
    static int clk_ticks = 0;
    if (! clk_ticks) {
        clk_ticks = sysconf(_SC_CLK_TCK);
#ifdef TESTU_G4P_LINUX_SVGA
        printf("clk_ticks = %d\n", clk_ticks);
#endif
    }

    Int32 t = times(&buf) * 1000 / clk_ticks;
    return t;
}

// pause execution
void g4pDelay(Int32 d) {
    usleep(d * 1000);
}

// Initialize the game engine
void g4piInit() {
    // SVGA initialization is typically handled by the display system
    // This stub can be extended if needed for SVGA-specific initialization
}

// Cleanup the game engine
void g4piDestroy() {
    // SVGA cleanup is typically handled by the display system
    // This stub can be extended if needed for SVGA-specific cleanup
}

// poll user events
int g4pPollEvents() {
    static int firstRun = 1;

    int rc = 0;  // return code

    if (firstRun) {
        firstRun = 0;
    }

    char alph = vga_getkey();

    while (alph) {  // key loop
        switch (alph) {
            case 'q':
            case 'Q':
            case 27:
                {
                    rc = 1;
                    break;
                }
            default:  // TODO : Mouse not supported yet
                break;
        }  // switch
        alph = vga_getkey();
    }  // loop
    return rc;
}

#ifdef TESTU_G4P_LINUX_SVGA
    #include <stdio.h>
    #include "game_engine.h"
Boolean g4pOnInit() {
    return success;
}

void g4pOnQuit() {
}

Boolean g4pOnIterate() {
    return ! success;
}

Boolean g4pOnFrame() {
    return success;
}

int main(int argc, char** argv) {
    Int32 t0 = g4pGetTicks();
    g4pDelay(1000);
    Int32 t1 = g4pGetTicks();
    printf("Waited 1s from %d to %d\n", (int) t0, (int) t1);
    return g4pMain(argc, argv);
}
#endif
