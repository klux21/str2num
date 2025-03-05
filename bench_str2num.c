#if 0
rm -f ./_bench_str2num
cc -Wall -O3 -o _bench_str2num -I . bench_str2num.c str2num.c
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
#include <inttypes.h>
#include <sys/types.h>

#if defined (_WIN32) || defined (__CYGWIN__)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <str2num.h>

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
   const char * ps = "-2134567890";

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nl = str2l(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average _____ str2l() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      ol = strtol(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average ____ strtol() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 10000;
   while (i--)
   {
      nl = str2l(ps + (i&7), NULL, 2 + (i & 0x1f));
      ol = strtol(ps + (i&7), NULL, 2 + (i & 0x1f));

      if (ol != nl)
      {
         fprintf(stderr, "Return values of strtol() and str2l() differ! (%ld != %ld)", ol, nl);
         goto Exit;
      }
   }

   ps = "-12345678901234567890";
   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nll = str2ll(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average ____ str2ll() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      oll = strtoll(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average ___ strtoll() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 10000;
   while (i--)
   {
      nll = str2ll(ps + (i&15), NULL, 2 + (i & 0x1f));
      oll = strtoll(ps + (i&15), NULL, 2 + (i & 0x1f));

      if (oll != nll)
      {
         fprintf(stderr, "Return values of strtoll() and str2ll() differ! (%lld != %lld)\n", oll, nll);
         goto Exit;
      }
   }

   ps = "1234567890";
   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nlu = str2ul(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average ____ str2ul() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      olu = strtoul(ps + (i&7), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average ___ strtoul() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 10000;
   while (i--)
   {
      nlu = str2ul(ps + (i&7), NULL, 2 + (i & 0x1f));
      olu = strtoul(ps + (i&7), NULL, 2 + (i & 0x1f));
      if (olu != nlu)
      {
         fprintf(stdout, "Return values of strtoul() and str2ul() differ! (%lu != %lu)\n", olu, nlu);
         goto Exit;
      }
   }

   ps = "12345678901234567890";
   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      nllu = str2ull(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average ___ str2ull() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = TimeStamp();
   while (i--)
      ollu = strtoull(ps + (i&15), NULL, 0);
   t1 = TimeStamp() - t0;
   fprintf(stdout, "An average __ strtoull() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 10000;
   while (i--)
   {
      nllu = str2ull(ps + (i&15), NULL, 2 + (i & 0x1f));
      ollu = strtoull(ps + (i&15), NULL, 2 + (i & 0x1f));

      if (ollu != nllu)
      {
         fprintf(stderr, "Return values of strtoull() and str2ull() differ! (%llu != %llu)\n", ollu, nllu);
         goto Exit;
      }
   }

   iret = 1;
   Exit:;
   return (iret);
} /* run_tests() */


/* ------------------------------------------------------------------------- *\
   main function
\* ------------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
    int iret = 1;

    if(!run_tests())
        goto Exit;

    iret = 0;

    Exit:;

    if(!iret)
        fprintf(stdout, "All tests passed!\n");
    else
        fprintf(stderr, "Tests failed!\n");

    return (iret);
}/* main() */


/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
