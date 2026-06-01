str2num provides very fast, portable and full featured wrappers for
strtol, strtoul, strtoll and strtoull like functions.

The implementation doesn't use any integer divisions and for this it is much
faster than the common implementation of several compilers that I have tested
until now, including gcc, clang and Visual Studio on several plattforms.
The interface currently provides the following most important functions


 int64_t  str2i64  (const char * ps, char ** pe, int base);
uint64_t  str2u64  (const char * ps, char ** pe, int base);

 int32_t  str2i32  (const char * ps, char ** pe, int base);
uint32_t  str2u32  (const char * ps, char ** pe, int base);

 int16_t  str2i16  (const char * ps, char ** pe, int base);
uint16_t  str2u16  (const char * ps, char ** pe, int base);

 int8_t   str2i8   (const char * ps, char ** pe, int base);
uint8_t   str2u8   (const char * ps, char ** pe, int base);

ptrdiff_t str2pd   (const char * ps, char ** pe, int base);
size_t    str2sz   (const char * ps, char ** pe, int base);

intptr_t  str2ip   (const char * ps, char ** pe, int base);
uintptr_t str2up   (const char * ps, char ** pe, int base);

         int str2i (const char * ps, char ** pe, int base);
unsigned int str2u (const char * ps, char ** pe, int base);

         long str2l  (const char * ps, char ** pe, int base);
unsigned long str2ul (const char * ps, char ** pe, int base);

         long long str2ll  (const char * ps, char ** pe, int base);
unsigned long long str2ull (const char * ps, char ** pe, int base);


All functions implement overflow handling according to the C standards and the
several types. All function support all numeric bases between 2 and 36.

All function recognize the prefixes 0b for dual numbers, 0o for octal numbers
and 0x for hexadecimal numbers if the base is set to 0 or 1.

If the base is set to 1 than a different recognition of the numeric base
happens. All numbers with leading zeros are recognized as decimal values
and all octal numbers require the prefix 0o for being recognized as octal
values. This prevents unintended octal numbers because of a leading 0.
    
For all functions exist a reentrant safe version with an _r appended to
the function name. The *_r versions have an optional pointer to an int as
last parameter for storing the error value (or 0 in success case) instead
using errno. This is easier to use and slightly faster as well.

( Note: In version 2.0 the previous r_* names of those functions were
  changed to a more common *_r name. The names of the functions for
  reading  ptrdiff_t and size_t values were adjusted too. ) 

Adding the support of other signed or unsigned integer types is very trivial
because the implementation only requires some few macros for the types and
their limits and a subsequent recursive include of str2num.c that acts like
a template as well. 

There are a bunch of functions for reading floating point numbers of different
numeric bases from a string in the format that callback_printf uses where the
exponent of numeric bases higher than 15 starts after a preceeding tilde '~'
instead of an 'e' and is always of the same numeric base as the mantissa.
The numeric format of the %a printf output where the mantisse is hexadecimal
and the exponent is for a base 2 mantissa but given decimal after a 'p' is
supported as well. There are

long double str2ld_r (const char * psrc, char ** pend, int base, int * perr);
double      str2d_r  (const char * psrc, char ** pend, int base, int * perr);

and some wrappers of strtof, strtod and strtold which are using those

long double str2ld (const char * psrc, char ** pend);
double      str2d  (const char * psrc, char ** pend);
float       str2f  (const char * psrc, char ** pend);

The mantissa of dual and hexadecimal numbers must be prefixed with 0b or 0x
for an automatic recognition of the base where you can't specify the base as
an argument. str2d_r and str2ld_r are doing an automatic base recognition if
the base is set to 0.

Because of the generic calculations the mantissa of the returned numbers may
slightly deviate in the least significant digits from the one the exact value
if the numeric base of the value is not a power of 2. Denormalized numbers as
well as infinity and NaN are supported according to the C standard.
The code doesn't depend on the math library but the code is pretty new and the
functions are not yet much tested beside of the tests in bench_str2num.c .

This code uses the zlib license that allows a the free integration in common
compilers and professional software. The little test and benchmark program
(or script) bench_str2num.c uses the "Civil Usage Public License" however just
as callback_printf which is used for generating the strings in the tests.

Once you like this little project and have a look at my time API functions
https://github.com/klux21/limitless_times which provide truly fast, portable
and an easy to use time and timezone handling functions and the portable
sprintf wrappers at https://github.com/klux21/callback_printf too. Once you
have ever dealt with multiple platforms then you should know the advantages of
a solid base that you can count on. It's the mass of little problems that steal
your time and drain power and rarely the big tasks that seem just a trivial
thing to be done until the problems that bunch of forgotten details start.

Kind regards,

Klaus Lux
