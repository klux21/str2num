str2num provides very fast, portable and full featured wrappers for
strtol, strtoul, strtoll and strtoull like functions.

The implementation doesn't use any integer divisions and for this it is
much faster than the implementation of the of any compilers that I have
tested until now, including gcc, clang, Microsoft Visual Studio on
several plattforms. The API currently contains the following functions


 int64_t  str2i64  (const char * ps, char ** pe, int base);
uint64_t  str2u64  (const char * ps, char ** pe, int base);

 int32_t  str2i32  (const char * ps, char ** pe, int base);
uint32_t  str2u32  (const char * ps, char ** pe, int base);

 int16_t  str2i16  (const char * ps, char ** pe, int base);
uint16_t  str2u16  (const char * ps, char ** pe, int base);

 int8_t   str2i8   (const char * ps, char ** pe, int base);
uint8_t   str2u8   (const char * ps, char ** pe, int base);

ptrdiff_t str2sz   (const char * ps, char ** pe, int base);
size_t    str2uz   (const char * ps, char ** pe, int base);

         int str2i (const char * ps, char ** pe, int base);
unsigned int str2u (const char * ps, char ** pe, int base);

         long str2l  (const char * ps, char ** pe, int base);
unsigned long str2ul (const char * ps, char ** pe, int base);

         long long str2ll  (const char * ps, char ** pe, int base);
unsigned long long str2ull (const char * ps, char ** pe, int base);


All functions implement overflow handling according to the returned types
and support the common numeric bases between 2 and 36.

Adding the support of other signed or unsigned integer types is very trivial
because the implementation only requires some macros for the types and their
limits and a subsequent include of str2num.c .

The implementations uses the zlib license for allowing the integration in
nearly all compilers and professional software.

Please leave a star once you like this project and have a look at my time API
functions in https://github.com/klux21/limitless_times which provides truly fast,
portable and an easy to use time and timezone handling functions and at my
portable printf family wrappers in https://github.com/klux21/callback_printf
as well. 

Kind regards,

Klaus Lux
