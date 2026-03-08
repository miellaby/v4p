#include "v4p_platform.h"
#include "v4p_trace.h"

void v4p_debug(char* formatString, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}

int v4p_error(char* formatString, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}

void v4pi_trace(const char* tag, const char* format, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}

void v4p_memset(uint8_t* pdst, uint32_t numBytes, uint8_t value) {
    uint32_t a4;
    int32_t n;
    void* dst;
    n = numBytes;
    dst = pdst;
    if (n > 6) {
        while ((int32_t) dst & 3) {
            *((uint8_t*) dst)++ = value;
            n--;
        }
        a4 = value;
        a4 = (a4 << 8) | a4;
        a4 = (a4 << 16) | a4;
        n -= 16;
        while (n > 0) {
            *((uint32_t*) dst)++ = a4;
            *((uint32_t*) dst)++ = a4;
            *((uint32_t*) dst)++ = a4;
            *((uint32_t*) dst)++ = a4;
            n -= 16;
        }
        n += 12;
        while (n > 0) {
            *((uint32_t*) dst)++ = a4;
            n -= 4;
        }
        n += 4;
    }
    while (n > 0) {
        *((uint8_t*) dst)++ = value;
        n--;
    }
}
