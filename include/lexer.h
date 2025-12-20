#ifndef LEXER_H
#define LEXER_H

#include "config.h"
#include "token.h"

TokenObj* getTokens(sConfig* conf, char* line);

#endif