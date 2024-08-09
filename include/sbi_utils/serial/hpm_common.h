/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_COMMON_H
#define _HPM_COMMON_H

/**
 *
 * @brief COMMON driver APIs
 * @defgroup common_interface COMMON driver APIs
 * @{
 *
 */

#define __R volatile const /* Define "read-only" permission */
#define __RW volatile      /* Define "read-write" permission */
#define __W volatile       /* Define "write-only" permission */

#ifndef __I
#define __I __R
#endif

#ifndef __IO
#define __IO __RW
#endif

#ifndef __O
#define __O __W
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef HPM_ALIGN_DOWN
#define HPM_ALIGN_DOWN(a, n)                    ((uint32_t)(a) & ~(n-1U))
#endif

#ifndef HPM_ALIGN_UP
#define HPM_ALIGN_UP(a, n)                      (((uint32_t)(a) + (n-1U)) & ~(n-1U))
#endif

#define HPM_BITSMASK(val, offset) ((uint32_t)(val) << (offset))
#define IS_HPM_BITMASK_SET(val, mask) (((uint32_t)(val) & (uint32_t)(mask)) != 0U)
#define IS_HPM_BIT_SET(val, offset) (((uint32_t)(val) & (1UL << (offset))) != 0U)
#define IS_HPM_BITMASK_CLR(val, mask) (((uint32_t)(val) & (uint32_t)(mask)) == 0U)
#define IS_HPM_BIT_CLR(val, offset) (((uint32_t)(val) & (1UL << (offset))) == 0U)

#define HPM_BREAK_IF(cond)   if (cond) { break; }
#define HPM_CONTINUE_IF(cond) if (cond) { continue; }

#define HPM_DIV_ROUND_CLOSEST(x, div) (((x) + (div) / 2) / (div))
#define HPM_DIV_ROUND_UP(x, div) (((x) + (div) - 1) / (div))

#define HPM_NUM_TO_EVEN_CEILING(x) ((x + 1) & 0xFFFFFFFEUL)
#define HPM_NUM_TO_EVEN_FLOOR(x) ((x) & 0xFFFFFFFEUL)

#define HPM_CHECK_RET(x)               \
    do {                               \
        stat = (x);                    \
        if (status_success != stat) { \
            return stat;               \
        }                              \
    } while (false)

#define SIZE_1KB (1024UL)
#define SIZE_1MB (1048576UL)

#define BIT0_MASK  (0x00000001UL)
#define BIT1_MASK  (0x00000002UL)
#define BIT2_MASK  (0x00000004UL)
#define BIT3_MASK  (0x00000008UL)
#define BIT4_MASK  (0x00000010UL)
#define BIT5_MASK  (0x00000020UL)
#define BIT6_MASK  (0x00000040UL)
#define BIT7_MASK  (0x00000080UL)
#define BIT8_MASK  (0x00000100UL)
#define BIT9_MASK  (0x00000200UL)
#define BIT10_MASK (0x00000400UL)
#define BIT11_MASK (0x00000800UL)
#define BIT12_MASK (0x00001000UL)
#define BIT13_MASK (0x00002000UL)
#define BIT14_MASK (0x00004000UL)
#define BIT15_MASK (0x00008000UL)
#define BIT16_MASK (0x00010000UL)
#define BIT17_MASK (0x00020000UL)
#define BIT18_MASK (0x00040000UL)
#define BIT19_MASK (0x00080000UL)
#define BIT20_MASK (0x00100000UL)
#define BIT21_MASK (0x00200000UL)
#define BIT22_MASK (0x00400000UL)
#define BIT23_MASK (0x00800000UL)
#define BIT24_MASK (0x01000000UL)
#define BIT25_MASK (0x02000000UL)
#define BIT26_MASK (0x04000000UL)
#define BIT27_MASK (0x08000000UL)
#define BIT28_MASK (0x10000000UL)
#define BIT29_MASK (0x20000000UL)
#define BIT30_MASK (0x40000000UL)
#define BIT31_MASK (0x80000000UL)

#define HPM_PI      (3.14159265358979323846)
#define HPM_2_PI    (6.28318530717958647692)
#define HPM_HALF_PI (1.57079632679489661923)

typedef uint32_t hpm_stat_t;

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Count bits set to 1
 *
 * @param value Data to be counted
 *
 * @return number of bits set to 1
 */
static inline uint32_t count_set_bits(uint32_t value)
{
    if (value == 0) {
        return 0;
    }
    return 1 + count_set_bits(value & (value - 1));
}

/**
 * @brief   Count bits set to 1 from least significant bit
 *
 * @param value Data to be counted
 *
 * @return number of bits set to 1
 * @return 0xFFFFFFFF if no bit was set to 1
 */
static inline uint32_t get_first_set_bit_from_lsb(uint32_t value)
{
    uint32_t i = 0;
    if (!value) {
        return 0xFFFFFFFFUL;
    }
    while (value && !(value & 0x1)) {
        value >>= 1;
        i++;
    }
    return i;
}

/**
 * @brief   Count bits set to 1 from most significant bit
 *
 * @param value Data to be counted
 *
 * @return number of bits set to 1
 * @return 0xFFFFFFFF if no bit was set to 1
 */
static inline uint32_t get_first_set_bit_from_msb(uint32_t value)
{
    uint32_t i = 31;
    if (!value) {
        return 0xFFFFFFFFUL;
    }
    while (value && !(value & 0x80000000)) {
        value <<= 1;
        value &= ~1;
        i--;
    }
    return i;
}

/**
 * @brief Convert the elapsed ticks to microseconds according to the source clock frequency
 * @param [in] ticks elapsed ticks
 * @param [in] src_clk_freq The Frequency of the source
 *
 * @return elapsed microseconds
 */
static inline uint32_t hpm_convert_ticks_to_us(uint32_t ticks, uint32_t src_clk_freq)
{
    uint32_t ticks_per_us = (src_clk_freq + 1000000UL - 1UL) / 1000000UL;
    return (ticks + ticks_per_us - 1UL) / ticks_per_us;
}

/**
 * @brief Convert the elapsed ticks to milliseconds according to the source clock frequency
 * @param [in] ticks elapsed ticks
 * @param [in] src_clk_freq The Frequency of the source
 *
 * @return elapsed milliseconds
 */
static inline uint32_t hpm_convert_ticks_to_ms(uint32_t ticks, uint32_t src_clk_freq)
{
    uint32_t ticks_per_ms = (src_clk_freq + 1000UL - 1UL) / 1000UL;
    return (ticks + ticks_per_ms - 1UL) / ticks_per_ms;
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* _HPM_COMMON_H */
