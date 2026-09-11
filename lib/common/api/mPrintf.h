/**
 * \file m_printf.h
 * \brief Minimal MISRA-friendly printf replacement using fixed parameter counts.
 *
 * Supported conversion specifiers: %c %s %d %u %x %p %ld %g and %%.
 * No <stdio.h> or <stdarg.h> is used.
 */
#ifndef M_PRINTF_H__
#define M_PRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define M_PRINTF_MAX_PARAMETERS (10U)

/* maximum output buffer for a single formatted string */
#define M_PRINTF_BUFFER_SIZE (256U)

/*
 * Since the standard C preprocessor can't count zero args reliably across
 * compiler settings, we avoid using m_snprintf_s/m_printf variadic dispatcher
 * for zero parameter calls in unit tests; explicit wrappers are available.
 */

#define M_PRINTF_CAT(a, b)           a##b
#define M_PRINTF_SELECT(name, count) M_PRINTF_CAT(name, count)


static inline uint64_t m_printf_doubleToU64(double i_val)
{
    union {
        double f;
        uint64_t u;
    } converter;

    converter.f = i_val;
    return converter.u;
}

static inline double m_printf_u64ToDouble(uint64_t i_val)
{
    union {
        double f;
        uint64_t u;
    } converter;

    converter.u = i_val;
    return converter.f;
}


/**
 * Output sink callback used by m_printf_x functions.
 * Argument is a single character output.
 */
typedef void (*pf_printf_output_t)(char i_char);

/**
 * Set output callback for m_printf_x family.
 * If callback is NULL, output is discarded.
 */
void m_printf_setOutput(pf_printf_output_t i_output_fn);

/**
 * Core implementation; use wrappers m_printf_0..m_printf_10 below.
 */
uint32_t mPrintf_private(const char* i_format, uint32_t i_parameter_count,
                         uint64_t i_p1, uint64_t i_p2, uint64_t i_p3,
                         uint64_t i_p4, uint64_t i_p5, uint64_t i_p6,
                         uint64_t i_p7, uint64_t i_p8, uint64_t i_p9,
                         uint64_t i_p10);

uint32_t mSnprintf_general(char* dest, uint32_t destsz, const char* i_format,
                           uint32_t i_parameter_count, uint64_t i_p1,
                           uint64_t i_p2, uint64_t i_p3, uint64_t i_p4,
                           uint64_t i_p5, uint64_t i_p6, uint64_t i_p7,
                           uint64_t i_p8, uint64_t i_p9, uint64_t i_p10);

#define m_snprintf_0(out, outsz, fmt)                                          \
    mSnprintf_general((out), (outsz), (fmt), 0U, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, \
                      0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_snprintf_1(out, outsz, fmt, p1)                                      \
    mSnprintf_general((out), (outsz), (fmt), 1U, (uint64_t)(p1), 0ULL, 0ULL,   \
                      0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_snprintf_2(out, outsz, fmt, p1, p2)                                  \
    mSnprintf_general((out), (outsz), (fmt), 2U, (uint64_t)(p1),               \
                      (uint64_t)(p2), 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,      \
                      0ULL, 0ULL)

#define m_snprintf_3(out, outsz, fmt, p1, p2, p3)                              \
    mSnprintf_general((out), (outsz), (fmt), 3U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), 0ULL, 0ULL, 0ULL, 0ULL,  \
                      0ULL, 0ULL, 0ULL)

#define m_snprintf_4(out, outsz, fmt, p1, p2, p3, p4)                          \
    mSnprintf_general((out), (outsz), (fmt), 4U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4), 0ULL,    \
                      0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_snprintf_5(out, outsz, fmt, p1, p2, p3, p4, p5)                      \
    mSnprintf_general((out), (outsz), (fmt), 5U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4),          \
                      (uint64_t)(p5), 0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_snprintf_6(out, outsz, fmt, p1, p2, p3, p4, p5, p6)                  \
    mSnprintf_general((out), (outsz), (fmt), 6U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4),          \
                      (uint64_t)(p5), (uint64_t)(p6), 0ULL, 0ULL, 0ULL, 0ULL)

#define m_snprintf_7(out, outsz, fmt, p1, p2, p3, p4, p5, p6, p7)              \
    mSnprintf_general((out), (outsz), (fmt), 7U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4),          \
                      (uint64_t)(p5), (uint64_t)(p6), (uint64_t)(p7), 0ULL,    \
                      0ULL, 0ULL)

#define m_snprintf_8(out, outsz, fmt, p1, p2, p3, p4, p5, p6, p7, p8)          \
    mSnprintf_general((out), (outsz), (fmt), 8U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4),          \
                      (uint64_t)(p5), (uint64_t)(p6), (uint64_t)(p7),          \
                      (uint64_t)(p8), 0ULL, 0ULL)

#define m_snprintf_9(out, outsz, fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9)      \
    mSnprintf_general((out), (outsz), (fmt), 9U, (uint64_t)(p1),               \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4),          \
                      (uint64_t)(p5), (uint64_t)(p6), (uint64_t)(p7),          \
                      (uint64_t)(p8), (uint64_t)(p9), 0ULL)

#define m_snprintf_10(out, outsz, fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9,     \
                      p10)                                                     \
    mSnprintf_general((out), (outsz), (fmt), 10U, (uint64_t)(p1),              \
                      (uint64_t)(p2), (uint64_t)(p3), (uint64_t)(p4),          \
                      (uint64_t)(p5), (uint64_t)(p6), (uint64_t)(p7),          \
                      (uint64_t)(p8), (uint64_t)(p9), (uint64_t)(p10))

/**
 * Polymorphic m_printf and m_snprintf wrappers are not available due to lack
 * of reliable portable variadic macro argument counting in this project.
 * Use explicit `m_printf_n` and `m_snprintf_n` macros instead.
 */

#define m_printf_0(fmt)                                                        \
    mPrintf_private((fmt), 0U, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, \
                    0ULL, 0ULL)

#define m_printf_1(fmt, p1)                                                    \
    mPrintf_private((fmt), 1U, (uint64_t)(p1), 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,   \
                    0ULL, 0ULL, 0ULL, 0ULL)

#define m_printf_2(fmt, p1, p2)                                                \
    mPrintf_private((fmt), 2U, (uint64_t)(p1), (uint64_t)(p2), 0ULL, 0ULL,     \
                    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_printf_3(fmt, p1, p2, p3)                                            \
    mPrintf_private((fmt), 3U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_printf_4(fmt, p1, p2, p3, p4)                                        \
    mPrintf_private((fmt), 4U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    (uint64_t)(p4), 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL)

#define m_printf_5(fmt, p1, p2, p3, p4, p5)                                    \
    mPrintf_private((fmt), 5U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    (uint64_t)(p4), (uint64_t)(p5), 0ULL, 0ULL, 0ULL, 0ULL,    \
                    0ULL)

#define m_printf_6(fmt, p1, p2, p3, p4, p5, p6)                                \
    mPrintf_private((fmt), 6U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    (uint64_t)(p4), (uint64_t)(p5), (uint64_t)(p6), 0ULL,      \
                    0ULL, 0ULL, 0ULL)

#define m_printf_7(fmt, p1, p2, p3, p4, p5, p6, p7)                            \
    mPrintf_private((fmt), 7U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    (uint64_t)(p4), (uint64_t)(p5), (uint64_t)(p6),            \
                    (uint64_t)(p7), 0ULL, 0ULL, 0ULL)

#define m_printf_8(fmt, p1, p2, p3, p4, p5, p6, p7, p8)                        \
    mPrintf_private((fmt), 8U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    (uint64_t)(p4), (uint64_t)(p5), (uint64_t)(p6),            \
                    (uint64_t)(p7), (uint64_t)(p8), 0ULL, 0ULL)

#define m_printf_9(fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9)                    \
    mPrintf_private((fmt), 9U, (uint64_t)(p1), (uint64_t)(p2), (uint64_t)(p3), \
                    (uint64_t)(p4), (uint64_t)(p5), (uint64_t)(p6),            \
                    (uint64_t)(p7), (uint64_t)(p8), (uint64_t)(p9), 0ULL)

#define m_printf_10(fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)              \
    mPrintf_private((fmt), 10U, (uint64_t)(p1), (uint64_t)(p2),                \
                    (uint64_t)(p3), (uint64_t)(p4), (uint64_t)(p5),            \
                    (uint64_t)(p6), (uint64_t)(p7), (uint64_t)(p8),            \
                    (uint64_t)(p9), (uint64_t)(p10))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* M_PRINTF_H__ */
