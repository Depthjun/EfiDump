#include "general.h"
#include "handler.h"
#include "nt.h"

void HandleCommand(InputCommand* cmd) 
{
    if (cmd->Action == 0) 
    {
        CopyMem(cmd->Data[0], cmd->Data[1], cmd->Data[2]);    
        return EFI_SUCCESS;
    }

    if (cmd->Action == 1) 
    {
        // data 0 -> size
        // data 1 -> source process id
        // data 2 -> target process id
        // data 3 -> source address
        // data 4 -> target address

        InputCommand lcmd;
        CopyMem(&lcmd, cmd, sizeof(InputCommand));
        
        ImpPsLookupProcessByProcessId psLookupProcessByProcessId = (ImpPsLookupProcessByProcessId)lcmd.Functions[IndexPsLookupProcessByProcessId];
        ImpKeAttachProcess keAttachProcess = (ImpKeAttachProcess)lcmd.Functions[IndexKeAttachProcess];
        ImpKeDetachProcess keDetachProcess = (ImpKeDetachProcess)lcmd.Functions[IndexKeDetachProcess];
        ImpExAllocatePool exAllocatePool = (ImpExAllocatePool)lcmd.Functions[IndexExAllocatePool];
        ImpExFreePoolWithTag exFreePoolWithTag = (ImpExFreePoolWithTag)lcmd.Functions[IndexExFreePoolWithTag];

        void* buffer = exAllocatePool(NonPagedPool, lcmd.Data[0]);
        if (!buffer)
            return;

        PEPROCESS sourceProcess;
        NTSTATUS status = psLookupProcessByProcessId(lcmd.Data[1], &sourceProcess);
        if (status)
            return;

        PEPROCESS targetProcess;
        status = psLookupProcessByProcessId(lcmd.Data[2], &targetProcess);
        if (status)
            return;

        keAttachProcess(sourceProcess);
        CopyMem(buffer, lcmd.Data[3], lcmd.Data[0]);
        keDetachProcess();

        keAttachProcess(targetProcess);
        CopyMem(lcmd.Data[4], buffer, lcmd.Data[0]);
        keDetachProcess();

        exFreePoolWithTag(buffer, 0);
    }
}