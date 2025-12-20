#include "../include/comm_handlers.h"
#include "../include/common.h"
#include "../include/config.h"
#include "../include/error.h"
#include "../include/sighandle.h"
#include <errno.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void handle_echo(TokenObj* tokens, sConfig* conf)
{
    bool noNewLine = false;
    size_t index = 1;
    
    (void) conf;

    if (tokens->tokTypes[index] == T_OPTION)
    {
        if (!strcmp(tokens->tokStrs[index], "-n"))
            noNewLine = true;
        index++;
    }

    for (; index < tokens->count; index++)
    {
        printf("%s%s", tokens->tokStrs[index],
            (index == tokens->count - 1 ? "" : " "));
    }
    printf("%s", noNewLine ? "" : "\n");
}

void handle_cd(TokenObj* tokens, sConfig* conf)
{
    if (tokens->count > 2)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Command Error", "Too many arguments for command '%s'.",
            tokens->tokStrs[0]);
        return;
    }

    char* homePath = expandEnv(conf, "HOME", NULL, NULL);
    int ret;
    if (tokens->count == 1)
        ret = chdir(homePath);
    else
    {
        if ((strlen(tokens->tokStrs[1]) == 1)
            && !strncmp(tokens->tokStrs[1], "~", 1))
                ret = chdir(homePath);
        else
            ret = chdir(tokens->tokStrs[1]);
    }
    if (ret == -1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Argument Error", "%s.", strerror(errno));
        return;
    }

    free(homePath);
    resetConfigCWD(conf);
}

void handle_pwd(TokenObj* tokens, sConfig* conf)
{
    if (tokens->count > 1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Command Error", "Too many arguments for command '%s'.",
            tokens->tokStrs[0]);
        return;
    }

    printf("%s\n", conf->cwd);
}

void handle_export(TokenObj* tokens, sConfig* conf)
{
    for (size_t i = 1; i < tokens->count; i++)
    {
        if ((i != tokens->count - 1) && (tokens->tokTypes[i + 1] == T_EQUAL))
        {
            // Bash exports until it hits an error.
            // It doesn't do complete verification first.
            if (!isValidVar(tokens->tokStrs[i]))
            {
                setConfigExitCode(conf, GEN_ERROR);
                reportError("Argument Error", "Token '%s' is not a valid identifier.",
                    tokens->tokStrs[i]);
                return;
            }
            setEnvVar(conf->env, tokens->tokStrs[i], tokens->tokStrs[i + 2]);
            i += 2;
        }
        else
            setEnvVar(conf->env, tokens->tokStrs[i], NULL);
    }
}

void handle_unset(TokenObj* tokens, sConfig* conf)
{
    for (size_t i = 1; i < tokens->count; i++)
        removeEnvVar(conf->env, tokens->tokStrs[i]);
}

void handle_env(TokenObj* tokens, sConfig* conf)
{
    if (tokens->count > 1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Command Error", "Too many arguments for command '%s'.",
            tokens->tokStrs[0]);
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

void handle_exit(TokenObj* tokens, sConfig* conf)
{
    resetTerminal();
    freeConfig(&conf);
    rl_clear_history();
    if (tokens->count == 1)
    {
        freeTokenObj(&tokens);
        exit(0);
    }
    else if (tokens->count > 1)
    {
        unsigned char exitCode = 0;
        // Check if it's a number.
        if (!isValidNumber(tokens->tokStrs[1]))
            exitCode = INVALID_EXIT;

        if (exitCode == 0)
        {
            int code = atoi(tokens->tokStrs[1]);
            if ((code < 0) || (code > 255))
                exitCode = EXIT_RANGE;
            else
                exitCode = (unsigned char) code;
        }
        freeTokenObj(&tokens);
        exit(exitCode);
    }
}