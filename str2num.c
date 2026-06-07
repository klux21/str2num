/*****************************************************************************\
*                                                                             *
*  FILENAME :    str2num.c                                                    *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION : string to integer conversion functions                       *
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


#ifndef STR2NUM_C
#define STR2NUM_C

#include <errno.h>

#include <stdlib.h> /* strtod */
#include <string.h> /* strnicmp */

#include <str2num.h>

/* table for fast decoding ascii encoded numbers e.g. if (Digit[c] < 10) i = Digit[c]; */
const uint8_t digit_value[256] = { 64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                               /*      !  "  #  $   %  &  '   (  )  *  +   ,  -  .  /     0  1  2  3   4  5  6  7   8  9  :  ;   <  =  >  ?  */
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,    0, 1, 2, 3,  4, 5, 6, 7,  8, 9,64,64, 64,64,64,64,
                               /*   @  A  B  C   D  E  F  G   H  I  J  K   L  M  N  O     P  Q  R  S   T  U  V  W   X  Y  Z  [   \  ]  ^  _  */
                                   64,10,11,12, 13,14,15,16, 17,18,19,20, 21,22,23,24,   25,26,27,28, 29,30,31,32, 33,34,35,64, 64,64,64,64,
                               /*   `  a  b  c   d  e  f  g   h  i  j  k   l  m  n  o     p  q  r  s   t  u  v  w   x  y  z  {   |  }  ~     */
                                   64,10,11,12, 13,14,15,16, 17,18,19,20, 21,22,23,24,   25,26,27,28, 29,30,31,32, 33,34,35,64, 64,64,64,64,

                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64 };


/* ========================================================================= *\
   Floating point conversion functions
\* ========================================================================= */

static uint32_t inf   = 0x7f800000;
static uint32_t ninf  = 0xff800000;
static uint32_t nan   = 0x7fc00000;
static uint32_t nnan  = 0xffc00000;
static void *   pvinf  = &inf;
static void *   pvninf = &ninf;
static void *   pvnan  = &nan;
static void *   pvnnan = &nnan;


/* ------------------------------------------------------------------------- *\
   powil calculates a power of the base and returns it as a long double.
   See comments of rebase() in callback_printf for details.
\* ------------------------------------------------------------------------- */

static long double powil (uint8_t base, uint32_t expo)
{
   long double val = 1.0;

   if (expo)
   {
      uint64_t v = 1;
      uint64_t p = base;

      if (expo & 1)
         v = p;

      while (expo >>= 1)
      {
         p *= p;

         if (p > 0xffffffff)
         {
            long double fp = (long double) p;

            if (expo & 1)
               val = fp * v;
            else
               val = (long double) v;

            while(expo >>= 1)
            {
               fp *= fp;

               if (expo & 1)
                  val *= fp;
            }
            return (val);
         }

         if (expo & 1)
            v *= p;
      }

      val = (long double) v;
   }

   return (val);
} /* long double powil (uint8_t base, uint32_t expo) */


/* ------------------------------------------------------------------------- *\
   powi calculates a power of the base and returns it as a double.
   See comments of rebase() in callback_printf.c for details.
\* ------------------------------------------------------------------------- */

static double powi (uint8_t base, uint32_t expo)
{
   double val = 1.0;

   if (expo)
   {
      uint64_t v = 1;
      uint64_t p = base;

      if (expo & 1)
         v = p;

      while (expo >>= 1)
      {
         p *= p;

         if (p > 0xffffffff)
         {
            double fp = (double) p;

            if (expo & 1)
               val = fp * v;
            else
               val = (double) v;

            while(expo >>= 1)
            {
               fp *= fp;

               if (expo & 1)
                  val *= fp;
            }
            return (val);
         }

         if (expo & 1)
            v *= p;
      }

      val = (double) v;
   }

   return (val);
} /* double powi (uint8_t base, int32_t expo) */


/* ------------------------------------------------------------------------- *\
   str2ld_r reads a long double from a string and cares about a specified base.
\* ------------------------------------------------------------------------- */

long double str2ld_r(const char * psrc, char ** pend, int base, int * perr)
{
#if S2N_USE_POW10_ARRAY
   static const long double pow10a[] = { 1e-99L, 1e-98L, 1e-97L, 1e-96L, 1e-95L, 1e-94L, 1e-93L, 1e-92L, 1e-91L, 1e-90L,
                                         1e-89L, 1e-88L, 1e-87L, 1e-86L, 1e-85L, 1e-84L, 1e-83L, 1e-82L, 1e-81L, 1e-80L,
                                         1e-79L, 1e-78L, 1e-77L, 1e-76L, 1e-75L, 1e-74L, 1e-73L, 1e-72L, 1e-71L, 1e-70L,
                                         1e-69L, 1e-68L, 1e-67L, 1e-66L, 1e-65L, 1e-64L, 1e-63L, 1e-62L, 1e-61L, 1e-60L,
                                         1e-59L, 1e-58L, 1e-57L, 1e-56L, 1e-55L, 1e-54L, 1e-53L, 1e-52L, 1e-51L, 1e-50L,
                                         1e-49L, 1e-48L, 1e-47L, 1e-46L, 1e-45L, 1e-44L, 1e-43L, 1e-42L, 1e-41L, 1e-40L,
                                         1e-39L, 1e-38L, 1e-37L, 1e-36L, 1e-35L, 1e-34L, 1e-33L, 1e-32L, 1e-31L, 1e-30L,
                                         1e-29L, 1e-28L, 1e-27L, 1e-26L, 1e-25L, 1e-24L, 1e-23L, 1e-22L, 1e-21L, 1e-20L,
                                         1e-19L, 1e-18L, 1e-17L, 1e-16L, 1e-15L, 1e-14L, 1e-13L, 1e-12L, 1e-11L, 1e-10L,
                                         1e-09L, 1e-08L, 1e-07L, 1e-06L, 1e-05L, 1e-04L, 1e-03L, 1e-02L, 1e-01L,
                                          1e00L,  1e01L,  1e02L,  1e03L,  1e04L,  1e05L,  1e06L,  1e07L,  1e08L,  1e09L,
                                          1e10L,  1e11L,  1e12L,  1e13L,  1e14L,  1e15L,  1e16L,  1e17L,  1e18L,  1e19L,
                                          1e20L,  1e21L,  1e22L,  1e23L,  1e24L,  1e25L,  1e26L,  1e27L,  1e28L,  1e29L,
                                          1e30L,  1e31L,  1e32L,  1e33L,  1e34L,  1e35L,  1e36L,  1e37L,  1e38L,  1e39L,
                                          1e40L,  1e41L,  1e42L,  1e43L,  1e44L,  1e45L,  1e46L,  1e47L,  1e48L,  1e49L,
                                          1e50L,  1e51L,  1e52L,  1e53L,  1e54L,  1e55L,  1e56L,  1e57L,  1e58L,  1e59L,
                                          1e60L,  1e61L,  1e62L,  1e63L,  1e64L,  1e65L,  1e66L,  1e67L,  1e68L,  1e69L,
                                          1e70L,  1e71L,  1e72L,  1e73L,  1e74L,  1e75L,  1e76L,  1e77L,  1e78L,  1e79L,
                                          1e80L,  1e81L,  1e82L,  1e83L,  1e84L,  1e85L,  1e86L,  1e87L,  1e88L,  1e89L,
                                          1e90L,  1e91L,  1e92L,  1e93L,  1e94L,  1e95L,  1e96L,  1e97L,  1e98L,  1e99L };

   static const long double * pow10 = pow10a + 99;
#endif

   long double  dret  = 0.0;
   int          err   = EINVAL;
   const char * ps    = psrc;
   uint64_t     m0    = 0;   /* lower bits of mantissa */
   uint64_t     m1    = 0;   /* higher bits of mantissa */
   int32_t      sign  = 0;   /* sign of the value */
   int32_t      e     = 0;   /* value of exponent */
   int32_t      c     = 0;   /* correction of comma position */
   uint8_t      d     = 0;   /* last found digit */

   if(!ps || (base < 0) || (base > 36))
      goto Exit;

   /* skip leading blanks */
   while((*ps == ' ') || ((*ps >= 0x9) && (*ps <= 0xd)))
      ++ps;

   if(*ps == '-')
   {
      sign = -1;
      ++ps;
   }
   else if(*ps == '+')
   {
      sign = 1;
      ++ps;
   }

   if(((*ps | 0x20) == 'i') && ((*(ps+1)  | 0x20) == 'n') && ((*(ps+2) | 0x20) == 'f'))
   { /* "INF" found */
      if (((*(ps+3) | 0x20) == 'i') && ((*(ps+4)  | 0x20) == 'n') && ((*(ps+5) | 0x20) == 'i') && ((*(ps+6) | 0x20) == 't') && ((*(ps+7) | 0x20) == 'y') &&
          ((base <= 34) || (((digit_value[(uint8_t) *(ps+8)] >= base) && (*(ps+8) != '.') && (*(ps+8) != '~')))))
      {
         ps += 8;

         if(sign >= 0)
            dret = *(float*) pvinf;
         else
            dret = *(float*) pvninf;

         err = 0;
         goto Exit;
      }

      if ((base <= 23) || ((digit_value[(uint8_t) *(ps+3)] >= base) && (*(ps+3) != '.') && (*(ps+3) != '~')))
      {
         ps += 3;

         if(sign >= 0)
            dret = *(float*) pvinf;
         else
            dret = *(float*) pvninf;

         err = 0;
         goto Exit;
      }
   }
   else if(((*ps | 0x20) == 'n') && ((*(ps+1)  | 0x20) == 'a') && ((*(ps+2) | 0x20) == 'n'))
   { /* "NAN" found */
      if ((base <= 23) || ((digit_value[(uint8_t) *(ps+3)] >= base) && (*(ps+3) != '.') && (*(ps+3) != '~')))
      {
         ps += 3;

         if(sign >= 0)
            dret = *(float*) pvnan;
         else
            dret = *(float*) pvnnan;

         err = 0;
         goto Exit;
      }
   }

   if(base < 2)
   { /* let's detect the base */
      base = 10; /* default base is 10 */

      if(*ps == '0')
      {
         if(((*(ps+1) == 'x') || (*(ps+1) == 'X')) && (digit_value[(uint8_t) *(ps+2)] < 16))
         {
            base = 16;
            ps += 2;
         }
         else if(((*(ps+1) == 'b') || (*(ps+1) == 'B')) && (digit_value[(uint8_t) *(ps+2)] < 2))
         {
            base = 2;
            ps += 2;
         }
         else if(((*(ps+1) == 'o') || (*(ps+1) == 'O')) && (digit_value[(uint8_t) *(ps+2)] < 8))
         {
            base = 8;
            ps += 2;
         }
      }
   }/* if(base < 2) */
   else if (*ps == '0')
   {
      /* Care about base specifications in hex data even if base is given.
         (It's a rather dirty thing within the specification of strtoul.) */
      if ((base == 16) && ((ps[1] == 'x') || (ps[1] == 'X')) && (digit_value[(uint8_t) ps[2]] < 16))
         ps += 2;
      else if ((base == 2) && ((ps[1] == 'b') || (ps[1] == 'B')) && (digit_value[(uint8_t) ps[2]] < 2))
         ps += 2;
      else if ((base == 8) && ((ps[1] == 'o') || (ps[1] == 'O')) && (digit_value[(uint8_t) ps[2]] < 8))
         ps += 2;
   }

   if(digit_value[(uint8_t) *ps] >= base)
   { /* not a valid number */
      ps = psrc;
      goto Exit;
   }

   while (*ps == '0')
     ++ps; /* skip leading zeros */

   d = digit_value[(uint8_t) *ps];
   while(d < base)
   {
      if (m1 < 0x400000000000000ll)
      {
         m0 *= base;
         m1 *= base;
         m0 += d;
         m1 += m0 >> 58;
         m0 &= 0x3ffffffffffffffll;
      }
      else
      {
         ++c;
      }

      d = digit_value[(uint8_t) *(++ps)];
   }

   if(*ps == '.')
   {
      d = digit_value[(uint8_t) *(++ps)];
      while(d < base)
      {
         if (m1 < 0x400000000000000ll)
         {
            m0 *= base;
            m1 *= base;
            m0 += d;
            m1 += m0 >> 58;
            m0 &= 0x3ffffffffffffffll;
            --c;
         }
         d = digit_value[(uint8_t) *(++ps)];
      }
   }

   if (((base < 15) && ((*ps | 0x20) == 'e')) || (*ps == '~'))
   {
      int32_t   exp_sign = 0;   /* whether the exponent is signed */

      d = (uint8_t) *(ps+1);
      if(digit_value[d] < base)
      {
         e   = digit_value[d];
         ps += 2;
      }
      else if (d == '+')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < base)
         {
            e   = d;
            ps += 3;
         }
      }
      else if (d == '-')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < base)
         {
            exp_sign = 1;
            e   = d;
            ps += 3;
         }
      }

      d = digit_value[(uint8_t) *ps];
      while(d < base)
      {
         e *= base;
         e += d;
         d  = digit_value[(uint8_t) *(++ps)];
      }

      if(exp_sign)
         e = -e;
   }
   else if ((base == 16) && ((*ps | 0x20) == 'p'))
   {  /* Seems to be a printf %a of %A output string that uses a decimal printed binary exponent ... */
      int32_t   exp_sign = 0;   /* whether the exponent is signed */

      d = (uint8_t) *(ps+1);
      if(digit_value[d] < 10)
      {
         e = digit_value[d];
         ps += 2;
      }
      else if (d == '+')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < 10)
         {
            e   = d;
            ps += 3;
         }
      }
      else if (d == '-')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < 10)
         {
            exp_sign = 1;
            e   = d;
            ps += 3;
         }
      }

      d = digit_value[(uint8_t) *ps];
      while(d < 10)
      {
         e *= 10;
         e += d;
         d  = digit_value[(uint8_t) *(++ps)];
      }

      if(exp_sign)
         e = -e;

      base = 2;
      c   *= 4;
   }

   e += c;

#if S2N_USE_POW10_ARRAY
   if ((base == 10) && (e < 100) && (e > -100))
   {
      dret = ((long double) m1 * 0x400000000000000ll + m0) * pow10[e];
   }
   else
#endif
   if(e >= 0)
   {
      dret = ((long double) m1 * 0x400000000000000ll + m0) * powil(base, e);
   }
   else if(e >= -192)
   {
      dret = ((long double) m1 * 0x400000000000000ll + m0) / powil(base, -e);
   }
   else
   {  /* Try to prevent a possible overflow within powil */
      long double p;
      c  = e / 2;
      e -= c;
      p  = powil(base, -c);

      dret = ((long double) m1 * 0x400000000000000ll + m0) / p;

      if(e != c)
         p *= base;

      dret /= p;
   }

   if(sign < 0)
      dret = -dret;

   err = 0;

Exit:;

   if(pend)
      *pend = (char *) ps; /* store pointer to end position */

   if(perr)
      *perr = err;

   return (dret);
} /* long double str2ld_r(const char * ps, char ** pe, int base, int * perr) */


/* ------------------------------------------------------------------------- *\
   str2ld is a wrapper for strtold for reading long doubles.
\* ------------------------------------------------------------------------- */

long double str2ld(const char * psrc, char ** pend)
{
   int err = 0;
   long double dret = str2ld_r(psrc, pend, 0, &err);
   if(err)
      errno = err;
   return (dret);
} /* long double str2ld(const char * psrc, char ** pend) */


/* ------------------------------------------------------------------------- *\
   str2d_r reads a double from a string and cares about a specified base.
\* ------------------------------------------------------------------------- */

double str2d_r(const char * psrc, char ** pend, int base, int * perr)
{
#if S2N_USE_POW10_ARRAY
   static const double pow10a[] = { 1e-99, 1e-98, 1e-97, 1e-96, 1e-95, 1e-94, 1e-93, 1e-92, 1e-91, 1e-90,
                                    1e-89, 1e-88, 1e-87, 1e-86, 1e-85, 1e-84, 1e-83, 1e-82, 1e-81, 1e-80,
                                    1e-79, 1e-78, 1e-77, 1e-76, 1e-75, 1e-74, 1e-73, 1e-72, 1e-71, 1e-70,
                                    1e-69, 1e-68, 1e-67, 1e-66, 1e-65, 1e-64, 1e-63, 1e-62, 1e-61, 1e-60,
                                    1e-59, 1e-58, 1e-57, 1e-56, 1e-55, 1e-54, 1e-53, 1e-52, 1e-51, 1e-50,
                                    1e-49, 1e-48, 1e-47, 1e-46, 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40,
                                    1e-39, 1e-38, 1e-37, 1e-36, 1e-35, 1e-34, 1e-33, 1e-32, 1e-31, 1e-30,
                                    1e-29, 1e-28, 1e-27, 1e-26, 1e-25, 1e-24, 1e-23, 1e-22, 1e-21, 1e-20,
                                    1e-19, 1e-18, 1e-17, 1e-16, 1e-15, 1e-14, 1e-13, 1e-12, 1e-11, 1e-10,
                                    1e-09, 1e-08, 1e-07, 1e-06, 1e-05, 1e-04, 1e-03, 1e-02, 1e-01,
                                     1e00,  1e01,  1e02,  1e03,  1e04,  1e05,  1e06,  1e07,  1e08,  1e09,
                                     1e10,  1e11,  1e12,  1e13,  1e14,  1e15,  1e16,  1e17,  1e18,  1e19,
                                     1e20,  1e21,  1e22,  1e23,  1e24,  1e25,  1e26,  1e27,  1e28,  1e29,
                                     1e30,  1e31,  1e32,  1e33,  1e34,  1e35,  1e36,  1e37,  1e38,  1e39,
                                     1e40,  1e41,  1e42,  1e43,  1e44,  1e45,  1e46,  1e47,  1e48,  1e49,
                                     1e50,  1e51,  1e52,  1e53,  1e54,  1e55,  1e56,  1e57,  1e58,  1e59,
                                     1e60,  1e61,  1e62,  1e63,  1e64,  1e65,  1e66,  1e67,  1e68,  1e69,
                                     1e70,  1e71,  1e72,  1e73,  1e74,  1e75,  1e76,  1e77,  1e78,  1e79,
                                     1e80,  1e81,  1e82,  1e83,  1e84,  1e85,  1e86,  1e87,  1e88,  1e89,
                                     1e90,  1e91,  1e92,  1e93,  1e94,  1e95,  1e96,  1e97,  1e98,  1e99 };

   static const double * pow10 = pow10a + 99;
#endif

   double       dret  = 0.0;
   int          err   = EINVAL;
   const char * ps    = psrc;
   uint64_t     m     = 0;   /* mantissa */
   int32_t      sign  = 0;   /* sign of value */
   int32_t      e     = 0;   /* value of exponent */
   int32_t      c     = 0;   /* correction of comma position */
   uint8_t      d     = 0;   /* last found digit */

   if(!ps || (base < 0) || (base > 36))
      goto Exit;

   /* skip leading blanks */
   while((*ps == ' ') || ((*ps >= 0x9) && (*ps <= 0xd)))
      ++ps;

   if(*ps == '-')
   {
      sign = -1;
      ++ps;
   }
   else if(*ps == '+')
   {
      sign = 1;
      ++ps;
   }

   if(((*ps | 0x20) == 'i') && ((*(ps+1)  | 0x20) == 'n') && ((*(ps+2) | 0x20) == 'f'))
   { /* "INF" found */
      if (((*(ps+3) | 0x20) == 'i') && ((*(ps+4)  | 0x20) == 'n') && ((*(ps+5) | 0x20) == 'i') && ((*(ps+6) | 0x20) == 't') && ((*(ps+7) | 0x20) == 'y') &&
          ((base <= 34) || (((digit_value[(uint8_t) *(ps+8)] >= base) && (*(ps+8) != '.') && (*(ps+8) != '~')))))
      {
         ps += 8;

         if(sign >= 0)
            dret = *(float*) pvinf;
         else
            dret = *(float*) pvninf;

         err = 0;
         goto Exit;
      }

      if ((base <= 23) || ((digit_value[(uint8_t) *(ps+3)] >= base) && (*(ps+3) != '.') && (*(ps+3) != '~')))
      {
         ps += 3;

         if(sign >= 0)
            dret = *(float*) pvinf;
         else
            dret = *(float*) pvninf;

         err = 0;
         goto Exit;
      }
   }
   else if(((*ps | 0x20) == 'n') && ((*(ps+1)  | 0x20) == 'a') && ((*(ps+2) | 0x20) == 'n'))
   { /* "NAN" found */
      if ((base <= 23) || ((digit_value[(uint8_t) *(ps+3)] >= base) && (*(ps+3) != '.') && (*(ps+3) != '~')))
      {
         ps += 3;

         if(sign >= 0)
            dret = *(float*) pvnan;
         else
            dret = *(float*) pvnnan;

         err = 0;
         goto Exit;
      }
   }

   if(base < 2)
   { /* let's detect the base */
      base = 10; /* default base is 10 */

      if(*ps == '0')
      {
         if(((*(ps+1) == 'x') || (*(ps+1) == 'X')) && (digit_value[(uint8_t) *(ps+2)] < 16))
         {
            base = 16;
            ps += 2;
         }
         else if(((*(ps+1) == 'b') || (*(ps+1) == 'B')) && (digit_value[(uint8_t) *(ps+2)] < 2))
         {
            base = 2;
            ps += 2;
         }
         else if(((*(ps+1) == 'o') || (*(ps+1) == 'O')) && (digit_value[(uint8_t) *(ps+2)] < 8))
         {
            base = 8;
            ps += 2;
         }
      }
   }/* if(base < 2) */
   else if(*ps == '0')
   {
      /* Care about base specifications in hex data even if base is given.
         (It's a rather dirty thing within the specification of strtoul.) */
      if((base == 16) && ((ps[1] == 'x') || (ps[1] == 'X')) && (digit_value[(uint8_t) ps[2]] < 16))
         ps += 2;
      else if((base == 2) && ((ps[1] == 'b') || (ps[1] == 'B')) && (digit_value[(uint8_t) ps[2]] < 2))
         ps += 2;
      else if((base == 8) && ((ps[1] == 'o') || (ps[1] == 'O')) && (digit_value[(uint8_t) ps[2]] < 8))
         ps += 2;
   }

   if(digit_value[(uint8_t) *ps] >= base)
   { /* not a valid number */
      ps = psrc;
      goto Exit;
   }

   while (*ps == '0')
     ++ps; /* skip leading zeros */

   d = digit_value[(uint8_t) *ps];
   while(d < base)
   {
      if (m < 0x400000000000000ll)
      {
         m *= base;
         m += d;
      }
      else
      {
         ++c;
      }

      d = digit_value[(uint8_t) *(++ps)];
   }

   if(*ps == '.')
   {
      d = digit_value[(uint8_t) *(++ps)];
      while(d < base)
      {
         if (m < 0x400000000000000ll)
         {
            m *= base;
            m += d;
            --c;
         }
         d = digit_value[(uint8_t) *(++ps)];
      }
   }

   if (((base < 15) && ((*ps | 0x20) == 'e')) || (*ps == '~'))
   {
      int32_t exp_sign = 0; /* sign of exponent */

      d = (uint8_t) *(ps+1);
      if(digit_value[d] < base)
      {
         e   = digit_value[d];
         ps += 2;
      }
      else if (d == '+')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < base)
         {
            e   = d;
            ps += 3;
         }
      }
      else if (d == '-')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < base)
         {
            exp_sign = 1;
            e   = d;
            ps += 3;
         }
      }

      d = digit_value[(uint8_t) *ps];
      while(d < base)
      {
         e *= base;
         e += d;
         d  = digit_value[(uint8_t) *(++ps)];
      }

      if(exp_sign)
         e = -e;
   }
   else if ((base == 16) && ((*ps | 0x20) == 'p'))
   {  /* Seems to be a printf %a of %A output string that uses a decimal printed binary exponent ... */
      int32_t exp_sign = 0;

      d = (uint8_t) *(ps+1);
      if(digit_value[d] < 10)
      {
         e = digit_value[d];
         ps += 2;
      }
      else if (d == '+')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < 10)
         {
            e   = d;
            ps += 3;
         }
      }
      else if (d == '-')
      {
         d = digit_value[(uint8_t) *(ps+2)];
         if(d < 10)
         {
            exp_sign = 1;
            e   = d;
            ps += 3;
         }
      }

      d = digit_value[(uint8_t) *ps];
      while(d < 10)
      {
         e *= 10;
         e += d;
         d = digit_value[(uint8_t) *(++ps)];
      }

      if(exp_sign)
         e = -e;

      base = 2;
      c   *= 4;
   }

   e += c;

#if S2N_USE_POW10_ARRAY
   if ((base == 10) && (e < 100) && (e > -100))
   {
      dret = (double) m * pow10[e];
   }
   else
#endif
   if(e >= 0)
   {
      dret = (double) m * powi(base, e);
   }
   else
   {  /* Try to prevent a possible overflow within powi */
      double p;
      c  = e / 2;
      e -= c;
      p  = powi(base, -c);

      dret = (double) m / p;

      if(e != c)
         p *= base;

      dret /= p;
   }

   if(sign < 0)
      dret = -dret;

   err = 0;

Exit:;

   if(pend)
      *pend = (char *) ps; /* store pointer to end position */

   if(perr)
      *perr = err;

   return (dret);
} /* double str2d_r(const char * ps, char ** pe, int base, int * perr) */


/* ------------------------------------------------------------------------- *\
   str2d is a wrapper for strtod for reading doubles.
\* ------------------------------------------------------------------------- */

double str2d(const char * psrc, char ** pend)
{
   int err = 0;
   double dret = str2d_r(psrc, pend, 0, &err);
   if(err)
      errno = err;
   return (dret);
} /* double str2d(const char * psrc, char ** pend) */


/* ------------------------------------------------------------------------- *\
   str2f is a wrapper for strtof for reading floats.
\* ------------------------------------------------------------------------- */

float str2f(const char * psrc, char ** pend)
{
   int err = 0;
   float dret = (float) str2d_r(psrc, pend, 0, &err);
   if(err)
      errno = err;
   return (dret);
} /* double str2d(const char * psrc, char ** pend) */


/* ========================================================================= *\
   Integer conversion functions
\* ========================================================================= */

/*
   SFE   signed function name that uses errno for errors
   UFE   unsigned function name that uses errno for errors
   SFN   signed function name
   UFN   unsigned function name
   UT    unsigned type
   ST    signed type
   U_MAX maximum of unsigned type
   S_MAX maximum of signed type
   S_MIN minimum of signed type
*/


#define SFE str2i64
#define UFE str2u64
#define SFN str2i64_r
#define UFN str2u64_r
#define UT  uint64_t
#define ST  int64_t
#define U_MAX UINT64_MAX
#define S_MAX INT64_MAX
#define S_MIN INT64_MIN

#include "str2num.c"

#define SFE str2i32
#define UFE str2u32
#define SFN str2i32_r
#define UFN str2u32_r
#define UT  uint32_t
#define ST  int32_t
#define U_MAX UINT32_MAX
#define S_MAX INT32_MAX
#define S_MIN INT32_MIN

#include "str2num.c"

#define SFE str2i16
#define UFE str2u16
#define SFN str2i16_r
#define UFN str2u16_r
#define UT  uint16_t
#define ST  int16_t
#define U_MAX UINT16_MAX
#define S_MAX INT16_MAX
#define S_MIN INT16_MIN

#include "str2num.c"

#define SFE str2i8
#define UFE str2u8
#define SFN str2i8_r
#define UFN str2u8_r
#define UT  uint8_t
#define ST  int8_t
#define U_MAX UINT8_MAX
#define S_MAX INT8_MAX
#define S_MIN INT8_MIN

#include "str2num.c"

#define SFE str2pd
#define UFE str2sz
#define SFN str2pd_r
#define UFN str2sz_r
#define UT  size_t
#define ST  ptrdiff_t
#define U_MAX (~(size_t)0)
#define S_MIN ((ptrdiff_t) 1 << (sizeof(ptrdiff_t) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"

#define SFE str2ip
#define UFE str2up
#define SFN str2ip_r
#define UFN str2up_r
#define UT  uintptr_t
#define ST  intptr_t
#define U_MAX (~(uintptr_t)0)
#define S_MIN ((intptr_t) 1 << (sizeof(intptr_t) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"

#define SFE str2l
#define UFE str2ul
#define SFN str2l_r
#define UFN str2ul_r
#define UT  unsigned long
#define ST  long
#define U_MAX (~(unsigned long)0)
#define S_MIN ((long) 1 << (sizeof(long) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"

#define SFE str2ll
#define UFE str2ull
#define SFN str2ll_r
#define UFN str2ull_r
#define UT  unsigned long long
#define ST  long long
#define U_MAX (~(unsigned long long)0)
#define S_MIN ((long long) 1 << (sizeof(long long) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"


#else /* STR2NUM_C */

/* ========================================================================= *\
   Implementation of integer conversion functions
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   UFN converts a string to an unsigned integer type
\* ------------------------------------------------------------------------- */

UT UFN (const char * ps, char ** pe, int base, int * perr)
{
   UT  u_ret = 0;
   int err   = 0;
   static const UT max_base[] = {0, 0, U_MAX/2,  U_MAX/3,  U_MAX/4,  U_MAX/5,  U_MAX/6,  U_MAX/7,  U_MAX/8,  U_MAX/9,  U_MAX/10, U_MAX/11, U_MAX/12, U_MAX/13,
                                       U_MAX/14, U_MAX/15, U_MAX/16, U_MAX/17, U_MAX/18, U_MAX/19, U_MAX/20, U_MAX/21, U_MAX/22, U_MAX/23, U_MAX/24, U_MAX/25,
                                       U_MAX/26, U_MAX/27, U_MAX/28, U_MAX/29, U_MAX/30, U_MAX/31, U_MAX/32, U_MAX/33, U_MAX/34, U_MAX/35, U_MAX/36};
   const char * psrc = ps;
   UT max;
   UT d;
   int sign = 0;

   if(!ps)
   {
      err = EINVAL;
      goto Exit;
   }

   /* skip leading blanks */
   while((*ps == ' ') || ((*ps >= 0x9) && (*ps <= 0xd)))
      ++ps;

   if(*ps == '+')
   {
      sign = 1;
      ++ps;
   }
   else if(*ps == '-')
   {
      sign = -1;
      ++ps;
   }

   if(base < 2)
   { /* let's detect the base */
      if (base < 0)
      {
         ps  = psrc;
         err = EINVAL;
         goto Exit;
      }

      if((*ps > '0') && (*ps <= '9'))
      {
         base = 10;
      }
      else if(*ps == '0')
      {
         ++ps;
         if(((*ps == 'x') || (*ps == 'X')) && (digit_value[(uint8_t) ps[1]] < 16))
         {
            base = 16;
            ++ps;
         }
         else if(((*ps == 'b') || (*ps == 'B')) && (digit_value[(uint8_t) ps[1]] < 2))
         {
            base = 2;
            ++ps;
         }
         else if(((*ps == 'o') || (*ps == 'O')) && (digit_value[(uint8_t) ps[1]] < 2))
         {
            base = 8;
            ++ps;
         }
         else if(!base)
         { /* if base is 0 than the default base after a leading 0 is 8 */
            base = 8;
            if (digit_value[(uint8_t) *ps] >= 8)
               goto Exit;
         }
         else
         { /* if base is 1 than the default base after a leading 0 is 10 */
            base = 10;
            if (digit_value[(uint8_t) *ps] >= 10)
               goto Exit;
         }
      }
      else
      {
         ps  = psrc;
         err = EINVAL;
         goto Exit;
      }
   }
   else if(base > 36)
   {
      ps  = psrc;
      err = EINVAL;
      goto Exit;
   }
   else if(*ps == '0')
   {
      /* Care about base specifications in hex data even if base is given.
         (It's a rather dirty thing within the specification of strtoul.) */
      if((base == 16)  && ((ps[1] == 'x') || (ps[1] == 'X')) && (digit_value[(uint8_t) ps[2]] < 16))
         ps += 2;
      else if((base == 2) && ((ps[1] == 'b') || (ps[1] == 'B')) && (digit_value[(uint8_t) ps[2]] < 2))
         ps += 2;
      else if((base == 8) && ((ps[1] == 'o') || (ps[1] == 'O')) && (digit_value[(uint8_t) ps[2]] < 8))
         ps += 2;
   }

   max = max_base[base];

   d = digit_value[(uint8_t) *ps];
   if(d >= (uint8_t) base)
   {
      ps  = psrc;
      err = EINVAL;
      goto Exit;
   }

   u_ret = d;
   d = digit_value[(uint8_t) *(++ps)];

   while(d < (uint8_t) base)
   {
      if(u_ret >= max)
      {
         if(((uint8_t) base > digit_value[(uint8_t) *(ps+1)]) || (u_ret > max) || ((u_ret * base) > (U_MAX - d)))
         {
            err   = ERANGE; /* indicate overflow error */
#if S2N_UMIN_IS_SMIN
            /* This does not match POSIX standard that allows return of U_MAX only. :o( */

            if (sign < 0)
            { /* negative value exceeds the minimum of a signed type of that width */
               u_ret = ((U_MAX >> 1) + 1);
            }
            else
#endif
            {
               u_ret = U_MAX;
            }
#if 1
            /* Move to end of valid digits even if those exceed the range of our type. */
            while ((uint8_t) base > digit_value[(uint8_t) *(++ps)])
            {}
#endif
            goto Exit;
         }
      }

      u_ret = u_ret * base + d;
      d = digit_value[(uint8_t) *(++ps)];
   }

   if (sign < 0)
   {
#if S2N_UMIN_IS_SMIN
      /* This does not match POSIX standard that demands a negation. :o( */

      if(u_ret > ((U_MAX >> 1) + 1))
      { /* The negative value exceeds the minimum of a signed type of that width. */
         err   = ERANGE; /* indicate overflow error */
         u_ret = ((U_MAX >> 1) + 1);
      }
      else
#endif
      {
         u_ret = ~u_ret + 1; /* negate the result */
      }
   }

Exit:;

   if(perr)
      *perr = err;

   if(pe)
      *pe = (char *) ps;

   return(u_ret);
}/* UT UFN(...) */



/* ------------------------------------------------------------------------- *\
   UFE converts a string to an unsigned integer type
\* ------------------------------------------------------------------------- */

UT UFE (const char * ps, char ** pe, int base)
{
   int err;
   UT  u_ret = UFN (ps, pe, base, &err);
   if(err)
      errno = err;
   return(u_ret);
} /* UT UFE (...) */



/* ------------------------------------------------------------------------- *\
   ST SFN converts a string to a signed integer type
\* ------------------------------------------------------------------------- */

ST SFN (const char * ps, char ** pe, int base, int * perr)
{
   ST  s_ret = 0;
   int err   = 0;
   static const ST max_base[] = {0, 0, S_MAX/2,  S_MAX/3,  S_MAX/4,  S_MAX/5,  S_MAX/6,  S_MAX/7,  S_MAX/8,  S_MAX/9,  S_MAX/10, S_MAX/11, S_MAX/12, S_MAX/13,
                                       S_MAX/14, S_MAX/15, S_MAX/16, S_MAX/17, S_MAX/18, S_MAX/19, S_MAX/20, S_MAX/21, S_MAX/22, S_MAX/23, S_MAX/24, S_MAX/25,
                                       S_MAX/26, S_MAX/27, S_MAX/28, S_MAX/29, S_MAX/30, S_MAX/31, S_MAX/32, S_MAX/33, S_MAX/34, S_MAX/35, S_MAX/36};

   static const ST min_base[] = {0, 0, S_MIN/2,  S_MIN/3,  S_MIN/4,  S_MIN/5,  S_MIN/6,  S_MIN/7,  S_MIN/8,  S_MIN/9,  S_MIN/10, S_MIN/11, S_MIN/12, S_MIN/13,
                                       S_MIN/14, S_MIN/15, S_MIN/16, S_MIN/17, S_MIN/18, S_MIN/19, S_MIN/20, S_MIN/21, S_MIN/22, S_MIN/23, S_MIN/24, S_MIN/25,
                                       S_MIN/26, S_MIN/27, S_MIN/28, S_MIN/29, S_MIN/30, S_MIN/31, S_MIN/32, S_MIN/33, S_MIN/34, S_MIN/35, S_MIN/36};
   const char * psrc = ps;
   ST max;
   ST d;
   int sign = 0;

   if(!ps)
   {
      err = EINVAL;
      goto Exit;
   }

   /* skip leading blanks */
   while((*ps == ' ') || ((*ps >= 0x9) && (*ps <= 0xd)))
      ++ps;

   if(*ps == '+')
   {
      sign = 1;
      ++ps;
   }
   else if(*ps == '-')
   {
      sign = -1;
      ++ps;
   }

   if(base < 2)
   { /* let's detect the base */
      if (base < 0)
      {
         ps  = psrc;
         err = EINVAL;
         goto Exit;
      }

      if((*ps > '0') && (*ps <= '9'))
      {
         base = 10;
      }
      else if(*ps == '0')
      {
         ++ps;
         if(((*ps == 'x') || (*ps == 'X')) && (digit_value[(uint8_t) ps[1]] < 16))
         {
            base = 16;
            ++ps;
         }
         else if(((*ps == 'b') || (*ps == 'B')) && (digit_value[(uint8_t) ps[1]] < 2))
         {
            base = 2;
            ++ps;
         }
         else if(((*ps == 'o') || (*ps == 'O')) && (digit_value[(uint8_t) ps[1]] < 2))
         {
            base = 8;
            ++ps;
         }
         else if(!base)
         { /* if base is 0 than the default base after a leading 0 is 8 */
            base = 8;
            if (digit_value[(uint8_t) *ps] >= 8)
               goto Exit;
         }
         else
         { /* if base is 1 than the default base after a leading 0 is 10 */
            base = 10;
            if (digit_value[(uint8_t) *ps] >= 10)
               goto Exit;
         }
      }
      else
      {
         ps  = psrc;
         err = EINVAL;
         goto Exit;
      }
   }
   else if(base > 36)
   {
      ps  = psrc;
      err = EINVAL;
      goto Exit;
   }
   else if(*ps == '0')
   {
      /* Care about base specifications in hex data even if base is given.
         (It's a rather dirty thing within the specification of strtoul.) */
      if((base == 16)  && ((ps[1] == 'x') || (ps[1] == 'X')) && (digit_value[(uint8_t) ps[2]] < 16))
         ps += 2;
      else if((base == 2) && ((ps[1] == 'b') || (ps[1] == 'B')) && (digit_value[(uint8_t) ps[2]] < 2))
         ps += 2;
      else if((base == 8) && ((ps[1] == 'o') || (ps[1] == 'O')) && (digit_value[(uint8_t) ps[2]] < 8))
         ps += 2;
   }

   max = sign < 0 ? -min_base[base] : max_base[base];

   d = digit_value[(uint8_t) *ps];

   if(d >= (uint8_t) base)
   {
      ps  = psrc;
      err = EINVAL;
      goto Exit;
   }

   s_ret = d;
   d = digit_value[(uint8_t)*(++ps)];

   while(d < (uint8_t) base)
   {
      if(s_ret >= max)
      {
         if(((uint8_t) base <= digit_value[(uint8_t)*(ps+1)]) && (s_ret == max))
         { /* last possibly valid digit */
            ++ps;

            if(sign < 0)
            { /* negative value */
               s_ret *= -base;
               if(s_ret < (S_MIN + d))
               {
                  err = ERANGE; /* indicate overflow error */
                  s_ret = S_MIN;
               }
               else
                  s_ret -= d;
            }
            else
            {
               s_ret *= base;
               if(s_ret > (S_MAX - d))
               {
                  err = ERANGE; /* indicate overflow error */
                  s_ret = S_MAX;
               }
               else
                  s_ret += d;
            }
            goto Exit;
         }

         /* do a range check to handle overflows */
         err = ERANGE; /* indicate overflow error */

         if(sign < 0)
            s_ret = S_MIN;
         else
            s_ret = S_MAX;
#if 1
         /* Move to end of valid digits even if those exceed the range of our type */
         while ((uint8_t) base > digit_value[(uint8_t) *(++ps)])
         {}
#endif
         goto Exit;
      }

      s_ret = s_ret * base + d;
      d = digit_value[(uint8_t)*(++ps)];
   }

   if (sign < 0)
      s_ret = -s_ret;

   Exit:;

   if(perr)
      *perr = err;

   if(pe)
      *pe = (char *) ps;

   return(s_ret);
}/* ST SFN (...)*/


/* ------------------------------------------------------------------------- *\
   SFE converts a string to an unsigned integer type
\* ------------------------------------------------------------------------- */

ST SFE (const char * ps, char ** pe, int base)
{
   int err;
   ST  s_ret = SFN (ps, pe, base, &err);
   if(err)
      errno = err;
   return(s_ret);
}/* ST SFE(...) */


#undef SFE
#undef UFE
#undef SFN
#undef UFN
#undef UT
#undef ST
#undef U_MAX
#undef S_MAX
#undef S_MIN

#endif /* STR2NUM_C */

/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */

