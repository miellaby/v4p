
#include <PalmOS.h>

void v4pi_debug(char* formatString, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}

Boolean v4p_error(char* formatString, ...) {
    va_list args;
    Char text[0x100];
    va_start(args, formatString);
    StrVPrintF(text, formatString, args);
    va_end(args);
    WinDrawChars(text, StrLen(text), 0, 0);
}
