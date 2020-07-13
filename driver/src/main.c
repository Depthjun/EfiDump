#include "general.h"
#include "hooks.h"

static const EFI_GUID ProtocolGuid
    = { 0x2a479eea, 0x0acf, 0x5a46, {0x96, 0x24, 0x8f, 0x14, 0xed, 0x92, 0xd9, 0xec} };

// gEfiEventVirtualAddressChangeGuid
static const EFI_GUID VirtualGuid
    = { 0x13FA7698, 0xC831, 0x49C7, { 0x87, 0xEA, 0x8F, 0x43, 0xFC, 0xC2, 0x51, 0x96 }};

// gEfiEventExitBootServicesGuid
static const EFI_GUID ExitGuid
    = { 0x27ABF055, 0xB1B8, 0x4C26, { 0x80, 0x48, 0x74, 0x8F, 0x37, 0xBA, 0xA2, 0xDF }};

typedef struct _DummyProtocalData
{
    UINTN Blank;
} DummyProtocalData;

static EFI_EVENT NotifyEvent = NULL;
static EFI_EVENT ExitEvent = NULL;

void EFIAPI SetVirtualAddressMapEvent(EFI_EVENT ivent, void* context)
{    
    RT->ConvertPointer(0, (void**)&oGetTime);
    RT->ConvertPointer(0, (void**)&oSetTime);
    RT->ConvertPointer(0, (void**)&oGetWakeupTime);
    RT->ConvertPointer(0, (void**)&oSetWakeupTime);
    RT->ConvertPointer(0, (void**)&oSetVirtualAddressMap);
    RT->ConvertPointer(0, (void**)&oConvertPointer);
    RT->ConvertPointer(0, (void**)&oGetVariable);
    RT->ConvertPointer(0, (void**)&oGetNextVariableName);
    RT->ConvertPointer(0, (void**)&oSetVariable);
    RT->ConvertPointer(0, (void**)&oGetNextHighMonotonicCount);
    RT->ConvertPointer(0, (void**)&oResetSystem);
    RT->ConvertPointer(0, (void**)&oUpdateCapsule);
    RT->ConvertPointer(0, (void**)&oQueryCapsuleCapabilities);
    RT->ConvertPointer(0, (void**)&oQueryVariableInfo);
    
    RtLibEnableVirtualMappings();

    NotifyEvent = NULL;
    Virtual = TRUE;
}

void EFIAPI ExitBootServicesEvent(EFI_EVENT event, void* context)
{
    BS->CloseEvent(ExitEvent);
	ExitEvent = NULL;

    BS = NULL;
    Runtime = TRUE;

    ST->ConOut->SetAttribute(ST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLUE);
	ST->ConOut->ClearScreen(ST->ConOut);
}

void* SetServicePointer(EFI_TABLE_HEADER *serviceTableHeader, void** serviceTableFunction, void* NewFunction)
{
    if (serviceTableFunction == NULL || NewFunction == NULL || *serviceTableFunction == NULL)
        return NULL;

    ASSERT(BS != NULL);
    ASSERT(BS->CalculateCrc32 != NULL);

    CONST EFI_TPL Tpl = BS->RaiseTPL(TPL_HIGH_LEVEL);

    VOID* OriginalFunction = *serviceTableFunction;
    *serviceTableFunction = NewFunction;

    serviceTableHeader->CRC32 = 0;
    BS->CalculateCrc32((UINT8*)serviceTableHeader, serviceTableHeader->HeaderSize, &serviceTableHeader->CRC32);

    BS->RestoreTPL(Tpl);

    return OriginalFunction;
}

EFI_STATUS EFI_FUNCTION EfiUnload(EFI_HANDLE imageHandle)
{
    return EFI_ACCESS_DENIED;
}

EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) 
{ 
    InitializeLib(imageHandle, systemTable);

    EFI_LOADED_IMAGE *loadedImage = NULL;
    EFI_STATUS status = BS->OpenProtocol(imageHandle, &LoadedImageProtocol,
                                        (void**)&loadedImage, imageHandle,
                                        NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    if (EFI_ERROR(status)) 
    {
        Print(L"Can't open protocol: %d\n", status);
        return status;
    }
    
    DummyProtocalData dummy = { 0 };
    status = LibInstallProtocolInterfaces(&imageHandle, &ProtocolGuid, &dummy, NULL);

    if (EFI_ERROR(status)) 
    {
        Print(L"Can't register interface: %d\n", status);
        return status;
    }

    loadedImage->Unload = (EFI_IMAGE_UNLOAD)EfiUnload;

    status = BS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                                TPL_NOTIFY,
                                SetVirtualAddressMapEvent,
                                NULL,
                                VirtualGuid,
                                &NotifyEvent);

    if (EFI_ERROR(status)) 
    {
        Print(L"Can't create event (SetVirtualAddressMapEvent): %d\n", status);
        return status;
    }

    status = BS->CreateEventEx(EVT_NOTIFY_SIGNAL,
                                TPL_NOTIFY,
                                ExitBootServicesEvent,
                                NULL,
                                ExitGuid,
                                &ExitEvent);

    if (EFI_ERROR(status)) 
    {
        Print(L"Can't create event (ExitBootServicesEvent): %d\n", status);
        return status;
    }

    // I know that \t for tab exists, but since the text is different length
    // and I don't want to spend entire day trying out how many tabs I need for it
    // to fit, this is it.

    Print(L"Hooking GetTime...");
    oGetTime = (EFI_GET_TIME)SetServicePointer(&RT->Hdr, (VOID**)&RT->GetTime, (VOID**)&HookedGetTime);
    Print(L"                                  done\n");

    Print(L"Hooking SetTime...");
    oSetTime = (EFI_SET_TIME)SetServicePointer(&RT->Hdr, (VOID**)&RT->SetTime, (VOID**)&HookedSetTime);
    Print(L"                                  done\n");

    Print(L"Hooking GetWakeupTime...");
    oGetWakeupTime = (EFI_GET_WAKEUP_TIME)SetServicePointer(&RT->Hdr, (VOID**)&RT->GetWakeupTime, (VOID**)&HookedGetWakeupTime);
    Print(L"                            done\n");

    Print(L"Hooking SetWakeupTime...");
    oSetWakeupTime = (EFI_SET_WAKEUP_TIME)SetServicePointer(&RT->Hdr, (VOID**)&RT->SetWakeupTime, (VOID**)&HookedSetWakeupTime);
    Print(L"                            done\n");

    Print(L"Hooking SetVirtualAddressMap...");
    oSetVirtualAddressMap = (EFI_SET_VIRTUAL_ADDRESS_MAP)SetServicePointer(&RT->Hdr, (VOID**)&RT->SetVirtualAddressMap, (VOID**)&HookedSetVirtualAddressMap);
    Print(L"                     done\n");

    Print(L"Hooking ConvertPointer...");
    oConvertPointer = (EFI_CONVERT_POINTER)SetServicePointer(&RT->Hdr, (VOID**)&RT->ConvertPointer, (VOID**)&HookedConvertPointer);
    Print(L"                           done\n");

    Print(L"Hooking GetVariable...");
    oGetVariable = (EFI_GET_VARIABLE)SetServicePointer(&RT->Hdr, (VOID**)&RT->GetVariable, (VOID**)&HookedGetVariable);
    Print(L"                              done\n");

    Print(L"Hooking GetNextVariableName...");
    oGetNextVariableName = (EFI_GET_NEXT_VARIABLE_NAME)SetServicePointer(&RT->Hdr, (VOID**)&RT->GetNextVariableName, (VOID**)&HookedGetNextVariableName);
    Print(L"                      done\n");

    Print(L"Hooking SetVariable...");
    oSetVariable = (EFI_SET_VARIABLE)SetServicePointer(&RT->Hdr, (VOID**)&RT->SetVariable, (VOID**)&HookedSetVariable);
    Print(L"                              done\n");

    Print(L"Hooking GetNextHighMonotonicCount...");
    oGetNextHighMonotonicCount = (EFI_GET_NEXT_HIGH_MONO_COUNT)SetServicePointer(&RT->Hdr, (VOID**)&RT->GetNextHighMonotonicCount, (VOID**)&HookedGetNextHighMonotonicCount);
    Print(L"                done\n");

    Print(L"Hooking ResetSystem...");
    oResetSystem = (EFI_RESET_SYSTEM)SetServicePointer(&RT->Hdr, (VOID**)&RT->ResetSystem, (VOID**)&HookedResetSystem);
    Print(L"                              done\n");

    Print(L"Hooking UpdateCapsule...");
    oUpdateCapsule = (EFI_UPDATE_CAPSULE)SetServicePointer(&RT->Hdr, (VOID**)&RT->UpdateCapsule, (VOID**)&HookedUpdateCapsule);
    Print(L"                            done\n");

    Print(L"Hooking QueryCapsuleCapabilities...");
    oQueryCapsuleCapabilities = (EFI_QUERY_CAPSULE_CAPABILITIES)SetServicePointer(&RT->Hdr, (VOID**)&RT->QueryCapsuleCapabilities, (VOID**)&HookedQueryCapsuleCapabilities);
    Print(L"                 done\n");

    Print(L"Hooking QueryVariableInfo...");
    oQueryVariableInfo = (EFI_QUERY_VARIABLE_INFO)SetServicePointer(&RT->Hdr, (VOID**)&RT->QueryVariableInfo, (VOID**)&HookedQueryVariableInfo);
    Print(L"                        done\n");

    Print(L"\n");
    Print(L"EfiDump\n");
    Print(L"Build on %a\n", __DATE__);

    return 0;
}
