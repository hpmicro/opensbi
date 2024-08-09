/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Andes Technology Corporation
 *               2024 HPMicro
 *
 * Authors:
 *   Zong Li <zong@andestech.com>
 *   Nylon Chen <nylon7@andestech.com>
 *   Zihan Xu <zihan.xu@hpmicro.com>
 */

#ifndef _HPMICRO_PLATFORM_H_
#define _HPMICRO_PLATFORM_H_
#define HPMICRO_HART_COUNT		1
#define HPMICRO_HART_STACK_SIZE		8192

#ifdef HPM6800
/* PMP Config Definetions */
#define HPMICRO_AXISRAM_BASE        0x01200000
#define HPMICRO_AXISRAM_SIZE        (512 * 1024)
#elif defined(HPM6E00)
#define HPMICRO_AXISRAM_BASE        0x01080000
#define HPMICRO_AXISRAM_SIZE        (1024 * 1024)
#elif defined(HPM6P00)
#define HPMICRO_AXISRAM_BASE        0x01080000
#define HPMICRO_AXISRAM_SIZE        (1024 * 1024)
#elif defined(HPM6300)
#define HPMICRO_AXISRAM_BASE        0x01080000
#define HPMICRO_AXISRAM_SIZE        (512 * 1024)
#endif
#define HPMICRO_SDRAM_BASE          0x40000000
#define HPMICRO_SDRAM_SIZE          (32 * 1024 * 1024)

#define HPMICRO_XPI_BASE            0x80000000
#define HPMICRO_XPI_SIZE            0x20000000

#define HPMICRO_PERIPH_BASE0        0xE0000000
#define HPMICRO_PERIPH_SIZE0        0x10000000

#define HPMICRO_PERIPH_BASE1        0xF0000000
#define HPMICRO_PERIPH_SIZE1        0x10000000
/* End of PMP Config Definetions */

#define HPMICRO_PLIC_ADDR			0xe4000000

#ifdef HPM6800
#define HPMICRO_PLIC_NUM_SOURCES		71
#elif defined(HPM6E00)
#define HPMICRO_PLIC_NUM_SOURCES		161
#elif defined(HPM6P00)
#define HPMICRO_PLIC_NUM_SOURCES		110
#elif defined(HPM6300)
#define HPMICRO_PLIC_NUM_SOURCES		115
#endif

#define HPMICRO_PLICSW_ADDR		    0xe6400000

#define HPMICRO_PLMT_ADDR			0xe6000000

#define HPMICRO_UART_ADDR			(0xf0040000)
#define HPMICRO_UART_FREQUENCY		24000000
#define HPMICRO_UART_BAUDRATE		115200
#define HPMICRO_UART_REG_SHIFT		2
#define HPMICRO_UART_REG_WIDTH		2

/* nds mcache_ctl register*/
#define CSR_MCACHECTL			0x7ca

#define V5_MCACHE_CTL_IC_EN_OFFSET      0
#define V5_MCACHE_CTL_DC_EN_OFFSET      1
#define V5_MCACHE_CTL_IC_ECCEN_OFFSET   2
#define V5_MCACHE_CTL_DC_ECCEN_OFFSET   4
#define V5_MCACHE_CTL_IC_RWECC_OFFSET   6
#define V5_MCACHE_CTL_DC_RWECC_OFFSET   7
#define V5_MCACHE_CTL_CCTL_SUEN_OFFSET  8

#define V5_MCACHE_CTL_IC_EN     (1UL << V5_MCACHE_CTL_IC_EN_OFFSET)
#define V5_MCACHE_CTL_DC_EN     (1UL << V5_MCACHE_CTL_DC_EN_OFFSET)
#define V5_MCACHE_CTL_IC_RWECC  (1UL << V5_MCACHE_CTL_IC_RWECC_OFFSET)
#define V5_MCACHE_CTL_DC_RWECC  (1UL << V5_MCACHE_CTL_DC_RWECC_OFFSET)
#define V5_MCACHE_CTL_CCTL_SUEN (1UL << V5_MCACHE_CTL_CCTL_SUEN_OFFSET)

#endif /* _HPMICRO_PLATFORM_H_ */
