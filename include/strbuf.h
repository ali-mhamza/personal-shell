#ifndef STRBUF_H
#define STRBUF_H

#include <stdbool.h>
#include <stddef.h>

typedef struct buffer {
    char*   chars;
    size_t  count;
    size_t  capacity;
} strbuf;

strbuf* initBuf();
void	appendBuf(strbuf *buf, const char *str, size_t size);

#define FREE_CHARS      true
#define NO_FREE_CHARS   false

char*   freeBuf(strbuf **buf, int freeChars);

#endif