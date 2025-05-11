#if 0
rm -f ./_bench_str2num
cc -Wall -O3 -o _bench_str2num -I . -I ../callback_printf ../callback_printf/callback_printf.c  ../callback_printf/sfprintf.c bench_str2num.c str2num.c
./_bench_str2num
exit $?
#endif

/*****************************************************************************\
*                                                                             *
*  FILENAME:      bench_str2num.c                                             *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:   tests and benchmark of str2num API functions                *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT:     (c) 2025 Dipl.-Ing. Klaus Lux (Aachen, Germany)             *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  ORIGIN:        https://github/klux21/str2num                               *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
* Civil Usage Public License, Version 1.1, January 2024                       *
*                                                                             *
* Redistribution and use in source and binary forms, with or without          *
* modification, are permitted provided that the following conditions are met: *
*                                                                             *
* 1. Redistributions of source code must retain the above copyright           *
*    notice, this list of conditions, the explanation of terms                *
*    and the following disclaimer.                                            *
*                                                                             *
* 2. Redistributions in binary form must reproduce the above copyright        *
*    notice, this list of conditions and the following disclaimer in the      *
*    documentation or other materials provided with the distribution.         *
*                                                                             *
* 3. All modified files must carry prominent notices stating that the         *
*    files have been changed.                                                 *
*                                                                             *
* 4. The source code and binary forms and any derivative works are not        *
*    stored or executed in systems or devices which are designed or           *
*    intended to harm, to kill or to forcibly immobilize people.              *
*                                                                             *
* 5. The source code and binary forms and any derivative works are not        *
*    stored or executed in systems or devices which are intended to           *
*    monitor, to track, to change or to control the behavior, the             *
*    constitution, the location or the communication of any people or         *
*    their property without the explicit and prior agreement of those         *
*    people except those devices and systems are solely designed for          *
*    saving or protecting peoples life or health.                             *
*                                                                             *
* 6. The source code and binary forms and any derivative works are not        *
*    stored or executed in any systems or devices that are intended           *
*    for the production of any of the systems or devices that                 *
*    have been stated before except the ones for saving or protecting         *
*    peoples life or health only.                                             *
*                                                                             *
* The term 'systems' in all clauses shall include all types and combinations  *
* of physical, virtualized or simulated hardware and software and any kind    *
* of data storage.                                                            *
*                                                                             *
* The term 'devices' shall include any kind of local or non-local control     *
* system of the stated devices as part of that device als well. Any assembly  *
* of more than one device is one and the same device regarding this license.  *
*                                                                             *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  *
* POSSIBILITY OF SUCH DAMAGE.                                                 *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#if defined (_WIN32) || defined (__CYGWIN__)

#include <windows.h>

#if defined (_WIN32)
#define strtoll     _strtoi64
#define strtoull    _strtoui64
#endif

#else
#include <sys/time.h>
#endif
#include <inttypes.h>

#define PRINTF_LIKE_ARGS(fmtidx, argidx)
#include <sfprintf.h>
#include <str2num.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

#if defined (_WIN32) || defined (__CYGWIN__)

static void (WINAPI * vGetSystemTimePreciseAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);
static HMODULE hmKernel32Dll = (HMODULE) -1;

/* ------------------------------------------------------------------------- *\
   unix_time returns the Unix time in microsecond (time since 01/01/1970)
\* ------------------------------------------------------------------------- */
int64_t TimeStamp()
{
   int64_t iRet;
   FILETIME CurrentTime;

   if(vGetSystemTimePreciseAsFileTime)
   {
      vGetSystemTimePreciseAsFileTime(&CurrentTime);
   }
   else if(hmKernel32Dll == (HMODULE) -1)
   { /* 1rst call */
      hmKernel32Dll = LoadLibrary("Kernel32.dll");
      if(hmKernel32Dll)
         vGetSystemTimePreciseAsFileTime = (void (WINAPI * )(LPFILETIME)) GetProcAddress(hmKernel32Dll, "GetSystemTimePreciseAsFileTime");

      if(vGetSystemTimePreciseAsFileTime)
         vGetSystemTimePreciseAsFileTime(&CurrentTime);
      else
         GetSystemTimeAsFileTime(&CurrentTime);
   }
   else
   {
      GetSystemTimeAsFileTime(&CurrentTime);
   }

   iRet  = ((int64_t) CurrentTime.dwHighDateTime << 32);
   iRet += (int64_t)  CurrentTime.dwLowDateTime;
   iRet -= (int64_t)  116444736 * 1000000 * 1000; /* offset of Windows FileTime to start of Unix time */
   return (iRet / 10);
}/* int64_t TimeStamp() */

#else

int64_t TimeStamp()
{
   int64_t tRet;
   struct timeval tv;

   gettimeofday(&tv, NULL);

   tRet = (int64_t) tv.tv_sec;

   /* Try to turn the year 2038 problem into a year 2106 problem. */
   if((sizeof(time_t) <= 4) && (tv.tv_sec < 0))
      tRet += (int64_t) 0x80000000ul + (int64_t) 0x80000000ul;

   tRet *= 1000000ul;
   tRet += tv.tv_usec;
   return (tRet);
}/* int64_t TimeStamp() */
#endif


int run_tests()
{
   int iret = 0;

   time_t t0 = TimeStamp();
   time_t t1 = TimeStamp();
   int64_t i;
   long ol;
   long nl;
   long long oll;
   long long nll;
   unsigned long olu;
   unsigned long nlu;
   unsigned long long ollu;
   unsigned long long nllu;
   char * pe;
   char * pr;
   const char * ps = " -2134567890-2 ";

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nl = r_str2l(ps + (i&7), NULL, 0, NULL);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ___ r_str2l() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nl = str2l(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average _____ str2l() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      ol = strtol(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ____ strtol() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 100000;
   while (i--)
   {
      nl = str2l(ps + ((i+(i>>4))&15), &pe, 2 + (i & 0x1f));
      ol = strtol(ps + ((i+(i>>4))&15), &pr, 2 + (i & 0x1f));

      if (ol != nl)
      {
         sfprintf(stderr, "Return values of strtol() and str2l() differ for base %d! (%ld != %ld)", (int)(2 + (i & 0x1f)), ol, nl);
         goto Exit;
      }
      if(pe != pr)
      {
         ps += ((i+(i>>4))&15);
         sfprintf(stderr, "The returned endpointer strtol() and str2l() differ for base %d! "
                 "('%.16s' != '%.16s', scan '%.*s' != '%.*s')\n", (int)(2 + (i & 0x1f)), pr, pe, (int)(pr-ps), ps, (int)(pe-ps), ps);
         goto Exit;
      }
   }

   ps = " -12345678901234567890  -54321 ";
   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nll = r_str2ll(ps + (i&15), NULL, 0, NULL);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average __ r_str2ll() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nll = str2ll(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ____ str2ll() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      oll = strtoll(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ___ strtoll() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 100000;
   while (i--)
   {
      nll = str2ll(ps + ((i+(i>>4))&31), &pe, 2 + (i & 0x1f));
      oll = strtoll(ps + ((i+(i>>4))&31), &pr, 2 + (i & 0x1f));

      if (oll != nll)
      {
         sfprintf(stderr, "Return values of strtoll() and str2ll() differ for base %d! (%lld != %lld)\n", (int)(2 + (i & 0x1f)), oll, nll);
         goto Exit;
      }
      if(pe != pr)
      {
         ps += ((i+(i>>4))&31);
         sfprintf(stderr, "The returned endpointer strtoll() and str2ll() differ for base %d! "
                 "('%.16s' != '%.16s', scan '%.*s' != '%.*s')\n", (int)(2 + (i & 0x1f)), pr, pe, (int)(pr-ps), ps, (int)(pe-ps), ps);
         goto Exit;
      }
   }

   ps = " 1234567890 -2 ";
   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nlu = r_str2ul(ps + (i&7), NULL, 0, NULL);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average __ r_str2ul() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nlu = str2ul(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ____ str2ul() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      olu = strtoul(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ___ strtoul() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 100000;
   while (i--)
   {
      nlu = str2ul(ps + ((i+(i>>4))&15), &pe, 2 + (i & 0x1f));
      olu = strtoul(ps + ((i+(i>>4))&15), &pr, 2 + (i & 0x1f));
      if (olu != nlu)
      {
         sfprintf(stdout, "Return values of strtoul() and str2ul() differ for base %d! (%lu != %lu)\n", (int)(2 + (i & 0x1f)), olu, nlu);
         goto Exit;
      }
      if(pe != pr)
      {
         ps += ((i+(i>>4))&15);
         sfprintf(stderr, "The returned endpointer strtoul() and str2ul() differ for base %d! "
                 "('%.16s' != '%.16s', scan '%.*s' != '%.*s')\n", (int)(2 + (i & 0x1f)), pr, pe, (int)(pr-ps), ps, (int)(pe-ps), ps);
         goto Exit;
      }
   }

   ps = "  12345678901234567890  -54321 ";

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nllu = r_str2ull(ps + (i&15), NULL, 0, NULL);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average _ r_str2ull() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nllu = str2ull(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average ___ str2ull() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      ollu = strtoull(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   sfprintf(stdout, "An average __ strtoull() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 100000;
   while (i--)
   {
      nllu = str2ull(ps + ((i+(i>>4))&31), &pe, 2 + (i & 0x1f));
      ollu = strtoull(ps + ((i+(i>>4))&31), &pr, 2 + (i & 0x1f));

      if (ollu != nllu)
      {
         sfprintf(stderr, "Return values of strtoull() and str2ull() differ for base %d! (%llu != %llu, '%s')\n",
                 (int)(2 + (i & 0x1f)), ollu, nllu, ps + ((i+(i>>4))&31));
         goto Exit;
      }
      if(pe != pr)
      {
         ps += ((i+(i>>4))&31);
         sfprintf(stderr, "The returned endpointer strtoull() and str2ull() differ for base %d! "
                 "('%.16s' != '%.16s', scan '%.*s' != '%.*s')\n", (int)(2 + (i & 0x1f)), pr, pe, (int)(pr-ps), ps, (int)(pe-ps), ps);
         goto Exit;
      }
   }

   iret = 1;
   Exit:;
   return (iret);
} /* run_tests() */

int test_str2value()
{
   int bRet = 0;
   char buf[256];

   int64_t l1;
   int64_t l2;
   uint64_t u1;
   uint64_t u2;
   char * p1;
   char * p2;
   int e1;
   int e2;
   int    base = 2;
   int64_t long_values[] = {0, 1, -1, LLONG_MIN, LLONG_MIN+1, LLONG_MIN+2, LLONG_MIN+3, LLONG_MAX, LLONG_MAX-1, LLONG_MAX-2, LLONG_MAX-3};

   while(base <= 36)
   {
      int64_t * pvalue = long_values;
      size_t count  = ARRAY_SIZE(long_values);
      while (count--) 
      {
          if ((base == 16) && (*pvalue > 0))
              ssprintf(buf, "0x%r*l8d", base, *pvalue++);
          else
              ssprintf(buf, "%r*l8d", base, *pvalue++);

          errno = 0;
          l1 = strtoll(buf, &p1, base);
          e1 = errno;
          errno = 0;
          l2 = str2ll(buf, &p2, base);
          e2 = errno;

          if ((l2 != l1) || (p1 != p2) || (e1 && !e2) || (e2 && !e1))
          {
             sfprintf(stderr, "Different return values of strtoll (%l8d (0x%l8x), pos %td, errno=%d)\n"
                      " and str2ll (%l8d (0x%l8x), pos %td, errno=%d)\n for base %d and string '%s'!\n",
                       l1, l1, p1-buf, e1, l2, l2, p2-buf, e2, base, buf);
             goto Exit;
          }

          errno = 0;
          u1 = strtoull(buf, &p1, base);
          e1 = errno;
          errno = 0;
          u2 = str2ull(buf, &p2, base);
          e2 = errno;

          if ((u2 != u1) || (p1 != p2) || (e1 && !e2) || (e2 && !e1))
          {
             sfprintf(stderr, "Different return values of strtoull (%l8u (0x%l8x), pos %td, errno=%d)\n"
                              " and str2ull (%l8u (0x%l8x), pos %td, errno=%d)\n for base %d and string '%s'!\n",
                       u1, u1, p1-buf, e1, u2, u2, p2-buf, e2, base, buf);
             goto Exit;
          }
      }
      ++base;
   }

   base = 2;
   while(base <= 36)
   {
      int64_t * pvalue = long_values;
      size_t count  = ARRAY_SIZE(long_values);
      while (count--) 
      {
          ssprintf(buf, "%r*l8d%d", base, *pvalue++, base-1);
          errno = 0;
          l1 = strtoll(buf, &p1, base);
          e1 = errno;
          errno = 0;
          l2 = str2ll(buf, &p2, base);
          e2 = errno;
          if ((l2 != l1) || (p1 != p2) || (e1 && !e2) || (e2 && !e1))
          {
             sfprintf(stderr, "Different return values of strtoll (%l8d (0x%l8x), pos %td, errno=%d)\n"
                              " and str2ll (%l8d (0x%l8x), pos %td, errno=%d)\n for base %d and string '%s'!\n",
                       l1, l1, p1-buf, e1, l2, l2, p2-buf, e2, base, buf);
             l2 = str2ll(buf, &p2, base);
             goto Exit;
          }

          errno = 0;
          u1 = strtoull(buf, &p1, base);
          e1 = errno;
          errno = 0;
          u2 = str2ull(buf, &p2, base);
          e2 = errno;

          if ((u2 != u1) || (p1 != p2) || (e1 && !e2) || (e2 && !e1))
          {
             sfprintf(stderr, "Different return values of strtoull (%l8u (0x%l8x), pos %td, errno=%d)\n"
                              " and str2ull (%l8u (0x%l8x), pos %td, errno=%d)\n for base %d and string '%s'!\n",
                       u1, u1, p1-buf, e1, u2, u2, p2-buf, e2, base, buf);
             u2 = str2ull(buf, &p2, base);
             //goto Exit;
          }
      }
      ++base;
   }

   bRet = 1;
   Exit:;
   return (bRet);
} /* int test_str2value */


/* ------------------------------------------------------------------------- *\
   main function
\* ------------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
    int iret = 1;
    if(!test_str2value())
        goto Exit;

    if(!run_tests())
        goto Exit;

    iret = 0;

    Exit:;

    if(!iret)
        sfprintf(stdout, "All tests passed!\n");
    else
        sfprintf(stderr, "Tests failed!\n");

    return (iret);
}/* main() */


/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
