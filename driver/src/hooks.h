#ifndef HOOKS_H
#define HOOKS_H

#include "general.h"

extern BOOLEAN Virtual;
extern BOOLEAN Runtime;

extern EFI_SET_VARIABLE oSetVariable;
EFIAPI EFI_STATUS HookedSetVariable(CHAR16* variableName, EFI_GUID* vendorGuid, UINT32 attributes, UINTN dataSize, void* data);

extern EFI_GET_TIME oGetTime;
EFIAPI EFI_STATUS HookedGetTime(EFI_TIME* time, EFI_TIME_CAPABILITIES* capabilities);

extern EFI_SET_TIME oSetTime;
EFIAPI EFI_STATUS HookedSetTime(EFI_TIME* time);

extern EFI_GET_WAKEUP_TIME oGetWakeupTime;
EFIAPI EFI_STATUS HookedGetWakeupTime(BOOLEAN* enabled, BOOLEAN* pending, EFI_TIME* time);

extern EFI_SET_WAKEUP_TIME oSetWakeupTime;
EFIAPI EFI_STATUS HookedSetWakeupTime(BOOLEAN enable, EFI_TIME* time);

extern EFI_SET_VIRTUAL_ADDRESS_MAP oSetVirtualAddressMap;
EFIAPI EFI_STATUS HookedSetVirtualAddressMap(UINTN mapSize, UINTN descriptorSize, UINT32 version, EFI_MEMORY_DESCRIPTOR* virtualMap);

extern EFI_CONVERT_POINTER oConvertPointer;
EFIAPI EFI_STATUS HookedConvertPointer(UINTN debug, void** address);

extern EFI_GET_VARIABLE oGetVariable;
EFIAPI EFI_STATUS HookedGetVariable(CHAR16* variableName, EFI_GUID* vendorGuid, UINT32* attributes, UINTN* dataSize, void* data);

extern EFI_GET_NEXT_VARIABLE_NAME oGetNextVariableName;
EFIAPI EFI_STATUS HookedGetNextVariableName(UINTN* variableNameSize, CHAR16* variableName, EFI_GUID* vendorGuid);

extern EFI_GET_NEXT_HIGH_MONO_COUNT oGetNextHighMonotonicCount;
EFIAPI EFI_STATUS HookedGetNextHighMonotonicCount(UINT32* highCount);

extern EFI_RESET_SYSTEM oResetSystem;
EFIAPI EFI_STATUS HookedResetSystem(EFI_RESET_TYPE resetType, EFI_STATUS resetStatus, UINTN dataSize, CHAR16* resetData);

extern EFI_UPDATE_CAPSULE oUpdateCapsule;
EFIAPI EFI_STATUS HookedUpdateCapsule(EFI_CAPSULE_HEADER** capsuleHeaderArray, UINTN capsuleCount, EFI_PHYSICAL_ADDRESS scatterGatherList);

extern EFI_QUERY_CAPSULE_CAPABILITIES oQueryCapsuleCapabilities;
EFIAPI EFI_STATUS HookedQueryCapsuleCapabilities(EFI_CAPSULE_HEADER** capsuleHeaderArray, UINTN capsuleCount, UINT64* maximumCapsuleSize, EFI_RESET_TYPE* resetType);

extern EFI_QUERY_VARIABLE_INFO oQueryVariableInfo;
EFIAPI EFI_STATUS HookedQueryVariableInfo(UINT32 attributes, UINT64* maximumVariableStorageSize, UINT64* remainingVariableStorageSize, UINT64* maximumVariableSize);

#endif