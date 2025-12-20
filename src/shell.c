#include "../include/common.h"
#include "../include/comm_handlers.h"
#include "../include/config.h"
#include "../include/error.h"
#include "../include/exec.h"
#include "../include/lexer.h"
#include "../include/sighandle.h"
#include "../include/strbuf.h"
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t gSignal = 0;

static char* makePrompt(sConfig* conf)
{
    strbuf* buf = initBuf();
    if (buf == NULL)
        return NULL;
    appendBuf(buf, "myshell:", -1);
    if (conf->cwd == NULL)
        resetConfigCWD(conf);
    appendBuf(buf, conf->cwd, -1); // Error occurs here.
    appendBuf(buf, "# ", -1);
    return freeBuf(&buf, NO_FREE_CHARS);
}

static void mapCommands(TokenObj* tokens, sConfig* conf)
{
    conf->exitCode = 0; // Reset for next command.
    
    if (IS_COMMAND(tokens->tokTypes[0]))
    {
        HandleFunc handlers[] = {
            handle_echo, handle_cd, handle_pwd,
            handle_export, handle_unset, handle_env,
            handle_exit
        };

        (*handlers[tokens->tokTypes[0]])(tokens, conf);
    }
    else if (tokens->tokTypes[0] == T_WORD)
        runExec(tokens->tokStrs[0], &tokens->tokStrs[0], conf);
    else
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Syntax Error", "Unexpected token '%s'.", tokens->tokStrs[0]);
    }
}

static void setUpHandler(sConfig* conf, int sig)
{
    struct sigaction sa;
    sa.sa_handler = (sig == SIGINT ? handleSigInt : handleSigQuit);
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if (sigaction(sig, &sa, NULL) == -1)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Internal Error", "Failed signal handling.");
        exit(1);
    }
}

static void execLine(sConfig* conf, char* line)
{
    if ((line == NULL) && (errno == 0) && isatty(STDIN_FILENO))
    {
        freeConfig(&conf);
        resetTerminal();
        rl_clear_history();
        exit(0);
    }
    if ((line != NULL) && (line[0] != '\0'))
    {
        add_history(line);
        TokenObj* tokens = getTokens(conf, line);
        if (tokens != NULL)
        {
            mapCommands(tokens, conf);
            freeTokenObj(&tokens);
        }
    }
}

int main(int argc, char* argv[], char* envp[])
{
    (void) argc; (void) argv;

    sConfig* conf = initConfig(envp);
    if (conf == NULL)
        exit(EXIT_FAILURE); // Fatal.
    setUpHandler(conf, SIGINT);
    setUpHandler(conf, SIGQUIT);

    resetTerminal();
    disableCtrlPrint();

    while (true)
    {           
        errno = 0;
        char* prompt = makePrompt(conf);
        if (prompt == NULL)
        {
            resetTerminal();
            rl_clear_history();
            exit(EXIT_FAILURE); // Fatal.
        }

        char* line = readline(prompt);
        free(prompt);

        if (gSignal == SIGINT)
        {
            setConfigExitCode(conf, gSignal + 128);
            gSignal = 0;
        }

        execLine(conf, line);
        free(line);
    }
}