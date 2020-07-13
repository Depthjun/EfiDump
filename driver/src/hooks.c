#include "general.h"
#include "handler.h"

BOOLEAN Virtual = FALSE;
BOOLEAN Runtime = FALSE;

#define VARIABLE_NAME L"pmuDifE" // EfiDump

EFI_SET_VARIABLE oSetVariable;
EFIAPI EFI_STATUS HookedSetVariable(CHAR16* variableName, EFI_GUID* vendorGuid, UINT32 attributes, UINTN dataSize, void* data) 
{
    if (Virtual && Runtime) 
    {
        if (variableName != NULL && variableName[0] != CHAR_NULL) 
        {
            if (StrnCmp(variableName, VARIABLE_NAME, (sizeof(VARIABLE_NAME) / sizeof(CHAR16)) - 1) == 0) 
            {
                if (dataSize == 0 && data == NULL)
                {
                    return EFI_SUCCESS;
                }

                if (dataSize == sizeof(InputCommand)) 
                {
                    HandleCommand((InputCommand*)data);
                    return EFI_SUCCESS;
                }
            }
        }
    }
    
    return oSetVariable(variableName, vendorGuid, attributes, dataSize, data);
}

EFI_GET_TIME oGetTime;
EFIAPI EFI_STATUS HookedGetTime(EFI_TIME* time, EFI_TIME_CAPABILITIES* capabilities) 
{
    return oGetTime(time, capabilities);
}

EFI_SET_TIME oSetTime;
EFIAPI EFI_STATUS HookedSetTime(EFI_TIME* time) 
{
    return oSetTime(time);
}

EFI_GET_WAKEUP_TIME oGetWakeupTime;
EFIAPI EFI_STATUS HookedGetWakeupTime(BOOLEAN* enabled, BOOLEAN* pending, EFI_TIME* time) 
{
    return oGetWakeupTime(enabled, pending, time);
}

EFI_SET_WAKEUP_TIME oSetWakeupTime;
EFIAPI EFI_STATUS HookedSetWakeupTime(BOOLEAN enable, EFI_TIME* time) 
{
    return oSetWakeupTime(enable, time);
}

EFI_SET_VIRTUAL_ADDRESS_MAP oSetVirtualAddressMap;
EFIAPI EFI_STATUS HookedSetVirtualAddressMap(UINTN mapSize, UINTN descriptorSize, UINT32 version, EFI_MEMORY_DESCRIPTOR* virtualMap) 
{
    return oSetVirtualAddressMap(mapSize, descriptorSize, version, virtualMap);
}

EFI_CONVERT_POINTER oConvertPointer;
EFIAPI EFI_STATUS HookedConvertPointer(UINTN debug, void** address) 
{
    return oConvertPointer(debug, address);
}

EFI_GET_VARIABLE oGetVariable;
EFIAPI EFI_STATUS HookedGetVariable(CHAR16* variableName, EFI_GUID* vendorGuid, UINT32* attributes, UINTN* dataSize, void* data) 
{
    return oGetVariable(variableName, vendorGuid, attributes, dataSize, data);
}

EFI_GET_NEXT_VARIABLE_NAME oGetNextVariableName;
EFIAPI EFI_STATUS HookedGetNextVariableName(UINTN* variableNameSize, CHAR16* variableName, EFI_GUID* vendorGuid) 
{
    return oGetNextVariableName(variableNameSize, variableName, vendorGuid);
}

EFI_GET_NEXT_HIGH_MONO_COUNT oGetNextHighMonotonicCount;
EFIAPI EFI_STATUS HookedGetNextHighMonotonicCount(UINT32* highCount) 
{
    return oGetNextHighMonotonicCount(highCount);
}

EFI_RESET_SYSTEM oResetSystem;
EFIAPI EFI_STATUS HookedResetSystem(EFI_RESET_TYPE resetType, EFI_STATUS resetStatus, UINTN dataSize, CHAR16* resetData) 
{
    return oResetSystem(resetType, resetStatus, dataSize, resetData);
}

EFI_UPDATE_CAPSULE oUpdateCapsule;
EFIAPI EFI_STATUS HookedUpdateCapsule(EFI_CAPSULE_HEADER** capsuleHeaderArray, UINTN capsuleCount, EFI_PHYSICAL_ADDRESS scatterGatherList) 
{
    return oUpdateCapsule(capsuleHeaderArray, capsuleCount, scatterGatherList);
}

EFI_QUERY_CAPSULE_CAPABILITIES oQueryCapsuleCapabilities;
EFIAPI EFI_STATUS HookedQueryCapsuleCapabilities(EFI_CAPSULE_HEADER** capsuleHeaderArray, UINTN capsuleCount, UINT64* maximumCapsuleSize, EFI_RESET_TYPE* resetType) 
{
    return oQueryCapsuleCapabilities(capsuleHeaderArray, capsuleCount, maximumCapsuleSize, resetType);
}

EFI_QUERY_VARIABLE_INFO oQueryVariableInfo;
EFIAPI EFI_STATUS HookedQueryVariableInfo(UINT32 attributes, UINT64* maximumVariableStorageSize, UINT64* remainingVariableStorageSize, UINT64* maximumVariableSize) 
{
    return oQueryVariableInfo(attributes, maximumVariableStorageSize, remainingVariableStorageSize, maximumVariableSize);
}