#ifndef NT_H
#define NT_H

#define MSABI __attribute__((ms_abi))

typedef void* HANDLE;
typedef int NTSTATUS;
typedef struct _KPROCESS *PEPROCESS;
typedef unsigned long long SIZE_T;

typedef enum _POOL_TYPE
{
    NonPagedPool
} POOL_TYPE;

typedef NTSTATUS (MSABI *ImpPsLookupProcessByProcessId)(HANDLE processID, PEPROCESS* process);
typedef void (MSABI *ImpKeAttachProcess)(PEPROCESS process);
typedef void (MSABI *ImpKeDetachProcess)();
typedef void* (MSABI *ImpExAllocatePool)(POOL_TYPE poolType, SIZE_T numberOfBytes);
typedef void (MSABI *ImpExFreePoolWithTag)(void* pool, unsigned int tag);

#endif