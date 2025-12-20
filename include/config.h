#ifndef CONFIG_H
#define CONFIG_H

#include "env.h"
#include <stddef.h>

typedef struct shellConfig {
    char*           cwd;        // Cache the cwd to not regenerate it each time.
    unsigned char   exitCode;   // Last command's exit code.
    envArray*       env;        // Our local environment variables.
} sConfig;

sConfig*    initConfig(char** envp);
void        resetConfigCWD(sConfig* conf);
void        freeConfig(sConfig** conf);

#endif