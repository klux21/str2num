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
/* S2N_UMIN_IS_SMIN is nonzero then the minumum negative value that is accepted for
   unsigned values equals the minimum value of signed values to ensure a valid sign bit
   to be kept. It very hard for applications to recognize a value as a negative one if
   the sign bit is lost and it's even more hard to deal with errors because of that.
   However the recent Unix standard specifies the functions that way and common compilers
   do implement stroul that way ... :o( */
#define S2N_UMIN_IS_SMIN 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* table for fast decoding ascii encoded numbers e.g. if (Digit[(uint8_t) c] < 10) i = Digit[(uint8_t) c]; */
extern const uint8_t digit_value[256];

/* The functions are full featured wrappers for strtol or strtoul like functions. 
   All functions return the minimum or maximum value of the integer type in case of overflows.  
   The r_* variants are reentrant save and return errors in *perr instead in errno. */

 int64_t r_str2i64 (const char * ps, char ** pe, int base, int * perr);
uint64_t r_str2u64 (const char * ps, char ** pe, int base, int * perr);
 int64_t str2i64   (const char * ps, char ** pe, int base);
uint64_t str2u64   (const char * ps, char ** pe, int base);

 int32_t r_str2i32 (const char * ps, char ** pe, int base, int * perr);
uint32_t r_str2u32 (const char * ps, char ** pe, int base, int * perr);
 int32_t str2i32   (const char * ps, char ** pe, int base);
uint32_t str2u32   (const char * ps, char ** pe, int base);

 int16_t r_str2i16 (const char * ps, char ** pe, int base, int * perr);
uint16_t r_str2u16 (const char * ps, char ** pe, int base, int * perr);
 int16_t str2i16   (const char * ps, char ** pe, int base);
uint16_t str2u16   (const char * ps, char ** pe, int base);

 int8_t r_str2i8 (const char * ps, char ** pe, int base, int * perr);
uint8_t r_str2u8 (const char * ps, char ** pe, int base, int * perr);
 int8_t str2i8   (const char * ps, char ** pe, int base);
uint8_t str2u8   (const char * ps, char ** pe, int base);

ptrdiff_t r_str2sz (const char * ps, char ** pe, int base, int * perr);
size_t    r_str2uz (const char * ps, char ** pe, int base, int * perr);
ptrdiff_t str2sz   (const char * ps, char ** pe, int base);
size_t    str2uz   (const char * ps, char ** pe, int base);

         int r_str2i (const char * ps, char ** pe, int base, int * perr);
unsigned int r_str2u (const char * ps, char ** pe, int base, int * perr);
         int str2i   (const char * ps, char ** pe, int base);
unsigned int str2u   (const char * ps, char ** pe, int base);

         long r_str2l  (const char * ps, char ** pe, int base, int * perr);
unsigned long r_str2ul (const char * ps, char ** pe, int base, int * perr);
         long str2l    (const char * ps, char ** pe, int base);
unsigned long str2ul   (const char * ps, char ** pe, int base);

         long long r_str2ll  (const char * ps, char ** pe, int base, int * perr);
unsigned long long r_str2ull (const char * ps, char ** pe, int base, int * perr);
         long long str2ll    (const char * ps, char ** pe, int base);
unsigned long long str2ull   (const char * ps, char ** pe, int base);


/* r_str2ld reads a long double from a string and cares about a specified base. */
long double r_str2ld (const char * psrc, char ** pend, int base, int * perr);

/* r_str2d reads a double from a string and cares about a specified base. */
double      r_str2d  (const char * psrc, char ** pend, int base, int * perr);

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
