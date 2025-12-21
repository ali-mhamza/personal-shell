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

    if (comm->argCount > 1) // First argument is the command itself.
    {
        if (!strcmp(comm->args[1], "-n"))
        {
            noNewLine = true;
            index++;
        }
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

    char* homePath = expandEnv(conf, "HOME", NULL, NULL);
    int ret;
    if (comm->argCount == 1)
        ret = chdir(homePath);
    else
    {
        if ((strlen(comm->args[1]) == 1)
            && !strncmp(comm->args[1], "~", 1))
                ret = chdir(homePath);
        else
            ret = chdir(comm->args[1]);
    }

    free(homePath);

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
    (void) list;
    
    if (comm->argCount > 1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Command Error", "Too many arguments for command '%s'.",
            comm->name);
        return;
    }

    printf("%s\n", conf->cwd);
}

void handle_export(CommList* list, Command* comm, sConfig* conf)
{
    (void) list;
    
    for (int i = 1; i < comm->argCount; i++)
    {
        if ((i != comm->argCount - 1) && (!strcmp(comm->args[i + 1], "=")))
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
    freeConfig(&conf);
    rl_clear_history();
    if (comm->argCount == 1)
    {
        freeCommList(&list);
        exit(0);
    }
    else if (comm->argCount > 1)
    {
        unsigned char exitCode = 0;
        // Check if it's a number.
        if (!isValidNumber(comm->args[1]))
            exitCode = INVALID_EXIT;

        if (exitCode == 0)
        {
            int code = atoi(comm->args[1]);
            if ((code < 0) || (code > 255))
                exitCode = EXIT_RANGE;
            else
                exitCode = (unsigned char) code;
        }
        freeCommList(&list);
        exit(exitCode);
    }
}