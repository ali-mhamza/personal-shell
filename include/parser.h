#ifndef PARSER_H
#define PARSER_H

#include "config.h"
#include "token.h"
#include <stddef.h>

typedef struct comm {
    TokType commType;
    char*   name;
    char**  args;
    char*   heredoc;
    int     argCount;
    int     argCapacity;
    int     redirectIn;
    int     redirectOut;
} Command;

typedef struct list {
    Command**    comms;
    size_t      count;
    size_t      capacity;
} CommList;

CommList*   parseCommands(sConfig* conf, TokenObj* tokens);
void        freeCommList(CommList** list);

#endif