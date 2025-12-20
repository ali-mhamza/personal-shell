#ifndef COMM_HANDLERS_H
#define COMM_HANDLERS_H

#include "config.h"
#include "token.h"

typedef void (*HandleFunc)(TokenObj* tokens, sConfig* conf);

void handle_echo(TokenObj* tokens, sConfig* conf);
void handle_cd(TokenObj* tokens, sConfig* conf);
void handle_pwd(TokenObj* tokens, sConfig* conf);
void handle_export(TokenObj* tokens, sConfig* conf);
void handle_unset(TokenObj* tokens, sConfig* conf);
void handle_env(TokenObj* tokens, sConfig* conf);
void handle_exit(TokenObj* tokens, sConfig* conf);

#endif