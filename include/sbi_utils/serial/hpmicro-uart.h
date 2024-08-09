/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SERIAL_HPMICRO_UART_H__
#define __SERIAL_HPMICRO_UART_H__
#include "hpm_common.h"
#include "hpm_uart_regs.h"
#include <sbi/sbi_types.h>

#ifdef HPM6800
/* UART related feature */
#define HPM_IP_FEATURE_UART_RX_IDLE_DETECT 1
#define HPM_IP_FEATURE_UART_FCRR 1
#define HPM_IP_FEATURE_UART_RX_EN 1
#endif
void hpimcro_uart_putc(char ch);

int hpimcro_uart_getc(void);

int hpimcro_uart_init(unsigned long base, u32 in_freq, u32 baudrate);

#endif
