#ifndef LOG_H
#define LOG_H

extern bool LogEnabled;

namespace log 
{
    void Print(const char* text, ...);
}

#endif