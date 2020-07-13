#include <windows.h>
#include <winternl.h>
#include "efi.h"
#include "log.h"
#include "helper.h"
#include "utils.h"

GUID DummyGuid
= { 0x4BE4DF61, 0x92CA, 0x11D2, { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C } };

bool efi::SetEnvironmentPrivilege() 
{
    BOOLEAN wasEnabled = false;
    NTSTATUS status = RtlAdjustPrivilege(SE_SYSTEM_ENVIRONMENT_PRIVILEGE, true, false, &wasEnabled);
    return (status == 0);
}

bool efi::SendCommand(InputCommand* cmd) 
{
    UNICODE_STRING VariableName = RTL_CONSTANT_STRING(VARIABLE_NAME);
	NTSTATUS status = NtSetSystemEnvironmentValueEx(&VariableName,
		&DummyGuid,
		cmd,
		sizeof(InputCommand),
		ATTRIBUTES);
    return (status == 0);
}

bool efi::SetupAndTest() 
{
    if (!efi::SetEnvironmentPrivilege()) 
    {
        log::Print("Failed to set SE_SYSTEM_ENVIRONMENT_PRIVILEGE\n");
        return false;
    }

    uint64_t testExport = helper::GetKernelModuleExport(utils::GetKernelModuleAddress("ntoskrnl.exe"), "ExAllocatePool");
    log::Print("Test export (ExAllocatePool) found at 0x%p\n", testExport);

    return (testExport != 0);
}