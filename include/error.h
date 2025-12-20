#ifndef ERROR_H
#define ERROR_H

#include "config.h"

#define GEN_ERROR       1
#define NOT_EXEC        126
#define NOT_FOUND       127
#define INVALID_EXIT    128
#define EXIT_RANGE      255

void reportError(sConfig* conf, unsigned char exitCode,
    const char* type, const char* format, ...);

#endif