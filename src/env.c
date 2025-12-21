#include "../include/env.h"
#include "../include/common.h"

envArray* initEnvArray(char** envp)
{
    envArray* arr = malloc(sizeof(envArray));
    arr->count = 0;
    arr->capacity = 0;
    arr->envVars = NULL;
    arr->envValues = NULL;

    if ((arr != NULL) && (envp != NULL))
    {
        for (int i = 0; envp[i] != NULL; i++)
        {
            char* stop = strchr(envp[i], '=');
            if (stop != NULL)
            {
                char* var = strndup(envp[i], stop - envp[i]);
                setEnvVar(arr, var, stop + 1);
                free(var); // setEnvVar does not take ownership of the string.
            }
            else
                setEnvVar(arr, envp[i], NULL);
        }
    }

    return arr;
}

char* getEnvVar(envArray* arr, char* varName)
{
    for (size_t i = 0; i < arr->count; i++)
    {
        if (arr->envVars[i] == NULL)
            continue;
        else if (!strcmp(arr->envVars[i], varName))
            return strdup(arr->envValues[i]); // Can be freed without issue.
    }

    return strdup("");
}

static void reallocEnvArray(envArray* arr, size_t newSize)
{
    if (newSize == 0)
        return;

    char** newVars = calloc(newSize, sizeof(char*));
    char** newValues = calloc(newSize, sizeof(char*));
    if ((newVars == NULL) || (newValues == NULL))
        return;

    // We could optimize this to remove the NULL var name
    // entries and update the count for this reallocated
    // array.
    //
    // This will help us avoid wasting space for NULL entries
    // when we unset many environment variables.

    for (size_t i = 0; i < arr->count; i++)
    {
        newVars[i] = arr->envVars[i];
        newValues[i] = arr->envValues[i];
    }

    free(arr->envVars);
    free(arr->envValues);
    arr->envVars = newVars;
    arr->envValues = newValues;
    arr->capacity = newSize;
}

void setEnvVar(envArray* arr, char* varName, char* varValue)
{
    // Check if we already have it.
    
    for (size_t i = 0; i < arr->count; i++)
    {
        if (arr->envVars[i] == NULL)
            continue;
        else if (!strcmp(varName, arr->envVars[i]))
        {
            free(arr->envValues[i]);
            arr->envValues[i] = strdup(varValue);
            return;
        }
    }
    
    // We don't, so we have to add it.

    if (arr->capacity < arr->count + 1)
        reallocEnvArray(arr, arr->capacity == 0 ? 8 : arr->capacity * 2);
    
    arr->envVars[arr->count] = strdup(varName); // We do not own the passed strings.
    if (varValue != NULL)
        arr->envValues[arr->count] = strdup(varValue);
    else
        arr->envValues[arr->count] = NULL;
    arr->count++;
}

void removeEnvVar(envArray* arr, char* varName)
{
    for (size_t i = 0; i < arr->count; i++)
    {
        if (!strcmp(arr->envVars[i], varName))
        {
            free(arr->envVars[i]);
            arr->envVars[i] = NULL;
            free(arr->envValues[i]);
            arr->envValues[i] = NULL;
            // Don't decrement the count so we
            // can still loop to the end of the array.
            break;
        }
    }
}

char** formExecEnv(envArray* arr)
{
    char** newEnv = calloc((arr->count + 1), sizeof(char*));
    if (newEnv == NULL)
        return NULL;
    size_t size = 0;
    for (size_t i = 0; i < arr->count; i++)
    {
        if (arr->envVars[i] == NULL)
            continue;
        if (arr->envValues[i] == NULL)
            newEnv[size++] = strdup(arr->envVars[i]);
        else
        {
            char* temp = strjoin(arr->envVars[i], "=");
            newEnv[size++] = strjoin(temp, arr->envValues[i]);
            free(temp);
        }
    }

    return newEnv;
}

void freeEnvArray(envArray** arr)
{
    for (size_t i = 0; i < (*arr)->count; i++)
    {
        free((*arr)->envVars[i]);
        free((*arr)->envValues[i]);
    }
    free((*arr)->envVars);
    free((*arr)->envValues);
    free(*arr);
    *arr = NULL;
}