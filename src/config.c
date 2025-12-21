#include "../include/config.h"
#include "../include/common.h"
#include <stdlib.h>
#include <unistd.h>

sConfig* initConfig(char** envp)
{
    sConfig* conf = malloc(sizeof(sConfig));
    if (conf != NULL)
    {
        conf->cwd = NULL;
        conf->oldpwd = NULL;
        conf->exitCode = 0;
        conf->env = initEnvArray(envp); // Not set to NULL.
    }

    return conf;
}

void resetConfigCWD(sConfig* conf)
{
    free(conf->cwd);
    char temp[1024] = {0};
    getcwd(temp, 1023);
    conf->cwd = strdup(temp);
}

void freeConfig(sConfig** conf)
{
    free((*conf)->cwd);
    free((*conf)->oldpwd);
    freeEnvArray(&((*conf)->env));
    free(*conf);
    *conf = NULL;
}