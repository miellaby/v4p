#include "v4p_platform.h"
#include "v4p_trace.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
static FILE* traceFile = NULL;

static void initTraceFile() {
    if (traceFile) return;

    const char* logFile = getenv("V4P_LOG_FILE");
    if (! logFile) {
        // printf("Trace will go into stderr\n");
        traceFile = stderr;
        return;
    }
    // printf("Trace will go into %s\n", logFile);
    traceFile = fopen(logFile, "a");
}

int32_t v4p_getTicks() {
    static struct tms buf;
    static int clk_ticks = 0;
    if (! clk_ticks) {
        clk_ticks = sysconf(_SC_CLK_TCK);
    }
    int32_t t = times(&buf) * 1000 / clk_ticks;
    return t;
}

void v4p_delay(int32_t d) {
    if (d <= 0) return;
    struct timespec req;
    req.tv_sec = d / 1000;
    req.tv_nsec = (d % 1000) * 1000000;
    while (nanosleep(&req, &req) == -1 && errno == EINTR) {
        // Continue if interrupted by signal (don't delay more than a few ms with this func)
    }
}

void v4p_debug(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    initTraceFile();
    vfprintf(traceFile, formatString, args);
    va_end(args);
}

void v4pi_trace(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    initTraceFile();
    fprintf(traceFile, "[%s] ", tag);
    vfprintf(traceFile, format, args);
    va_end(args);
}

// error logging helper
int v4p_error(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    initTraceFile();
    vfprintf(traceFile, formatString, args);
    va_end(args);

    return success;
}
