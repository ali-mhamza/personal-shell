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
        conf->homedir = getEnvVar(conf->env, "HOME");
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
    if (!conf || !(*conf))
        return;
    
    free((*conf)->cwd);
    free((*conf)->oldpwd);
    free((*conf)->homedir);
    freeEnvArray(&((*conf)->env));
    free(*conf);
    *conf = NULL;
}