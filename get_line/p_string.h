#ifndef PSTRING_H
#define PSTRING_H

#include "common.h"

typedef struct String {
    char*   chars;
    u32     count;
    u32     capacity;
} String;

String* initString(String* str, u32 size);
bool    appendCStr(String* to, const char* from, u32 size);
void    resetString(String* str);

#undef FREE_CHARS
#undef NO_FREE_CHARS

#define FREE_CHARS      true
#define NO_FREE_CHARS   false

char*   freeString(String** str, bool freeChars);

#endif
