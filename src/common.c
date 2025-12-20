#include "../include/common.h"
#include <stdlib.h>

static bool isVarChar(char c)
{
    return (isalpha(c) || (c == '_'));
}

bool isValidVar(char* name)
{
    if (strlen(name) == 0)
        return false;

    if (!isVarChar(name[0]))
        return false;
    
    name++;
    while (*name != '\0')
    {
        if (!isalnum(*name) && !(*name == '_'))
            return false;
        name++;
    }

    return true;
}

bool isWordChar(char c)
{
    return (isalnum(c) || c == '.' || c == '/'
            || c == '_' || c == '-' || c == '$'
            || c == '?' || c == '+');
}

// - `start` points to *after* the $ sign.
// Work in progress.
char*   expandEnv(sConfig* conf, char* start, size_t* origSize, size_t* index)
{
    // We need the config object here.
    if (start[0] == '?')
    {
        char* temp = itoa(conf->exitCode);
        size_t size = strlen(temp);
        if (index != NULL)
            (*index) += (size + 1);
        if (origSize != NULL)
            (*origSize) += (size - 2); // -1 for len('?') and another -1.
        return temp;
    }
    
    size_t size = 0;
    while (isWordChar(start[size]) && (start[size] != '$'))
        size++;
    char* temp = strndup(start, size);
    char* ret = getEnvVar(conf->env, temp);
    if (index != NULL)
        (*index) += (size + 1);
    if (origSize != NULL)
    {
        long long diff = (ret != NULL ? 
                        (long long) strlen(ret) - strlen(temp)
                        : -1 * strlen(temp));
        (*origSize) += diff - 1;
    }
    free(temp);
    return ret;
}