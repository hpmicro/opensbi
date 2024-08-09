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

#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/serial/hpmicro-uart.h>
#include <sbi_utils/irqchip/plic.h>
#include "platform.h"
#include "plmt.h"
#include "plicsw.h"

typedef struct {
	u32 base;
	u32 size;
	ulong prot;
} pmp_config_t;

pmp_config_t pmp_configs[] = {
#ifdef HPMICRO_AXISRAM_BASE
	{
		.base = HPMICRO_AXISRAM_BASE,
		.size = HPMICRO_AXISRAM_SIZE,
		.prot = PMP_R | PMP_W | PMP_X,
	},
#endif
	{
		.base = HPMICRO_SDRAM_BASE,
		.size = HPMICRO_SDRAM_SIZE,
		.prot = PMP_R | PMP_W | PMP_X,
	},
	{
		.base = HPMICRO_XPI_BASE,
		.size = HPMICRO_XPI_SIZE,
		.prot = PMP_R | PMP_X,
	},
	{
		.base = HPMICRO_PERIPH_BASE0,
		.size = HPMICRO_PERIPH_SIZE0,
		.prot = PMP_R | PMP_W,
	},
	{
		.base = HPMICRO_PERIPH_BASE1,
		.size = HPMICRO_PERIPH_SIZE1,
		.prot = PMP_R | PMP_W,
	},
};

static inline int calc_log2size(unsigned int size)
{
	int i = 0;
	for (i = 0; i < __riscv_xlen; i++) {
		if (size < (2 << i))
			break;
	}
	return i;
}

/* Platform final initialization. */
static int hpmicro_final_init(bool cold_boot)
{
	/* enable L1 cache */
	uintptr_t mcache_ctl_val = csr_read(CSR_MCACHECTL);

	if (!(mcache_ctl_val & V5_MCACHE_CTL_IC_EN))
		mcache_ctl_val |= V5_MCACHE_CTL_IC_EN;
	if (!(mcache_ctl_val & V5_MCACHE_CTL_DC_EN))
		mcache_ctl_val |= V5_MCACHE_CTL_DC_EN;
	if (!(mcache_ctl_val & V5_MCACHE_CTL_CCTL_SUEN))
		mcache_ctl_val |= V5_MCACHE_CTL_CCTL_SUEN;
	csr_write(CSR_MCACHECTL, mcache_ctl_val);

	if (!cold_boot)
		return 0;

	return 0;
}

/* Get number of PMP regions for given HART. */
static u32 hpmicro_pmp_region_count(u32 hartid)
{
	(void)hartid;
	return sizeof(pmp_configs) / sizeof(pmp_config_t);
}

/*
 * Get PMP regions details (namely: protection, base address, and size) for
 * a given HART.
 */
static int hpmicro_pmp_region_info(u32 hartid, u32 index, ulong *prot,
				 ulong *addr, ulong *log2size)
{
	if (index > hpmicro_pmp_region_count(hartid)) {
		return -1;
	}
	*prot = pmp_configs[index].prot;
	*addr = pmp_configs[index].base;
	*log2size = calc_log2size(pmp_configs[index].size);

	return 0;
}

/* Initialize the platform console. */
static int hpmicro_console_init(void)
{
	return hpimcro_uart_init(HPMICRO_UART_ADDR,
			     HPMICRO_UART_FREQUENCY,
			     HPMICRO_UART_BAUDRATE);
}

#ifdef HPMICRO_PLIC_NUM_SOURCES
/* Initialize the platform interrupt controller for current HART. */
static int hpmicro_irqchip_init(bool cold_boot)
{
	u32 hartid = sbi_current_hartid();
	int ret;

	if (cold_boot) {
		ret = plic_cold_irqchip_init(HPMICRO_PLIC_ADDR,
					     HPMICRO_PLIC_NUM_SOURCES,
					     HPMICRO_HART_COUNT);
		if (ret)
			return ret;
	}

	return plic_warm_irqchip_init(hartid, 2 * hartid, 2 * hartid + 1);
}

/* Initialize IPI for current HART. */
static int hpmicro_ipi_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = plicsw_cold_ipi_init(HPMICRO_PLICSW_ADDR,
					   HPMICRO_HART_COUNT);
		if (ret)
			return ret;
	}

	return plicsw_warm_ipi_init();
}
#endif
/* Initialize platform timer for current HART. */
static int hpmicro_timer_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = plmt_cold_timer_init(HPMICRO_PLMT_ADDR,
					   HPMICRO_HART_COUNT);
		if (ret)
			return ret;
	}

	return plmt_warm_timer_init();
}

/* Reboot the platform. */
static int hpmicro_system_reboot(u32 type)
{
	/* For now nothing to do. */
	sbi_printf("System reboot\n");
	return 0;
}

/* Shutdown or poweroff the platform. */
static int hpmicro_system_shutdown(u32 type)
{
	/* For now nothing to do. */
	sbi_printf("System shutdown\n");
	return 0;
}

/* Platform descriptor. */
const struct sbi_platform_operations platform_ops = {

	.final_init = hpmicro_final_init,

	.pmp_region_count = hpmicro_pmp_region_count,
	.pmp_region_info  = hpmicro_pmp_region_info,

	.console_init = hpmicro_console_init,
	.console_putc = hpimcro_uart_putc,
	.console_getc = hpimcro_uart_getc,
#ifdef HPMICRO_PLIC_NUM_SOURCES
	.irqchip_init = hpmicro_irqchip_init,
	.ipi_init     = hpmicro_ipi_init,
#endif
	.ipi_send     = plicsw_ipi_send,
	.ipi_clear    = plicsw_ipi_clear,

	.timer_init	   = hpmicro_timer_init,
	.timer_value	   = plmt_timer_value,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,

	.system_reboot	 = hpmicro_system_reboot,
	.system_shutdown = hpmicro_system_shutdown
};

const struct sbi_platform platform = {

	.opensbi_version = OPENSBI_VERSION,
	.platform_version = SBI_PLATFORM_VERSION(0x0, 0x01),
	.name = "HPMicro D45",
	.features = SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count = HPMICRO_HART_COUNT,
	.hart_stack_size = HPMICRO_HART_STACK_SIZE,
	.disabled_hart_mask = 0,
	.platform_ops_addr = (unsigned long)&platform_ops
};
