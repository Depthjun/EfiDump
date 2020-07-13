#include <windows.h>
#include <string>
#include "helper.h"
#include "efi.h"
#include "utils.h"
#include "log.h"

void helper::MemCopy(uint64_t destination, uint64_t source, uint64_t size) 
{
    InputCommand cmd;
	cmd.Action = 0;
    cmd.Data[0] = destination;
    cmd.Data[1] = source;
    cmd.Data[2] = size;

    efi::SendCommand(&cmd);
}

void helper::ReadMemory(uint64_t address, void* buffer, uint64_t size) 
{
    MemCopy(reinterpret_cast<uint64_t>(buffer), address, size);
}

void helper::WriteMemory(uint64_t address, void* buffer, uint64_t size) 
{
    MemCopy(address, reinterpret_cast<uint64_t>(buffer), size);
}

uint64_t helper::GetKernelModuleExport(uint64_t kernelModuleBase, const std::string& functionName)
{
	log::Print("Getting export %s...\n", functionName.c_str());
	
	if (!kernelModuleBase)
		return 0;

	IMAGE_DOS_HEADER dos_header = { 0 };
	IMAGE_NT_HEADERS64 nt_headers = { 0 };

	ReadMemory(kernelModuleBase, &dos_header, sizeof(dos_header));
    if (dos_header.e_magic != IMAGE_DOS_SIGNATURE) 
	{
		log::Print("Invalid DOS magic\n");
		return 0;
	}
	
    ReadMemory(kernelModuleBase + dos_header.e_lfanew, &nt_headers, sizeof(nt_headers));
    if (nt_headers.Signature != IMAGE_NT_SIGNATURE) 
	{	
		log::Print("Invalid NT magic\n");
		return 0;
	}      

	const auto export_base = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	const auto export_base_size = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	if (!export_base || !export_base_size)
		return 0;

	const auto export_data = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(VirtualAlloc(nullptr, export_base_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

	ReadMemory(kernelModuleBase + export_base, export_data, export_base_size);

	const auto delta = reinterpret_cast<uint64_t>(export_data) - export_base;

	const auto name_table = reinterpret_cast<uint32_t*>(export_data->AddressOfNames + delta);
	const auto ordinal_table = reinterpret_cast<uint16_t*>(export_data->AddressOfNameOrdinals + delta);
	const auto function_table = reinterpret_cast<uint32_t*>(export_data->AddressOfFunctions + delta);

	for (auto i = 0u; i < export_data->NumberOfNames; ++i)
	{
		const std::string current_functionName = std::string(reinterpret_cast<char*>(name_table[i] + delta));

		if (!_stricmp(current_functionName.c_str(), functionName.c_str()))
		{
			const auto function_ordinal = ordinal_table[i];
			const auto function_address = kernelModuleBase + function_table[function_ordinal];

			if (function_address >= kernelModuleBase + export_base && function_address <= kernelModuleBase + export_base + export_base_size)
			{
				VirtualFree(export_data, 0, MEM_RELEASE);
				return 0;
			}

			VirtualFree(export_data, 0, MEM_RELEASE);
			return function_address;
		}
	}

	VirtualFree(export_data, 0, MEM_RELEASE);
	return 0;
}

bool helper::PrepaireCommandFunctions(InputCommand* cmd) 
{
	cmd->Functions[IndexPsLookupProcessByProcessId] = GetKernelModuleExport(utils::GetKernelModuleAddress("ntoskrnl.exe"), "PsLookupProcessByProcessId");
	log::Print("Export PsLookupProcessByProcessId found at 0x%p\n", cmd->Functions[IndexPsLookupProcessByProcessId]);
	if (!cmd->Functions[IndexPsLookupProcessByProcessId])
		return false;

	cmd->Functions[IndexKeAttachProcess] = GetKernelModuleExport(utils::GetKernelModuleAddress("ntoskrnl.exe"), "KeAttachProcess");
	log::Print("Export KeAttachProcess found at 0x%p\n", cmd->Functions[IndexKeAttachProcess]);
	if (!cmd->Functions[IndexKeAttachProcess])
		return false;

	cmd->Functions[IndexKeDetachProcess] = GetKernelModuleExport(utils::GetKernelModuleAddress("ntoskrnl.exe"), "KeDetachProcess");
	log::Print("Export KeDetachProcess found at 0x%p\n", cmd->Functions[IndexKeDetachProcess]);
	if (!cmd->Functions[IndexKeDetachProcess])
		return false;

	cmd->Functions[IndexExAllocatePool] = GetKernelModuleExport(utils::GetKernelModuleAddress("ntoskrnl.exe"), "ExAllocatePool");
	log::Print("Export ExAllocatePool found at 0x%p\n", cmd->Functions[IndexExAllocatePool]);
	if (!cmd->Functions[IndexExAllocatePool])
		return false;

	cmd->Functions[IndexExFreePoolWithTag] = GetKernelModuleExport(utils::GetKernelModuleAddress("ntoskrnl.exe"), "ExFreePoolWithTag");
	log::Print("Export ExFreePoolWithTag found at 0x%p\n", cmd->Functions[IndexExFreePoolWithTag]);
	if (!cmd->Functions[IndexExFreePoolWithTag])
		return false;
	
	return true;
}

void helper::CopyProcessMemory(int sourceProcessId, void* sourceAddress, int targetProcessId, void* targetAddress, int size) 
{
	InputCommand cmd;
	cmd.Action = 1;

	PrepaireCommandFunctions(&cmd);

    cmd.Data[0] = size;
    cmd.Data[1] = sourceProcessId;
    cmd.Data[2] = targetProcessId;
    cmd.Data[3] = (uint64_t)sourceAddress;
    cmd.Data[4] = (uint64_t)targetAddress;

    efi::SendCommand(&cmd);
}