#define N_VA_ARGS_(_9,_8,_7,_6,_5,_4,_3,_2,_1, N, ...) N
#define N_VA_ARGS(...) N_VA_ARGS_(__VA_ARGS__ __VA_OPT__(,) 9,8,7,6,5,4,3,2,1,0)

#define FOREACH_0(FN, ...)
#define FOREACH_1(FN, E, ...)  FN(E)
#define FOREACH_2(FN, E, ...)  FN(E) FOREACH_1(FN, __VA_ARGS__)
#define FOREACH_3(FN, E, ...)  FN(E) FOREACH_2(FN, __VA_ARGS__)
#define FOREACH_4(FN, E, ...)  FN(E) FOREACH_3(FN, __VA_ARGS__)
#define FOREACH_5(FN, E, ...)  FN(E) FOREACH_4(FN, __VA_ARGS__)
#define FOREACH_6(FN, E, ...)  FN(E) FOREACH_5(FN, __VA_ARGS__)
#define FOREACH_7(FN, E, ...)  FN(E) FOREACH_6(FN, __VA_ARGS__)
#define FOREACH_8(FN, E, ...)  FN(E) FOREACH_7(FN, __VA_ARGS__)
#define FOREACH_9(FN, E, ...)  FN(E) FOREACH_8(FN, __VA_ARGS__)

#define FOREACH__(FN, NARGS, ...) FOREACH_##NARGS(FN, __VA_ARGS__)
#define FOREACH_(FN, NARGS, ...) FOREACH__(FN, NARGS, __VA_ARGS__)
#define FOREACH(FN, ...) FOREACH_(FN, N_VA_ARGS(__VA_ARGS__), __VA_ARGS__)






#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void fmt_make_format(const char *fmt, const char *types, char *out) {
    for ( ; *fmt; fmt++) {
        if (*fmt == '{') {
            *out++ = '%';
            if (*++fmt == ':') {
                while (fmt[1] != '}')
                    *out++ = *++fmt;
                if (!strchr("diouxXeEfFgGaAcsp", *fmt))
                    *out++ = *types;
                fmt++;
            }
            else
                *out++ = *types;
            types++;
        }
        else if (*fmt == '%') {
            *out++ = '%';
            *out++ = '%';
        }
        else
            *out++ = *fmt;
    }
    *out = 0;
}

size_t fmt_get_size(const char *fmt, const char *types) {
    size_t percents = 0;
    for (const char *f = fmt; *f; f++)
        if (*f == '%') percents++;

    return strlen(fmt) + strlen(types) + percents + 1;
}

int fmt_fprint_(FILE *stream, const char *fmt, const char* types, ...) {
    char printf_format[fmt_get_size(fmt, types)];
    fmt_make_format(fmt, types, printf_format);

    va_list valist;
    va_start(valist, types);
    int ret = vfprintf(stream, printf_format, valist);
    va_end(valist);

    return ret;
}

int fmt_snprint_(char *dest, size_t destsize, const char *fmt, const char* types, ...) {
    char printf_format[fmt_get_size(fmt, types)];
    fmt_make_format(fmt, types, printf_format);

    va_list valist;
    va_start(valist, types);
    int ret = vsnprintf(dest, destsize, printf_format, valist);
    va_end(valist);

    return ret;
}

#define argtype(x) _Generic((x), \
                   char : 'c', unsigned char : 'u', \
                   short: 'd', unsigned short: 'u', \
                   int  : 'd', unsigned int  : 'u', \
                   float: 'g', double        : 'g', \
                   char*: 's', const char*   : 's', \
                   void*: 'p'),

#define fmt_print(              fmt, ...) fmt_fprint_ (stdout,        fmt, (char[]){ FOREACH(argtype, __VA_ARGS__) 0}, ##__VA_ARGS__)
#define fmt_fprint( file,       fmt, ...) fmt_fprint_ (file,          fmt, (char[]){ FOREACH(argtype, __VA_ARGS__) 0}, ##__VA_ARGS__)
#define fmt_sprint( dest,       fmt, ...) fmt_snprint_(dest, INT_MAX, fmt, (char[]){ FOREACH(argtype, __VA_ARGS__) 0}, ##__VA_ARGS__)
#define fmt_snprint(dest, size, fmt, ...) fmt_snprint_(dest,    size, fmt, (char[]){ FOREACH(argtype, __VA_ARGS__) 0}, ##__VA_ARGS__)






int main() {
    fmt_print("{:x} {} {} {}\n", 0x123, 4, 5.6, "789");
    fmt_print("{:04} {:.4}\n", 123, 45.6789);
    fmt_print("{:.4} {}\n", "meow meow meow", (void*)main);
    char buffer[100];
    fmt_snprint(buffer, sizeof buffer, "{} {} {} {}", "hello", "world", (char)'!', 1234);
    fmt_print("{}\n", buffer);
}
