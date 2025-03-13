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
*  COPYRIGHT :   (c) 2025 Dipl.-Ing. Klaus Lux (Aachen, Germany)              *
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
                               /*      !  "  #  $   %  &  '   (   )  *  +  ,  -  .  /     0  1  2  3   4  5  6  7   8  9  :  ;   <  =  >  ?  */
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,    0, 1, 2, 3,  4, 5, 6, 7,  8, 9,64,64, 64,64,64,64,
                               /*   @  A  B  C   D  E  F  G   H  I  J  K   L  M  N  O     P  Q  R  S   T  U  V  W   X  Y  Z  [   \  ]  ^  _  */
                                   64,10,11,12, 13,14,15,16, 17,18,19,20, 21,22,23,24,   25,26,27,28, 29,30,31,32, 33,34,35,64, 64,64,64,64,
                               /*   `  a  b  c   d  e  f  g   h  i  j  k   l  m  n  o     p  q  r  s   t  u  v  w   x  y  z  {   |  }  ~     */
                                   64,10,11,12, 13,14,15,16, 17,18,19,20, 21,22,23,24,   25,26,27,28, 29,30,31,32, 33,34,35,64, 64,64,64,64,

                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,
                                   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64,   64,64,64,64, 64,64,64,64, 64,64,64,64, 64,64,64,64 };

/*
   SFN   signed function
   UFN   unsigned function
   UT    unsigned type
   ST    signed type
   U_MAX maximum of unsigned type
   S_MAX maximum of signed type
   S_MIN minimum of signed type
*/

#define SFN(Fn) Fn##i64
#define UFN(Fn) Fn##u64
#define UT uint64_t
#define ST int64_t
#define U_MAX UINT64_MAX
#define S_MAX INT64_MAX
#define S_MIN INT64_MIN

#include "str2num.c"

#define SFN(Fn) Fn##i32
#define UFN(Fn) Fn##u32
#define UT uint32_t
#define ST int32_t
#define U_MAX UINT32_MAX
#define S_MAX INT32_MAX
#define S_MIN INT32_MIN

#include "str2num.c"

#define SFN(Fn) Fn##i16
#define UFN(Fn) Fn##u16
#define UT uint16_t
#define ST int16_t
#define U_MAX UINT16_MAX
#define S_MAX INT16_MAX
#define S_MIN INT16_MIN

#include "str2num.c"

#define SFN(Fn) Fn##i8
#define UFN(Fn) Fn##u8
#define UT uint8_t
#define ST int8_t
#define U_MAX UINT8_MAX
#define S_MAX INT8_MAX
#define S_MIN INT8_MIN

#include "str2num.c"

#define SFN(Fn) Fn##sz
#define UFN(Fn) Fn##uz
#define UT size_t
#define ST ptrdiff_t
#define U_MAX (~(size_t)0)
#define S_MIN ((ptrdiff_t) 1 << (sizeof(ptrdiff_t) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"

#define SFN(Fn) Fn##l
#define UFN(Fn) Fn##ul
#define UT unsigned long
#define ST long
#define U_MAX (~(unsigned long)0)
#define S_MIN ((long) 1 << (sizeof(long) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"

#define SFN(Fn) Fn##ll
#define UFN(Fn) Fn##ull
#define UT unsigned long long
#define ST long long
#define U_MAX (~(unsigned long long)0)
#define S_MIN ((long long) 1 << (sizeof(long long) * 8 - 1))
#define S_MAX (~S_MIN)

#include "str2num.c"


#else /* STR2NUM_C */

/* ========================================================================= *\
   Implementation of conversion functions
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   UFN(r_str2) converts a string to an unsigned integer type
\* ------------------------------------------------------------------------- */

UT UFN(r_str2) (const char * ps, char ** pe, int base, int * perr)
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

   if(!base)
   { /* let's detect the base */
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
         else
         {
            base = 8;
            if (digit_value[(uint8_t) *ps] >= 8)
               goto Exit;
         }
      }
   }/* if(!base) */
   else
   {  /* Care about base specifications in hex data even if base is given.
         (It's a rather dirty thing within the specification of strtoul.) */
      if((base == 16) && (*ps == '0') && ((ps[1] == 'x') || (ps[1] == 'X')) && (digit_value[(uint8_t) ps[2]] < 16))
         ps += 2;
      else if((base == 2) && (*ps == '0') && ((ps[1] == 'b') || (ps[1] == 'B')) && (digit_value[(uint8_t) ps[2]] < 2))
         ps += 2;
   }

   if((base < 2) || (base > 36))
   {
      ps  = psrc;
      err = EINVAL;
      goto Exit;
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
            u_ret = U_MAX;
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
#if 0
      /* Disabled because the POSIX standard demands a negation only. :o( */

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
}/* UT UFN(r_str2) */



/* ------------------------------------------------------------------------- *\
   UFN(str2) converts a string to an unsigned integer type
\* ------------------------------------------------------------------------- */
UT UFN(str2) (const char * ps, char ** pe, int base)
{
   int err;
   UT  u_ret = UFN(r_str2) (ps, pe, base, &err);
   if(err)
      errno = err;
   return(u_ret);
} /* UT UFN(str2) */



/* ------------------------------------------------------------------------- *\
   ST SFN(str2) converts a string to a signed integer type
\* ------------------------------------------------------------------------- */
ST SFN(r_str2)(const char * ps, char ** pe, int base, int * perr)
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

   if(!base)
   { /* let's detect the base */
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
         else
         {
            base = 8;
            if (digit_value[(uint8_t) *ps] >= 8)
               goto Exit;
         }
      }
   }/* if(!base) */
   else
   {  /* Care about base specifications in hex data even if base is given.
         (It's a rather dirty thing within the specification of strtol.) */
      if((base == 16) && (*ps == '0') && ((ps[1] == 'x') || (ps[1] == 'X')) && (digit_value[(uint8_t) ps[2]] < 16))
         ps += 2;
      else if((base == 2) && (*ps == '0') && ((ps[1] == 'b') || (ps[1] == 'B')) && (digit_value[(uint8_t) ps[2]] < 2))
         ps += 2;
   }

   if((base < 2) || (base > 36))
   {
      ps  = psrc;
      err = EINVAL;
      goto Exit;
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
}/* ST SFN(r_str2) */


/* ------------------------------------------------------------------------- *\
   SFN(str2) converts a string to an unsigned integer type
\* ------------------------------------------------------------------------- */
ST SFN(str2) (const char * ps, char ** pe, int base)
{
   int err;
   ST  s_ret = SFN(r_str2) (ps, pe, base, &err);
   if(err)
      errno = err;
   return(s_ret);
}/* ST SFN(str2) */



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
