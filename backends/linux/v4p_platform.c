#include "v4p_platform.h"
#include "v4p_trace.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
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
