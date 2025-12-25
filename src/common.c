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
            || c == '?' || c == '+' || c == '~');
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
    if (!isalnum(start[size]))
    {
        (*index)++;
        return strdup("$");
    }
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

char* expandInPlace(sConfig* conf, char* line, size_t *size)
{
    if (line == NULL)
        return NULL;
    
    char* final = line;
    char* start = final;
    char* temp;
    while (((temp = strchr(start, '$')) != NULL)
            && (temp < final + *size))
    {
        // Idea:
        // Split quote around variable: [Welcome, ][$var1][, to this house.]
        // Replace variable: [Welcome, ][Malcolm][, to this house.]
        // Join back together: Welcome, Malcom, to this house.
        
        // We should probably add some kind of strReplace function
        // to do this properly and independently.

        size_t index = temp - final;
        char* first = strndup(final, temp - final);
        char* insert = expandEnv(conf, temp + 1, size, &index);
        char* part = strjoin(first, insert);
        final = strjoin(part, final + index);
        start += index;
        free(first);
        free(insert);
        free(part);
    }

    return final;
}