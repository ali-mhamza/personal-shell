#include "../include/strbuf.h"
#include "../include/common.h"
#include <stdlib.h>

strbuf* initBuf()
{
	strbuf	*buf = malloc(sizeof(strbuf));
	if (buf == NULL)
		return (NULL);
	buf->chars = NULL;
	buf->capacity = 0;
	buf->count = 0;
	return buf;
}

static void reallocBuf(strbuf *buf, size_t newSize)
{
	char* newChars = calloc(newSize, sizeof(char));
	if (!newChars)
		return ;
	if (buf->chars)
	{
        for (size_t i = 0; i < buf->count; i++)
			newChars[i] = buf->chars[i];
		free(buf->chars);
	}
	buf->chars = newChars;
	buf->capacity = newSize;
}

void appendBuf(strbuf *buf, const char *str, size_t size)
{
	if ((str == NULL) || (size == 0))
		return ;
	if (size == (size_t) -1)
		size = strlen(str);
	while (buf->capacity < buf->count + size + 1)
        reallocBuf(buf, buf->capacity == 0 ? 8 : buf->capacity * 2);
	size_t i = 0;
	while ((str[i] != '\0') && (i < size))
	{
		buf->chars[buf->count + i] = str[i];
		i++;
	}
	buf->count += i;
}

char* freeBuf(strbuf **buf, int freeChars)
{
	if (!buf || !(*buf))
		return (NULL);

	char* temp = NULL;
	if (freeChars)
		free((*buf)->chars);
	else
		temp = (*buf)->chars;
	free(*buf);
	*buf = NULL;
	return temp;
}