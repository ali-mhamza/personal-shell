#ifndef ERROR_H
#define ERROR_H

#include "config.h"

#define GEN_ERROR       1       // Generic error exit code.
#define INVALID_EXIT    2       // Exit code format is invalid (e.g., decimal number or containing letters).
#define NOT_EXEC        126     // File given is not executable (lack of permissions, wrong file type, etc.).
#define NOT_FOUND       127     // Executable given was not found.
#define EXIT_RANGE      255     // Exit code is outside of valid range 0-255 (i.e., negative or >255).

static inline void setConfigExitCode(sConfig* conf, unsigned char exitCode)
{
    conf->exitCode = exitCode;
}

void reportError(const char* type, const char* format, ...);

#endif