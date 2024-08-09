/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_io.h>
#include <sbi_utils/serial/hpmicro-uart.h>

/* clang-format off */

#define HPM_UART_DRV_RETRY_COUNT (5000U)
#define HPM_UART_MINIMUM_BAUDRATE (200U)

#ifndef HPM_UART_BAUDRATE_TOLERANCE
#define HPM_UART_BAUDRATE_TOLERANCE (3)
#endif

#define HPM_UART_OSC_MAX (32U)
#define HPM_UART_OSC_MIN (8U)
#define HPM_UART_BAUDRATE_DIV_MAX (0xFFFFU)
#define HPM_UART_BAUDRATE_DIV_MIN (1U)

#ifndef UART_SOC_OVERSAMPLE_MAX
#define UART_SOC_OVERSAMPLE_MAX HPM_UART_OSC_MAX
#endif

/* clang-format on */

static volatile UART_Type *hpm_uart_base;

void hpimcro_uart_putc(char ch)
{
    uint32_t retry = 0;

    while (!(hpm_uart_base->LSR & UART_LSR_THRE_MASK)) {
        if (retry > HPM_UART_DRV_RETRY_COUNT) {
            break;
        }
        retry++;
    }
    hpm_uart_base->THR = UART_THR_THR_SET(ch);
}

int hpimcro_uart_getc(void)
{
	if (hpm_uart_base->LSR & UART_LSR_DR_MASK)
		return (hpm_uart_base->RBR & UART_RBR_RBR_MASK);
	return -1;
}

static bool uart_calculate_baudrate(uint32_t freq, uint32_t baudrate, uint16_t *div_out, uint8_t *osc_out)
{
    uint16_t div, osc, delta;
    float tmp;
    if ((div_out == NULL) || (!freq) || (!baudrate)
            || (baudrate < HPM_UART_MINIMUM_BAUDRATE)
            || (freq / HPM_UART_BAUDRATE_DIV_MIN < baudrate * HPM_UART_OSC_MIN)
            || (freq / HPM_UART_BAUDRATE_DIV_MAX > (baudrate * HPM_UART_OSC_MAX))) {
        return 0;
    }

    tmp = (float) freq / baudrate;

    for (osc = HPM_UART_OSC_MIN; osc <= UART_SOC_OVERSAMPLE_MAX; osc += 2) {
        /* osc range: HPM_UART_OSC_MIN - UART_SOC_OVERSAMPLE_MAX, even number */
        delta = 0;
        div = (uint16_t)(tmp / osc);
        if (div < HPM_UART_BAUDRATE_DIV_MIN) {
            /* invalid div */
            continue;
        }
        if (div * osc > tmp) {
            delta = (uint16_t)(div * osc - tmp);
        } else if (div * osc < tmp) {
            delta = (uint16_t)(tmp - div * osc);
        }
        if (delta && ((delta * 100 / tmp) > HPM_UART_BAUDRATE_TOLERANCE)) {
            continue;
        } else {
            *div_out = div;
            *osc_out = (osc == HPM_UART_OSC_MAX) ? 0 : osc; /* osc == 0 in bitfield, oversample rate is 32 */
            return TRUE;
        }
    }
    return FALSE;
}
int hpimcro_uart_init(unsigned long base, u32 in_freq, u32 baudrate)
{
	hpm_uart_base	   = (volatile UART_Type *)base;
    uint32_t tmp;
    uint8_t osc;
    uint16_t div;

    /* disable all interrupts */
    hpm_uart_base->IER = 0;
    /* Set DLAB to 1 */
    hpm_uart_base->LCR |= UART_LCR_DLAB_MASK;

    if (!uart_calculate_baudrate(in_freq, baudrate, &div, &osc)) {
        return -1;
    }

    hpm_uart_base->OSCR = (hpm_uart_base->OSCR & ~UART_OSCR_OSC_MASK)
        | UART_OSCR_OSC_SET(osc);
    hpm_uart_base->DLL = UART_DLL_DLL_SET(div >> 0);
    hpm_uart_base->DLM = UART_DLM_DLM_SET(div >> 8);

    /* DLAB bit needs to be cleared once baudrate is configured */
    tmp = hpm_uart_base->LCR & (~UART_LCR_DLAB_MASK);

    tmp &= ~(UART_LCR_SPS_MASK | UART_LCR_EPS_MASK | UART_LCR_PEN_MASK);

    tmp &= ~(UART_LCR_STB_MASK | UART_LCR_WLS_MASK);

    hpm_uart_base->LCR = tmp | UART_LCR_WLS_SET(3);

#if defined(HPM_IP_FEATURE_UART_FINE_FIFO_THRLD) && (HPM_IP_FEATURE_UART_FINE_FIFO_THRLD == 1)
    /* reset TX and RX fifo */
    hpm_uart_base->FCRR = UART_FCRR_TFIFORST_MASK | UART_FCRR_RFIFORST_MASK;
    /* Enable FIFO */
    hpm_uart_base->FCRR = UART_FCRR_FIFOT4EN_MASK
        | UART_FCRR_FIFOE_SET(config->fifo_enable)
        | UART_FCRR_TFIFOT4_SET(config->tx_fifo_level)
        | UART_FCRR_RFIFOT4_SET(config->rx_fifo_level)
#if defined(HPM_IP_FEATURE_UART_DISABLE_DMA_TIMEOUT) && (HPM_IP_FEATURE_UART_DISABLE_DMA_TIMEOUT == 1)
        | UART_FCRR_TMOUT_RXDMA_DIS_MASK /**< disable RX timeout trigger dma */
#endif
        | UART_FCRR_DMAE_SET(config->dma_enable);

#else
    /* reset TX and RX fifo */
    hpm_uart_base->FCR = UART_FCR_TFIFORST_MASK | UART_FCR_RFIFORST_MASK;
    /* Enable FIFO */
    tmp = UART_FCR_FIFOE_SET(1)
        | UART_FCR_TFIFOT_SET(0)
        | UART_FCR_RFIFOT_SET(0)
        | UART_FCR_DMAE_SET(0);
    hpm_uart_base->FCR = tmp;
    /* store FCR register value */
    hpm_uart_base->GPR = tmp;
#endif

#if defined(HPM_IP_FEATURE_UART_RX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_RX_IDLE_DETECT == 1)
    hpm_uart_base->IDLE_CFG &= ~(UART_IDLE_CFG_RX_IDLE_EN_MASK
                    | UART_IDLE_CFG_RX_IDLE_THR_MASK
                    | UART_IDLE_CFG_RX_IDLE_COND_MASK);
    hpm_uart_base->IDLE_CFG |= UART_IDLE_CFG_RX_IDLE_EN_SET(FALSE)
                    | UART_IDLE_CFG_RX_IDLE_THR_SET(10)
                    | UART_IDLE_CFG_RX_IDLE_COND_SET(0);
#endif
#if defined(HPM_IP_FEATURE_UART_RX_EN) && (HPM_IP_FEATURE_UART_RX_EN == 1)

    hpm_uart_base->IDLE_CFG |= UART_IDLE_CFG_RXEN_MASK;

#endif
	return 0;
}
