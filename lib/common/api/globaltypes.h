/**
 * \file globaltypes.h
 * \brief global types
 */
#ifndef GLOBALTYPES_H__
#define GLOBALTYPES_H__
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <stdint.h>

#if __linux__
typedef int8_t char_t;
typedef uint8_t uchar_t;
typedef int int_t;
typedef unsigned int uint_t;
typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;

#else
typedef int8_t char_t;
typedef uint8_t uchar_t;
typedef int32_t int_t;
typedef uint32_t uint_t;
typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;

    /* test that these types match our platform */
    /* test our typedefs */
    #if ( __SIZEOF_INT__ != 4 )
        #error "Typedef for int is incorrect"
    #endif /* int */

    #if ( __SIZEOF_FLOAT__ != 4 )
        #error "Typedef for float is incorrect"
    #endif /* float */

    #if ( __SIZEOF_DOUBLE__ != 8 )
        #error "Typedef for double is incorrect"
    #endif /* double */

    #if ( __SIZEOF_LONG_DOUBLE__ != 16 )
        #error "Typedef for long double is incorrect"
    #endif /* long double */

    #if ( __CHAR_BIT__ != 8 )
        #error "char is not 8-bits"
    #endif /* bits in a char */

#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* GLOBALTYPES_H__ */
