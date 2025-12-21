#ifndef COMM_HANDLERS_H
#define COMM_HANDLERS_H

#include "config.h"
#include "parser.h"

typedef void (*HandleFunc)(CommList* list, Command* comm, sConfig* conf);

void handle_echo(CommList* list, Command* comm, sConfig* conf);
void handle_cd(CommList* list, Command* comm, sConfig* conf);
void handle_pwd(CommList* list, Command* comm, sConfig* conf);
void handle_export(CommList* list, Command* comm, sConfig* conf);
void handle_unset(CommList* list, Command* comm, sConfig* conf);
void handle_env(CommList* list, Command* comm, sConfig* conf);
void handle_exit(CommList* list, Command* comm, sConfig* conf);

#endif