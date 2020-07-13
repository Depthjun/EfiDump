#include <windows.h>
#include <stdio.h>
#include "log.h"

bool LogEnabled = false;

void log::Print(const char* text, ...) 
{
    va_list args;
    va_start(args, text);
    if (LogEnabled) vprintf(text, args);
    va_end (args);
}