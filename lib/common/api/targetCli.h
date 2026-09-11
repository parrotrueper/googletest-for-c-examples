/**
 * \file targetCli.h
 * \brief provide command line interfaces based on platform and OS
 */
#ifndef TARGETCLI_H__
#define TARGETCLI_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#if defined(STM32G030xx) || defined(STM32H743xx)
    #include "stdint.h"
void targetCli_puts(const char* buf, uint16_t len);
#endif

#if DD_UNIT_TESTING
    #include <stdio.h>
    #define targetCli_puts(buff, len) puts(buff)
#endif /* __linux__ */

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* TARGETCLI_H__ */
