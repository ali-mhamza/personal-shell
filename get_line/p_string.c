#define _CRT_SECURE_NO_WARNINGS

#include "p_string.h"
#include <string.h>

String* initString(String* str, u32 size)
{
    if (size == UINT32_MAX)
        return NULL;

    if (str == NULL)
    {
        str = (String*) malloc(sizeof(String));
        if (str == NULL)
            return NULL;
    }

    if (size != 0)
    {
        str->chars = (char*) malloc(size * sizeof(char));
        if (str->chars == NULL)
        {
            free(str);
            return NULL;
        }
        str->chars[0] = '\0';
        str->capacity = size;
    }
    else
    {
        str->chars = NULL;
        str->capacity = 0;
    }

    str->count = 0;
    return str;
}

void resetString(String* str)
{
    if ((str == NULL) || (str->chars == NULL))
        return;

    str->chars[0] = '\0';
    str->count = 0;
}

NODISCARD
static bool reallocString(String* str, u32 newSize)
{
    if ((str == NULL) || (newSize < str->count + 1)) // Cannot shrink.
        return false;

    char* temp = str->chars;
    str->chars = realloc(str->chars, newSize * sizeof(char));
    if (str->chars == NULL)
    {
        free(temp);
        return false;
    }

    str->capacity = newSize;
    return true;
}

bool appendCStr(String* to, const char* from, u32 size)
{
    if ((to == NULL) || (from == NULL))
        return false;

    u32 len = (size == 0 ? strlen(from) : size);
    if (to->count >= UINT32_MAX - len)
        return false;

    while (to->capacity < to->count + len + 1)
    {
        u32 newCapacity = (to->capacity == 0 ? 8 : to->capacity * 2);
        if (!reallocString(to, newCapacity))
            return false;
    }

    memcpy(to->chars + to->count, from, len);
    to->count += len;
    to->chars[to->count] = '\0';
    return true;
}

char* freeString(String** str, bool freeChars)
{
    if ((str == NULL) || (*str == NULL))
        return NULL;

    char* temp = NULL;
    if (freeChars)
        free((*str)->chars);
    else
        temp = (*str)->chars;
    free(*str);
    *str = NULL;
    return temp;
}
