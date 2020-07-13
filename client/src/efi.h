#ifndef EFI_H
#define EFI_H

#include <windows.h>
#include <winternl.h>

#define NTSTATUS LONG

extern "C"
{
    NTSTATUS
    RtlAdjustPrivilege(
        ULONG Privilege,
        BOOLEAN Enable,
        BOOLEAN Client,
        PBOOLEAN WasEnabled
    );

    NTSTATUS
    NtSetSystemEnvironmentValueEx(
        PUNICODE_STRING VariableName,
        LPGUID VendorGuid,
        PVOID Value,
        ULONG ValueLength,
        ULONG Attributes
    );
}

#define RTL_CONSTANT_STRING(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), (PWSTR)s }
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE (22L)

extern GUID DummyGuid;

#define EFI_VARIABLE_NON_VOLATILE                          0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                    0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS                        0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD                 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS            0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE                          0x00000040
#define ATTRIBUTES (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

// --- SHARED STRUCTS START ---
enum ActionIndex 
{
    IndexCopyMemory
};

enum FunctionIndex 
{
    IndexPsLookupProcessByProcessId,
    IndexKeAttachProcess,
    IndexKeDetachProcess,
    IndexExAllocatePool,
    IndexExFreePoolWithTag
};

typedef struct _InputCommand 
{
    unsigned long long Action;
    unsigned long long Functions[10];
    unsigned long long Data[10];
} InputCommand;

#define VARIABLE_NAME L"pmuDifE" // EfiDump
// --- SHARED STRUCTS END ---

namespace efi 
{
    bool SetEnvironmentPrivilege();
    bool SendCommand(InputCommand* cmd);
    bool SetupAndTest();
}

#endif