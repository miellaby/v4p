/**
 * V4P Low-Level Implementation for Emscripten
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <emscripten.h>

#include "v4p_ll.h"

// Debug function
void v4pi_debug(const char* format, ...) {
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    EM_ASM({
        console.log('DEBUG: ' + UTF8ToString($0));
    }, buffer);
    #endif
}

// Error function
void v4pi_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    EM_ASM({
        console.error('ERROR: ' + UTF8ToString($0));
    }, buffer);
}

// Error function that returns Boolean (compatibility with other backends)
Boolean v4p_error(char* s, ...) {
    va_list args;
    va_start(args, s);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), s, args);
    va_end(args);
    
    EM_ASM({
        console.error('ERROR: ' + UTF8ToString($0));
    }, buffer);
    
    return failure;
}

// Trace function
void v4pi_trace(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    EM_ASM({
        console.trace('TRACE[' + UTF8ToString($0) + ']: ' + UTF8ToString($1));
    }, tag, buffer);
}