#ifndef CONFIG_H
#define CONFIG_H

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

typedef struct shellConfig {
    char*           cwd;        // Cache the cwd to not regenerate it each time.
    unsigned char   exitCode;   // Last command's exit code.
    envArray*       env;        // Our local environment variables.
} sConfig;

sConfig* initConfig(char** envp);
void resetConfigCWD(sConfig* conf);
void freeConfig(sConfig** conf);

char* getEnvVar(envArray* arr, char* varName);
void setEnvVar(envArray* arr, char* varName, char* varValue);
void removeEnvVar(envArray* arr, char* varName);
char** formConfEnv(envArray* arr, size_t* size);

#endif