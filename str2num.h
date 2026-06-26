/*****************************************************************************\
*                                                                             *
*  FILENAME :    str2num.h                                                    *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION : string to integer conversion functions of str2num.c          *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT :   (c) 2026 Dipl.-Ing. Klaus Lux (Aachen, Germany)              *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  ORIGIN :      https://github/klux21/str2num                                *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
* This software is provided 'as-is', without any express or implied           *
* warranty. In no event will the authors be held liable for any damages       *
* arising from the use of this software.                                      *
*                                                                             *
* Permission is granted to anyone to use this software for any purpose,       *
* including commercial applications, and to alter it and redistribute it      *
* freely, subject to the following restrictions:                              *
*                                                                             *
* 1. The origin of this software must not be misrepresented; you must not     *
*    claim that you wrote the original software. If you use this software     *
*    in a product, an acknowledgment in the product documentation would be    *
*    appreciated but is not required.                                         *
* 2. Altered source versions must be plainly marked as such, and must not be  *
*    misrepresented as being the original software.                           *
* 3. This notice may not be removed or altered from any source distribution.  *
*                                                                             *
\*****************************************************************************/


#ifndef STR2NUM_H
#define STR2NUM_H


#include <stddef.h>
#include <stdint.h>

#ifndef S2N_UMIN_IS_SMIN
/* If S2N_UMIN_IS_SMIN is nonzero then the minumum negative value that is accepted for
   unsigned values equals the minimum value of signed values to ensure a valid sign bit
   to be kept. It's very hard for applications to recognize a value as a negative one if
   the sign bit is lost and it's even more hard to deal with errors because of that.
   However the recent Unix standard specifies the functions that way and common compilers
   do implement stroul that way so the default here is 0 ... :o( */

#define S2N_UMIN_IS_SMIN 0
#endif

#ifndef S2N_USE_POW10_ARRAY
/* If S2N_USE_POW10_ARRAY is nonzero then precalculated arrays of powers of 10 are used
   for speeding up the reading of most common decimal numbers in the range 10^-99 - 10^99.
   However this requires about 5kB of static memory and can be disabled if the performance
   of decimal floating point numbers in that range doesn't count. */

#define S2N_USE_POW10_ARRAY 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* table for fast decoding ascii encoded numbers e.g. if (Digit[(uint8_t) c] < 10) i = Digit[(uint8_t) c]; */
extern const uint8_t digit_value[256];

/* The functions are full featured wrappers for strtol or strtoul like functions. 
   All functions return the minimum or maximum value of the integer type in case of overflows.  
   The r_* variants are reentrant save and return errors in *perr instead in errno.
   All functions support the prefixes 0b for dual values, 0o for octal numbers and 0x for hexadecimal values.
   If base is set to 1 than numbers with a leading 0 are recognized as decimal values and octal values require
   the prefix 0o for being recognized as octal values.
   Octal floating point values always require the prefix 0o for an automatic detection of that base. */

 int64_t str2i64_r (const char * ps, char ** pe, int base, int * perr);
uint64_t str2u64_r (const char * ps, char ** pe, int base, int * perr);
 int64_t str2i64   (const char * ps, char ** pe, int base);
uint64_t str2u64   (const char * ps, char ** pe, int base);

 int32_t str2i32_r (const char * ps, char ** pe, int base, int * perr);
uint32_t str2u32_r (const char * ps, char ** pe, int base, int * perr);
 int32_t str2i32   (const char * ps, char ** pe, int base);
uint32_t str2u32   (const char * ps, char ** pe, int base);

 int16_t str2i16_r (const char * ps, char ** pe, int base, int * perr);
uint16_t str2u16_r (const char * ps, char ** pe, int base, int * perr);
 int16_t str2i16   (const char * ps, char ** pe, int base);
uint16_t str2u16   (const char * ps, char ** pe, int base);

 int8_t  str2i8_r  (const char * ps, char ** pe, int base, int * perr);
uint8_t  str2u8_r  (const char * ps, char ** pe, int base, int * perr);
 int8_t  str2i8    (const char * ps, char ** pe, int base);
uint8_t  str2u8    (const char * ps, char ** pe, int base);

ptrdiff_t str2pd_r (const char * ps, char ** pe, int base, int * perr);
size_t    str2sz_r (const char * ps, char ** pe, int base, int * perr);
ptrdiff_t str2pd   (const char * ps, char ** pe, int base);
size_t    str2sz   (const char * ps, char ** pe, int base);

 intptr_t str2ip_r (const char * ps, char ** pe, int base, int * perr);
uintptr_t str2up_r (const char * ps, char ** pe, int base, int * perr);
 intptr_t str2ip   (const char * ps, char ** pe, int base);
uintptr_t str2up   (const char * ps, char ** pe, int base);

         int str2i_r (const char * ps, char ** pe, int base, int * perr);
unsigned int str2u_r (const char * ps, char ** pe, int base, int * perr);
         int str2i   (const char * ps, char ** pe, int base);
unsigned int str2u   (const char * ps, char ** pe, int base);

         long str2l_r  (const char * ps, char ** pe, int base, int * perr);
unsigned long str2ul_r (const char * ps, char ** pe, int base, int * perr);
         long str2l    (const char * ps, char ** pe, int base);
unsigned long str2ul   (const char * ps, char ** pe, int base);

         long long str2ll_r  (const char * ps, char ** pe, int base, int * perr);
unsigned long long str2ull_r (const char * ps, char ** pe, int base, int * perr);
         long long str2ll    (const char * ps, char ** pe, int base);
unsigned long long str2ull   (const char * ps, char ** pe, int base);


/* r_str2ld reads a long double from a string and cares about a specified base. */
long double str2ld_r (const char * psrc, char ** pend, int base, int * perr);

/* r_str2d reads a double from a string and cares about a specified base. */
double      str2d_r  (const char * psrc, char ** pend, int base, int * perr);

/* str2ld is a wrapper for strtold that calls r_str2ld for reading long doubles. */
long double str2ld   (const char * psrc, char ** pend);

/* str2d is a wrapper for strtod that calls r_str2d for reading doubles. */
double      str2d    (const char * psrc, char ** pend);

/* str2f is a wrapper for strtof that calls r_str2d for reading floats. */
float       str2f    (const char * psrc, char ** pend);


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* STR2NUM_H */

/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
