#include <processthreadsapi.h>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include "dumper.h"
#include "modules.h"
#include "log.h"
#include "efi.h"
#include "helper.h"

void dumper::DumpProcess(int PID, const char* moduleName, const char* outputFile, int bufferSize) 
{
    if (!efi::SetupAndTest()) 
    {
        printf("Failed to contact EFI driver\n");
        return;
    }
    
    modules::ModuleInfo moduleInfo = modules::GetProcessModules(PID);

    if (moduleInfo.totalModuleCount == 0) 
    {
        printf("Failed to query process modules\n");
    }

    log::Print("Searching for module in module list...\n");
    modules::ModuleDll targetDll;
    for (int i = 0; i < moduleInfo.totalModuleCount; i++) 
    {
        if (moduleInfo.list[i].name.find(moduleName) != std::string::npos) 
        {
            targetDll = moduleInfo.list[i];
            log::Print("Found target module with address 0x%p (size: %llu) and fullpath %s\n", targetDll.baseAddress, targetDll.size, targetDll.name.c_str());
        }
    }

    if (targetDll.baseAddress == 0) 
    {
        printf("Module not found\n");
        return;
    }

    log::Print("Opening output file...\n");
    HANDLE fileHandle = CreateFileA(outputFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE || !fileHandle) 
    {
		printf("Failed to open output file\n");
		return;
	}

    log::Print("Dumping memory with buffer size of %i...\n", bufferSize);
    uint64_t currentAddress = targetDll.baseAddress;
    uint64_t endAddresss = targetDll.baseAddress + targetDll.size;
    DWORD written = 0;
    while (true) 
    {
        uint64_t remaining = endAddresss - currentAddress;
        log::Print("Processing address 0x%p with remaining %llu\n", currentAddress, remaining);

        if ((int)remaining <= bufferSize) 
        {
            void* buffer = malloc(remaining);
            helper::CopyProcessMemory(PID, (void*)currentAddress, GetCurrentProcessId(), buffer, remaining);
            log::Print("Writing buffer to file...\n");
            WriteFile(fileHandle, buffer, remaining, &written, nullptr);
            free(buffer);
            break;
        } else 
        {
            void* buffer = malloc(bufferSize);
            helper::CopyProcessMemory(PID, (void*)currentAddress, GetCurrentProcessId(), buffer, 1024);
            log::Print("Writing buffer to file...\n");
	        WriteFile(fileHandle, buffer, 1024, &written, nullptr);
            free(buffer);
            currentAddress += bufferSize;
        }
    }

    log::Print("Closing output file...\n");
    CloseHandle(fileHandle);
}