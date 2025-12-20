#ifndef ENV_H
#define ENV_H

#include <stddef.h>

// The internal array may contain NULL elements
// between valid env variables due to variable removal.
// Thus, when iterating over the array, always rely on
// the count, not whether or not the current element is NULL.
typedef struct envArray {
    char**  envVars;    // Holds the names of all our environment variables.
    char**  envValues;  // Holds the values of all our environment variables.
    size_t  count;
    size_t  capacity;
} envArray;

envArray*   initEnvArray(char** envp);
char*       getEnvVar(envArray* arr, char* varName);
void        setEnvVar(envArray* arr, char* varName, char* varValue);
void        removeEnvVar(envArray* arr, char* varName);
char**      formExecEnv(envArray* arr);
void        freeEnvArray(envArray** arr);

#endif