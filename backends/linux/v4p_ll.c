#define V4P_LL_C
#include "v4p_ll.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
static FILE* traceFile = NULL;

static void initTraceFile() {
    if (traceFile)
        return;

    if (! getenv("V4P_LOG_FILE")) {
        // printf("Trace will go into stderr\n");
        traceFile = stderr;
        return;
    }
    // printf("Trace will go into %s\n", getenv("V4P_LOG_FILE"));
    traceFile = fopen(getenv("V4P_LOG_FILE"), "a");
}

void v4pi_debug(char* formatString, ...) {
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
Boolean v4p_error(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    initTraceFile();
    vfprintf(traceFile, formatString, args);
    va_end(args);

    return success;
}
