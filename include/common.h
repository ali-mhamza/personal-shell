#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include <stdbool.h>
#include <stdlib.h> // For atoi().
#include <string.h>
#include <ctype.h>

// Temporary.
char	**split(char const *s, char c);
size_t	strlcat(char *dst, const char *src, size_t dstsize);
char	*strjoin(char const *s1, char const *s2);
char    *itoa(int n);

bool    isValidVar(char* name);
bool    isWordChar(char c);
char*   expandEnv(sConfig* conf, char* start, size_t* origSize, size_t* index);
char*   expandInPlace(sConfig* conf, char* line, size_t *size);

#endif