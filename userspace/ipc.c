/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2015, Alexey Kramarenko
    All rights reserved.
*/

#include "ipc.h"
#include "heap.h"
#include "svc.h"
#include "error.h"

void ipc_post(IPC* ipc)
{
    svc_call(SVC_IPC_POST, (unsigned int)ipc, 0, 0);
}

void ipc_post_inline(HANDLE process, unsigned int cmd, unsigned int param1, unsigned int param2, unsigned int param3)
{
    IPC ipc;
    ipc.process = process;
    ipc.cmd = cmd;
    ipc.param1 = param1;
    ipc.param2 = param2;
    ipc.param3 = param3;
    svc_call(SVC_IPC_POST, (unsigned int)&ipc, 0, 0);
}

void ipc_post_or_error(IPC* ipc)
{
    ipc->param3 = get_last_error();
    svc_call(SVC_IPC_POST, (unsigned int)ipc, 0, 0);
}

void ipc_ipost(IPC* ipc)
{
    __GLOBAL->svc_irq(SVC_IPC_POST, (unsigned int)ipc, 0, 0);
}

bool ipc_read(IPC* ipc, TIME* timeout, HANDLE wait_process)
{
    svc_call(SVC_IPC_READ, (unsigned int)ipc, (unsigned int)timeout, wait_process);
    return get_last_error() == ERROR_OK;
}

bool ipc_read_ms(IPC* ipc, unsigned int ms, HANDLE wait_process)
{
    TIME timeout;
    ms_to_time(ms, &timeout);
    return ipc_read(ipc, &timeout, wait_process);
}

void ipc_call(IPC* ipc, TIME* timeout)
{
    svc_call(SVC_IPC_CALL, (unsigned int)ipc, (unsigned int)timeout, 0);
}

void ipc_call_ms(IPC* ipc, unsigned int ms)
{
    TIME timeout;
    ms_to_time(ms, &timeout);
    ipc_call(ipc, &timeout);
}

bool call(IPC* ipc)
{
    ipc_call_ms(ipc, 0);
    if (ipc->param3 >= 0)
        return true;
    error(ipc->param3);
    return false;
}

bool ack(HANDLE process, unsigned int cmd, unsigned int param1, unsigned int param2, unsigned int param3)
{
    IPC ipc;
    ipc.cmd = cmd;
    ipc.process = process;
    ipc.param1 = param1;
    ipc.param2 = param2;
    ipc.param3 = param3;
    return call(&ipc);
}

unsigned int get(HANDLE process, unsigned int cmd, unsigned int param1, unsigned int param2, unsigned int param3)
{
    IPC ipc;
    ipc.cmd = cmd;
    ipc.process = process;
    ipc.param1 = param1;
    ipc.param2 = param2;
    ipc.param3 = param3;
    if (call(&ipc))
        return ipc.param2;
    return INVALID_HANDLE;
}
