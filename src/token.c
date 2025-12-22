#include "../include/token.h"
#include "../include/common.h"
#include <stddef.h>
#include <stdlib.h>

TokenObj* initTokenObj()
{
    TokenObj* obj = malloc(sizeof(TokenObj));
    if (obj != NULL)
    {
        obj->tokStrs = NULL;
        obj->tokTypes = NULL;
        obj->count = 0;
        obj->capacity = 0;
    }

    return obj;
}

void freeTokenObj(TokenObj** obj)
{
    if (!obj || !(*obj))
        return;
    
    for (size_t i = 0; i < (*obj)->count; i++)
        free((*obj)->tokStrs[i]);
    free((*obj)->tokStrs);
    free((*obj)->tokTypes);
    free(*obj);
    *obj = NULL;
}

static void reallocTokenObj(TokenObj* obj, size_t newSize)
{
    if (newSize == 0)
        return;

    char** newStrs = malloc(newSize * sizeof(char*));
    TokType* newTypes = malloc(newSize * sizeof(TokType));
    if ((newStrs == NULL) || (newTypes == NULL))
        return;
    
    for (size_t i = 0; i < obj->count; i++)
    {
        newStrs[i] = obj->tokStrs[i];
        newTypes[i] = obj->tokTypes[i];
    }

    free(obj->tokStrs);
    free(obj->tokTypes);
    obj->tokStrs = newStrs;
    obj->tokTypes = newTypes;
    obj->capacity = newSize;
}

void addToken(TokenObj* obj, const char* text, size_t size, TokType type)
{
    if (size == (size_t) -1)
        size = strlen(text);
    
    if (obj->capacity < obj->count + 1)
        reallocTokenObj(obj, obj->capacity == 0 ? 8 : obj->capacity * 2);

    // Check if strndup() exists within the allowed C standard.
    if (text != NULL)
        obj->tokStrs[obj->count] = strndup(text, size); // We do not own the passed string.
    else
        obj->tokStrs[obj->count] = NULL;
    obj->tokTypes[obj->count] = type;
    if (type != T_NULL)
        obj->count++;
}