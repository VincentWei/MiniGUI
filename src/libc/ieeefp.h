
#ifdef linux
#include <endian.h>
#endif

#if (__BYTE_ORDER == __BIG_ENDIAN) /* Big endian */

/*
** Note: there do not seem to be any current machines which are Big Endian but
** have a mixed up double layout. 
*/

typedef union 
{
    Sint32 asi32[2];

    Sint64 asi64;
    
    double value;
    
    struct 
    {
        unsigned int sign : 1;
        unsigned int exponent: 11;
        unsigned int fraction0:4;
        unsigned int fraction1:16;
        unsigned int fraction2:16;
        unsigned int fraction3:16;
        
    } number;
    
    struct 
    {
        unsigned int sign : 1;
        unsigned int exponent: 11;
        unsigned int quiet:1;
        unsigned int function0:3;
        unsigned int function1:16;
        unsigned int function2:16;
        unsigned int function3:16;
    } nan;
    
    struct 
    {
        Uint32 msw;
        Uint32 lsw;
    } parts;

    
} _libm_ieee_double_shape_type;


typedef union
{
    Sint32 asi32;
    
    float value;

    struct 
    {
        unsigned int sign : 1;
        unsigned int exponent: 8;
        unsigned int fraction0: 7;
        unsigned int fraction1: 16;
    } number;

    struct 
    {
        unsigned int sign:1;
        unsigned int exponent:8;
        unsigned int quiet:1;
        unsigned int function0:6;
        unsigned int function1:16;
    } nan;
    
} _libm_ieee_float_shape_type;


#else /* Little endian */

typedef union 
{
    Sint32 asi32[2];

    Sint64 asi64;
    
    double value;

    struct 
    {
#if (__FLOAT_WORD_ORDER == __BIG_ENDIAN) // Big endian
        unsigned int fraction1:16;
        unsigned int fraction0: 4;
        unsigned int exponent :11;
        unsigned int sign     : 1;
        unsigned int fraction3:16;
        unsigned int fraction2:16;
#else
        unsigned int fraction3:16;
        unsigned int fraction2:16;
        unsigned int fraction1:16;
        unsigned int fraction0: 4;
        unsigned int exponent :11;
        unsigned int sign     : 1;
#endif
    } number;

    struct 
    {
#if (__FLOAT_WORD_ORDER == __BIG_ENDIAN) // Big endian
        unsigned int function1:16;
        unsigned int function0:3;
        unsigned int quiet:1;
        unsigned int exponent: 11;
        unsigned int sign : 1;
        unsigned int function3:16;
        unsigned int function2:16;
#else
        unsigned int function3:16;
        unsigned int function2:16;
        unsigned int function1:16;
        unsigned int function0:3;
        unsigned int quiet:1;
        unsigned int exponent: 11;
        unsigned int sign : 1;
#endif
    } nan;

    struct 
    {
#if (__FLOAT_WORD_ORDER == __BIG_ENDIAN) // Big endian
        Uint32 msw;
        Uint32 lsw;
#else
        Uint32 lsw;
        Uint32 msw;
#endif
    } parts;
    
} _libm_ieee_double_shape_type;


typedef union
{
    Sint32 asi32;
  
    float value;

    struct 
    {
        unsigned int fraction0: 7;
        unsigned int fraction1: 16;
        unsigned int exponent: 8;
        unsigned int sign : 1;
    } number;

    struct 
    {
        unsigned int function1:16;
        unsigned int function0:6;
        unsigned int quiet:1;
        unsigned int exponent:8;
        unsigned int sign:1;
    } nan;

} _libm_ieee_float_shape_type;

#endif /* little-endian */

