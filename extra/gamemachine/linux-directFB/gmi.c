#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include "gamemachine.h"
#include "_v4pi.h"

// get ticks
#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC / 1000);

static Int32 SC_CLK_TCK = 0;

Int32 gmGetTicks() {
	
	static struct tms tmsBuffer;
	clock_t t = times(&tmsBuffer);
	if (!SC_CLK_TCK) SC_CLK_TCK = sysconf(_SC_CLK_TCK);

	return (t / SC_CLK_TCK) * 1000 + ((t % SC_CLK_TCK) * 1000) / SC_CLK_TCK;
}

// pause execution
void gmDelay(Int32 d) {
    usleep(d * 1000);
}

int gmPollEvents() {
  int rc = 0;
  return rc;
}
