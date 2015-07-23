/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2015, Alexey Kramarenko
    All rights reserved.
*/

#ifndef LPC_I2C_H
#define LPC_I2C_H

#include "lpc_config.h"
#include "lpc_core.h"
#include "../../userspace/ipc.h"
#include "../../userspace/io.h"
#include "../../userspace/lpc/lpc_driver.h"

typedef enum {
    I2C_IO_MODE_IDLE = 0,
    I2C_IO_MODE_TX,
    I2C_IO_MODE_RX
} I2C_IO_MODE;

typedef struct  {
    IO* io;
    HANDLE block, process;
#if (LPC_I2C_TIMEOUT_MS)
    HANDLE timer;
#endif
    I2C_IO_MODE io_mode;
    unsigned int addr, rx_len;
    uint16_t mode;
    uint16_t size;
    uint8_t sla, addr_processed, rx_len_processed;
} I2C;

typedef struct  {
    I2C* i2cs[I2C_COUNT];
} I2C_DRV;

void lpc_i2c_init(CORE* core);
bool lpc_i2c_request(CORE* core, IPC* ipc);

#endif // LPC_I2C_H
