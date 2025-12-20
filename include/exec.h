#ifndef EXEC_H
#define EXEC_H

#include "config.h"
#include "strbuf.h"

void runExec(char* path, char** args, sConfig* conf);

#endif