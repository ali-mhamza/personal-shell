#include "../include/comm_handlers.h"
#include "../include/common.h"
#include "../include/config.h"
#include "../include/env.h"
#include "../include/error.h"
#include "../include/parser.h"
#include "../include/sighandle.h"
#include <errno.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void handle_echo(CommList* list, Command* comm, sConfig* conf)
{
    bool noNewLine = false;
    int index = 1;
    
    (void) list; (void) conf;

    if ((comm->argCount > 1) && (!strcmp(comm->args[1], "-n")))
    {
        noNewLine = true;
        index++;
    }

    for (; index < comm->argCount; index++)
    {
        printf("%s%s", comm->args[index],
            (index == comm->argCount - 1 ? "" : " "));
    }
    printf("%s", noNewLine ? "" : "\n");
}

void handle_cd(CommList* list, Command* comm, sConfig* conf)
{
    (void) list;
    
    if (comm->argCount > 2)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Command Error", "Too many arguments for command '%s'.",
            comm->name);
        return;
    }

    char* path;
    if (comm->argCount == 1)
        path = strdup(conf->homedir);
    else if (strlen(comm->args[1]) == 1)
    {
        if (comm->args[1][0] == '~')
            path = strdup(conf->homedir);
        else if (comm->args[1][0] == '-')
        {
            if (conf->oldpwd == NULL)
            {
                setConfigExitCode(conf, GEN_ERROR);
                reportError("Directory Error", "No saved previous directory.");
                return;
            }
            path = strdup(conf->oldpwd);
        }
        else
            path = strdup(comm->args[1]);
    }
    else
        path = strdup(comm->args[1]);
    
    conf->oldpwd = strdup(conf->cwd);
    int ret = chdir(path);
    free(path);

    if (ret == -1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Argument Error", "%s.", strerror(errno));
    }
    else
        resetConfigCWD(conf);
}

void handle_pwd(CommList* list, Command* comm, sConfig* conf)
{
    (void) list; (void) comm;
    printf("%s\n", conf->cwd);
}

void handle_export(CommList* list, Command* comm, sConfig* conf)
{
    (void) list;
    
    for (int i = 1; i < comm->argCount; i++)
    {
        // Bash exports until it hits an error.
        // It doesn't do complete verification first.
        if (!isValidVar(comm->args[i]))
        {
            setConfigExitCode(conf, GEN_ERROR);
            reportError("Argument Error", "Token '%s' is not a valid identifier.",
                comm->args[i]);
            return;
        }
        if ((i != comm->argCount - 1) && (!strcmp(comm->args[i + 1], "=")))
        {
            setEnvVar(conf->env, comm->args[i], comm->args[i + 2]);
            i += 2;
        }
        else
            setEnvVar(conf->env, comm->args[i], NULL);
    }
}

void handle_unset(CommList* list, Command* comm, sConfig* conf)
{
    (void) list;
    
    for (int i = 1; i < comm->argCount; i++)
        removeEnvVar(conf->env, comm->args[i]);
}

void handle_env(CommList* list, Command* comm, sConfig* conf)
{
    (void) list;
    
    if (comm->argCount > 1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Command Error", "Too many arguments for command '%s'.",
            comm->name);
        return;
    }
    
    if (conf->env != NULL)
    {
        envArray* arr = conf->env;
        for (size_t i = 0; i < arr->count; i++)
        {
            char* envVar = arr->envVars[i];
            char* envVal = arr->envValues[i];
            if (envVar == NULL)
                continue;
            if (envVal != NULL)
                printf("%s=%s\n", envVar, envVal);
            else
                printf("%s\n", envVar);
        }
    }
}

static bool isValidNumber(const char* numStr)
{
    if ((*numStr == '+') || (*numStr == '-'))
    {
        if (isdigit(numStr[1]))
            numStr++;
        else
            return false;
    }
    while (*numStr != '\0')
    {
        if (isdigit(*numStr))
        {
            numStr++;
            continue;
        }
        else
            return false;
    }

    return true;
}

void handle_exit(CommList* list, Command* comm, sConfig* conf)
{
    resetTerminal();
    rl_clear_history();
    if (comm->argCount == 1)
    {
        freeCommList(&list);
        freeConfig(&conf);
        exit(0);
    }
    else if (comm->argCount == 2)
    {
        int exitCode = 0;
        // Check if it's a number.
        if (!isValidNumber(comm->args[1]))
        {
            reportError("Argument Error", "Numeric argument required for command 'exit'.");
            exitCode = INVALID_EXIT;
        }

        if (exitCode == 0)
        {
            exitCode = atoi(comm->args[1]);
            if (exitCode < 0)
            {
                while (exitCode < 0)
                    exitCode += 256;
            }
            else if (exitCode > 255)
                exitCode = exitCode % 256;
        }
        freeCommList(&list);
        freeConfig(&conf);
        exit(exitCode);
    }
    else
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Argument Error", "Too many arguments to command 'exit'.");
    }
}