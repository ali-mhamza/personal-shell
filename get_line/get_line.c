#include "get_line.h"
#include "p_string.h"
#include <string.h>

static void	destFill(char* dest, const char* src, size_t size)
{
	if (src != NULL)
		memmove(dest, src, size);
	size_t i = (src == NULL ? 0 : size);
	memset(dest + i, 0, BUFFER_SIZE - i);
}

static char* checkNewline(String** str, char* temp)
{
	char* newline = strchr(temp, '\n');
	if (newline != NULL)
	{
		appendCStr(*str, temp, newline - temp + 1);
		destFill(temp, newline + 1, BUFFER_SIZE - (newline + 1 - temp));
		return freeString(str, NO_FREE_CHARS);
	}
	else
	{
		appendCStr(*str, temp, 0);
		destFill(temp, NULL, BUFFER_SIZE);
		return NULL;
	}
}

static char* readLine(int fd, String** str, char* temp)
{
	char* newline = NULL;
	long long charsRead = BUFFER_SIZE;
	while ((newline == NULL) && (charsRead == BUFFER_SIZE))
	{
		charsRead = READ(fd, temp, BUFFER_SIZE);
		newline = checkNewline(str, temp);
	}

	if ((newline == NULL) && (*str != NULL))
		return freeString(str, NO_FREE_CHARS);

	return newline;
}

char* getLine(int fd)
{
	static char temp[BUFFER_SIZE];
	String* str = initString(NULL, 0);
	if (str == NULL) return NULL;

	char* newline = checkNewline(&str, temp);
	if (newline != NULL)
		return newline;

	long long charsRead = READ(fd, temp, BUFFER_SIZE);
	if (charsRead <= 0)
	{
		if ((charsRead == 0) && (str->count != 0))
			return freeString(&str, NO_FREE_CHARS);
		return freeString(&str, FREE_CHARS);
	}

	newline = checkNewline(&str, temp);
	if (newline != NULL)
		return newline;
	if (charsRead == BUFFER_SIZE)
		return readLine(fd, &str, temp);

	return freeString(&str, NO_FREE_CHARS);
}