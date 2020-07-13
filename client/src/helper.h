#ifndef HELPER_H
#define HELPER_H

#include "efi.h"
#include <windows.h>
#include <stdint.h>
#include <string>

namespace helper 
{
    void MemCopy(uint64_t destination, uint64_t source, uint64_t size);
    void ReadMemory(uint64_t address, void* buffer, uint64_t size);
    void WriteMemory(uint64_t address, void* buffer, uint64_t size);
    uint64_t GetKernelModuleExport(uint64_t kernelModuleBase, const std::string& functionName);
    bool PrepaireCommandFunctions(InputCommand* cmd);
    void CopyProcessMemory(int sourceProcessId, void* sourceAddress, int targetProcessId, void* targetAddress, int size);
}

#endif