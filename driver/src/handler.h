#ifndef HANDLER_H
#define HANDLER_H

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

void HandleCommand(InputCommand* cmd);

#endif