#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <string>
#include "utils.h"

uint64_t utils::GetKernelModuleAddress(const std::string& moduleName)
{
	void* buffer = nullptr;
	DWORD buffer_size = 0;

	NTSTATUS status = NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(SystemModuleInformation), buffer, buffer_size, &buffer_size);

	while (status == (NTSTATUS)0xC0000004)
	{
		VirtualFree(buffer, 0, MEM_RELEASE);

		buffer = VirtualAlloc(nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		status = NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(SystemModuleInformation), buffer, buffer_size, &buffer_size);
	}

	if (!NT_SUCCESS(status))
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
		return 0;
	}

	const auto modules = static_cast<PRTL_PROCESS_MODULES>(buffer);

	for (auto i = 0u; i < modules->NumberOfModules; ++i)
	{
		const std::string current_module_name = std::string(reinterpret_cast<char*>(modules->Modules[i].FullPathName) + modules->Modules[i].OffsetToFileName);

		if (!_stricmp(current_module_name.c_str(), moduleName.c_str()))
		{
			const uint64_t result = reinterpret_cast<uint64_t>(modules->Modules[i].ImageBase);

			VirtualFree(buffer, 0, MEM_RELEASE);
			return result;
		}
	}

	VirtualFree(buffer, 0, MEM_RELEASE);
	return 0;
}