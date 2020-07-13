#ifndef DUMPER_H
#define DUMPER_H

namespace dumper
{
    void DumpProcess(int PID, const char* moduleName, const char* outputFile, int bufferSize);
}

#endif