#include <windows.h>
#include <ntdef.h>
#include "modules.h"
#include "log.h"

char* modules::GetFileNameFromMemory(HANDLE proc, PVOID baseaddress) 
{
    log::Print("Getting filename from memory location 0x%p...\n", baseaddress);
    
    int bufferSize = 0x100;
	char* buffer = (char*)malloc(bufferSize);
	SIZE_T returnLength;

	NTSTATUS status = NtQueryVirtualMemory(proc, baseaddress, MemoryMappedFilenameInformation, buffer, bufferSize, &returnLength);
	
	if (status == ((NTSTATUS)0x80000005L)) // STATUS_BUFFER_OVERFLOW 
	{
		log::Print("NtQueryVirtualMemory returned STATUS_BUFFER_OVERFLOW\n");
        
        free(buffer);
		bufferSize = returnLength;
		buffer = (char*)malloc(bufferSize);

		status = NtQueryVirtualMemory(
			proc,
			baseaddress,
			MemoryMappedFilenameInformation,
			buffer,
			bufferSize,
			&returnLength
		);
	}

	if (status != 0) 
	{
		log::Print("NtQueryVirtualMemory returned non success status\n");
        return 0;
	}

    if (bufferSize < (int)(sizeof(USHORT) * 3)) 
    {
        log::Print("Size of buffer is less than UNICODE_STRING structure can have\n");
        return 0;
    }

	PUNICODE_STRING unistr = (PUNICODE_STRING)buffer;
    if (unistr->Length < 5 || unistr->Length >= 250) 
    {
        log::Print("NtQueryVirtualMemory returned non success status\n");
        return 0;
    }

    char* multi = (char*)malloc((int)unistr->Length);
    if (!multi) return 0;
    memset(multi, 0, unistr->Length);

    WideCharToMultiByte(CP_ACP, 0, unistr->Buffer, unistr->Length, multi, unistr->Length, 0, 0);
    free(buffer);

    log::Print("Found module %s\n", multi);

    return multi;
}

modules::ModuleInfo modules::GetProcessModules(int pid) 
{   
    log::Print("Getting process modules...\n");
    
    ModuleInfo mi;
	MEMORY_BASIC_INFORMATION basicInfo;
	int total = 0;

    log::Print("Opening PROCESS_QUERY_INFORMATION handle to the process...\n");
	HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (proc == 0 || proc == INVALID_HANDLE_VALUE) 
	{
        log::Print("Failed to open handle\n");
        CloseHandle(proc);
        return mi;
	}

    log::Print("Getting lowest address...\n");
	PVOID baseAddress = (PVOID)0;
	if (!VirtualQueryEx(proc, baseAddress, &basicInfo, sizeof(MEMORY_BASIC_INFORMATION))) 
	{
		log::Print("Failed to run VirtualQueryEx\n");
        CloseHandle(proc);
        return mi;
	}
    log::Print("Lowest address is 0x%p\n", baseAddress);

	int querySucceeded = 1;

    log::Print("Looping through memory regions...\n");
    while (querySucceeded)
    {
        PVOID allocationBase;
        SIZE_T allocationSize;

        if (basicInfo.Type == MEM_MAPPED || basicInfo.Type == MEM_IMAGE)
        {
            allocationBase = basicInfo.AllocationBase;
            allocationSize = 0;

            log::Print("Found valid memory region at 0x%p\n", allocationBase);

            do
            {
                baseAddress = PTR_ADD_OFFSET(baseAddress, basicInfo.RegionSize);
                allocationSize += basicInfo.RegionSize;

				if (!VirtualQueryEx(proc, baseAddress, &basicInfo, sizeof(MEMORY_BASIC_INFORMATION)))
				{
					log::Print("VirtualQueryEx failed\n", baseAddress);
                    querySucceeded = FALSE;
					break;
				}
            } while (basicInfo.AllocationBase == allocationBase);

			char* filename = GetFileNameFromMemory(proc, allocationBase);
			if (filename != 0) 
			{
				mi.list[total].name = filename;
				mi.list[total].baseAddress = (uintptr_t)allocationBase;
                mi.list[total].size = allocationSize;
                mi.totalModuleCount = total;
				total++;
				if (total >= MODULE_MAX) 
				{
					log::Print("Maximum module count reached\n");
                    CloseHandle(proc);
	                return mi;
				}
			}
        }
        else
        {
            baseAddress = PTR_ADD_OFFSET(baseAddress, basicInfo.RegionSize);
            log::Print("Found invalid memory region at 0x%p\n", baseAddress);

			if (!VirtualQueryEx(proc, baseAddress, &basicInfo, sizeof(MEMORY_BASIC_INFORMATION)))
			{
				log::Print("VirtualQueryEx failed\n", baseAddress);
                querySucceeded = FALSE;
			}
        }
    }

	CloseHandle(proc);
	return mi;
}

