#include "../include/error.h"
#include "../include/common.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

void reportError(sConfig* conf, unsigned char exitCode, 
    const char* type, const char* format, ...)
{
    write(2, type, strlen(type));
    write(2, ": ", 2);
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    write(2, "\n", 1);
    va_end(ap);
    conf->exitCode = exitCode;
}