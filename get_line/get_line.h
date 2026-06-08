#ifndef GET_LINE_H
#define GET_LINE_H

#include <stdbool.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
	#include <io.h>
	#define READ _read
#else
	#include <unistd.h>
	#define READ read
#endif

#ifndef BUFFER_SIZE
	#define BUFFER_SIZE 128
#elif BUFFER_SIZE <= 0
	#error "BUFFER_SIZE must be greater than 0."
#elif BUFFER_SIZE > 8000
	#error "BUFFER_SIZE too large."
#endif

char*	getLine(int fd);

#endif