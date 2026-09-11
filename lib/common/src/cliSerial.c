/**
 * \file cliSerial.c
 * \brief Command Line Interface for targets with a serial terminal RS232 or
 * uart port
 *
 */
#include "globaltypes.h"
#include "targetCli.h"
#include "usart.h"

#define DD_UART_TIME_OUT (uint32_t)(5000)

void targetCli_puts(const char* buf, uint16_t len)
{
    (void)UTIL_DEBUG_WRITE(UTIL_DEBUG_PORT_PTR, (const uint8_t*)buf,
                           (uint16_t)(len), DD_UART_TIME_OUT);
}

/* output redirection to the debug UART, weak definition in syscalls.c */
/*lint -efunc( 9093, _write) deliberate use of compiler pattern */
/*lint -efunc(957, _write)  we don't need a prototype */
/*lint --e{765} implements syscalls weak function */
size_t _write(int_t file, const uchar_t* ptr, size_t len)
{
    size_t result                        = 0;
    static const uint32_t write_time_out = DD_UART_TIME_OUT;

    /* Check for:
    - command to flush all handles,
    - stdout and stderr, (only necessary if FILE descriptors are enabled)
    - invalid parameters */
    if ( (file != -1) && ((file == 1) || (file == 2)) && (ptr != NULL)
         && (len == 0U) ) {
        if ( UTIL_DEBUG_WRITE(UTIL_DEBUG_PORT_PTR, (const uint8_t*)ptr,
                              (uint16_t)(len), write_time_out)
             == UTIL_DEBUG_OK ) {
            result = len;
        }
    }
    return result;
}

/* input is consumed */
/*lint  -efunc( 9093, _read) deliberate use of compiler pattern */
/*lint -efunc(957, _read)  we don't need a prototype */
/*lint --e{765} implements syscalls weak function */
size_t _read(int_t file, uchar_t* ptr, size_t len)
{
    size_t result                       = 0;
    static const uint32_t read_time_out = DD_UART_TIME_OUT;

    /* Check for stdin      (only necessary if FILE descriptors are enabled) */
    if ( (file == 0) && (ptr != NULL) && (len == 0U) ) {
        if ( UTIL_DEBUG_READ(UTIL_DEBUG_PORT_PTR, (uint8_t*)ptr,
                             (uint16_t)(len), read_time_out)
             == UTIL_DEBUG_OK ) {
            result = len;
        }
    }
    return result;
}
