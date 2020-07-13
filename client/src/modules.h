#ifndef MODULES_H
#define MODULES_H

#include <windows.h>
#include <string>

namespace modules 
{
    typedef struct _ModuleDll
    {
        uintptr_t baseAddress;
        uintptr_t size;
        std::string name;
    } ModuleDll;

    #define MODULE_MAX 256
    typedef struct _ModuleInfo 
    {
        ModuleDll list[MODULE_MAX];
        int totalModuleCount;
    } ModuleInfo;
    
    typedef enum _MEMORY_INFORMATION_CLASS
    {
        MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
        MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
        MemoryMappedFilenameInformation, // UNICODE_STRING
        MemoryRegionInformation, // MEMORY_REGION_INFORMATION
        MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
        MemorySharedCommitInformation, // MEMORY_SHARED_COMMIT_INFORMATION
        MemoryImageInformation, // MEMORY_IMAGE_INFORMATION
        MemoryRegionInformationEx,
        MemoryPrivilegedBasicInformation,
        MemoryEnclaveImageInformation, // MEMORY_ENCLAVE_IMAGE_INFORMATION // since REDSTONE3
        MemoryBasicInformationCapped
    } MEMORY_INFORMATION_CLASS;

    extern "C" 
    {
        NTSTATUS
        NtQueryVirtualMemory(
            HANDLE ProcessHandle,
            PVOID BaseAddress,
            MEMORY_INFORMATION_CLASS MemoryInformationClass,
            PVOID MemoryInformation,
            SIZE_T MemoryInformationLength,
            PSIZE_T ReturnLength
        );

        PVOID
        RtlAllocateHeap(
            PVOID HeapHandle,
            ULONG Flags,
            SIZE_T Size
        );

        BOOLEAN
        RtlFreeHeap(
            PVOID HeapHandle,
            ULONG Flags,
            PVOID BaseAddress
        );
    }

    typedef NTSTATUS(NTAPI* PRTL_HEAP_COMMIT_ROUTINE)(
        PVOID Base,
        PVOID* CommitAddress,
        PSIZE_T CommitSize
        );

    #define HEAP_SETTABLE_USER_VALUE 0x00000100
    #define HEAP_SETTABLE_USER_FLAG1 0x00000200
    #define HEAP_SETTABLE_USER_FLAG2 0x00000400
    #define HEAP_SETTABLE_USER_FLAG3 0x00000800
    #define HEAP_SETTABLE_USER_FLAGS 0x00000e00

    #define HEAP_CLASS_0 0x00000000 // Process heap
    #define HEAP_CLASS_1 0x00001000 // Private heap
    #define HEAP_CLASS_2 0x00002000 // Kernel heap
    #define HEAP_CLASS_3 0x00003000 // GDI heap
    #define HEAP_CLASS_4 0x00004000 // User heap
    #define HEAP_CLASS_5 0x00005000 // Console heap
    #define HEAP_CLASS_6 0x00006000 // User desktop heap
    #define HEAP_CLASS_7 0x00007000 // CSR shared heap
    #define HEAP_CLASS_8 0x00008000 // CSR port heap
    #define HEAP_CLASS_MASK 0x0000f000

    #define PTR_ADD_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) + (ULONG_PTR)(Offset)))

    char* GetFileNameFromMemory(HANDLE proc, PVOID baseaddress);
    ModuleInfo GetProcessModules(int pid);
}

#endif