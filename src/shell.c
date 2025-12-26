#include "../include/common.h"
#include "../include/comm_handlers.h"
#include "../include/config.h"
#include "../include/error.h"
#include "../include/exec.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/sighandle.h"
#include "../include/strbuf.h"
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../get_next_line/get_next_line.h"

volatile sig_atomic_t gSignal = 0;

static char* makePrompt(sConfig* conf)
{
    strbuf* buf = initBuf();
    if (buf == NULL)
        return NULL;
    appendBuf(buf, "myshell:", -1);
    if (conf->cwd == NULL)
        resetConfigCWD(conf);
    appendBuf(buf, conf->cwd, -1);
    appendBuf(buf, "# ", -1);
    return freeBuf(&buf, NO_FREE_CHARS);
}

static void runCommand(CommList* list, Command* comm, sConfig* conf)
{
    conf->exitCode = 0; // Reset for next command.

    if (IS_COMMAND(comm->commType))
    {
        HandleFunc handlers[] = {
            handle_echo, handle_cd, handle_pwd,
            handle_export, handle_unset, handle_env,
            handle_exit
        };

        (*handlers[(int) comm->commType])(list, comm, conf);
    }
    else if ((comm->commType == T_WORD) || (comm->commType == T_STR))
        runExec(comm->name, comm->args, conf);
    // No other option since the parser would have raised an
    // error.
}

static void setUpFDs(Command* comm, int inputFD, int pipeFD[],
    int heredocFD[], bool pipeUsed)
{   
    // Set up input file descriptor.

    int finalIn;
    if (comm->heredoc != NULL)
        finalIn = heredocFD[0];
    else if (comm->redirectIn != -1)
        finalIn = comm->redirectIn;
    else
        finalIn = inputFD;

    if (finalIn != STDIN_FILENO)
        dup2(finalIn, STDIN_FILENO);

    // Set up output file descriptor.

    int finalOut;
    if (comm->redirectOut != -1)
        finalOut = comm->redirectOut;
    else if (pipeUsed)
        finalOut = pipeFD[1];
    else
        finalOut = STDOUT_FILENO;

    if (finalOut != STDOUT_FILENO)
        dup2(finalOut, STDOUT_FILENO);

    // Close unnecessary file descriptors.

    if (pipeUsed)
        close(pipeFD[0]);
    if (comm->heredoc != NULL)
        close(heredocFD[1]);
    if ((inputFD != STDIN_FILENO) && (inputFD != finalIn))
        close(inputFD);
}

static void singleCommand(CommList* list, Command* comm, sConfig* conf)
{
    int stdinFD = dup(STDIN_FILENO);
    int stdoutFD = dup(STDOUT_FILENO);

    // Set up heredocs and redirects (if any).

    int heredocFD[2];
    if (comm->heredoc != NULL)
    {
        pipe(heredocFD);
        dup2(heredocFD[0], STDIN_FILENO);
        write(heredocFD[1], comm->heredoc, strlen(comm->heredoc));
    }
    else if (comm->redirectIn != -1)
        dup2(comm->redirectIn, STDIN_FILENO);
    if (comm->redirectOut != -1)
        dup2(comm->redirectOut, STDOUT_FILENO);

    runCommand(list, comm, conf);

    // Close any used pipes or redirect files.

    if (comm->heredoc != NULL)
    {
        close(heredocFD[1]);
        dup2(stdinFD, STDIN_FILENO);
        close(heredocFD[0]);
    }
    else if (comm->redirectIn != -1)
    {
        dup2(stdinFD, STDIN_FILENO);
        close(comm->redirectIn);
    }
    if (comm->redirectOut != -1)
    {
        dup2(stdoutFD, STDOUT_FILENO);
        close(comm->redirectOut);
    }
}

static void setUpCommands(CommList* list, sConfig* conf)
{
    if (list->count == 0) // Empty input got through.
        return;
    
    if (list->count == 1) // No pipes.
    {
        Command* comm = list->comms[0];
        if (comm->failed)
        {
            setConfigExitCode(conf, GEN_ERROR);
            return;
        }
        if (IS_COMMAND(comm->commType))
        {
            singleCommand(list, list->comms[0], conf);
            return;
        }
    }
    
    int inputFD = STDIN_FILENO;
    pid_t* processIDs = malloc(list->count * sizeof(int));

    for (size_t i = 0; i < list->count; i++)
    {   
        Command* comm = list->comms[i];
        int pipeFD[2];
        int heredocFD[2];
        bool pipeUsed = false;

        if (comm->failed)
        {
            inputFD = STDIN_FILENO;
            continue;
        }

        // Set up pipelines for piping or heredocs.

        if (comm->heredoc != NULL)
            pipe(heredocFD);
        if ((i != list->count - 1) && (comm->redirectOut == -1))
        {
            pipe(pipeFD);
            pipeUsed = true;
        }

        pid_t id = fork();
        processIDs[i] = id;

		if (id == 0) // Child process logic.
		{
			setUpFDs(comm, inputFD, pipeFD, heredocFD, pipeUsed);
            runCommand(list, comm, conf);

            if (comm->redirectIn != -1)
                close(comm->redirectIn);
            if (comm->redirectOut != -1)
                close(comm->redirectOut);

            exit(conf->exitCode); // Temporarily.
		}
        else // Parent process logic.
        {
            // Close open pipes and files.
            
            if (comm->redirectIn != -1)
                close(comm->redirectIn);
            if (comm->redirectOut != -1)
                close(comm->redirectOut);
            
            if (pipeUsed)
                close(pipeFD[1]);

            if (comm->heredoc != NULL)
            {
                close(heredocFD[0]);
                write(heredocFD[1], comm->heredoc, strlen(comm->heredoc));
                close(heredocFD[1]);
            }

            if ((inputFD != STDIN_FILENO) && (inputFD != comm->redirectIn))
                close(inputFD);

            // Reassign inputFD for next command's read.
            inputFD = (pipeUsed ? pipeFD[0] : STDIN_FILENO);
        }
    }

    int status;
    for (size_t i = 0; i < list->count; i++)
    {
        if (!list->comms[i]->failed)
            waitpid(processIDs[i], &status, 0);
        else
            status = -1;
    }
    free(processIDs);
    conf->exitCode = (status != -1 ? WEXITSTATUS(status) : GEN_ERROR);
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

static void checkEOF(sConfig* conf, char* line)
{
    if (line == NULL)
    {
        freeConfig(&conf);
        if (errno == 0) // Hit EOF, not an error.
        {
            resetTerminal();
            rl_clear_history();
            write(1, "\n", 1);
            exit(EXIT_SUCCESS);
        }
        else if (isatty(STDIN_FILENO))
        {
            reportError("Internal Error", "Failed to read input.");
            exit(EXIT_FAILURE);
        }
        else // Finished executing piped input.
            exit(EXIT_SUCCESS);
    }
}

static void execLine(sConfig* conf, char* line)
{
    checkEOF(conf, line);

    if ((line != NULL) && (line[0] != '\0'))
    {
        if (isatty(STDIN_FILENO))
            add_history(line);
        TokenObj* tokens = getTokens(conf, line);
        if (tokens != NULL)
        {
            CommList* list = parseCommands(conf, tokens);
            freeTokenObj(&tokens);
            if (!list)
            {
                if ((conf->exitCode == 0) && (gSignal == 0))
                {
                    setConfigExitCode(conf, GEN_ERROR);
                    reportError("Internal Error", "Failed to parse input.");
                }
                return;
            }
            setUpCommands(list, conf);
            freeCommList(&list);
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
            freeConfig(&conf);
            exit(EXIT_FAILURE); // Fatal.
        }

        rl_variable_bind("enable-bracketed-paste", "off");

        char* line;
        if (isatty(STDIN_FILENO)/* && isatty(STDOUT_FILENO)*/)
            line = readline(prompt);
        else
        {
            line = get_next_line(STDIN_FILENO);
            size_t len = (line != NULL ? strlen(line) : 0);
            if ((len > 0) && (line[len - 1] == '\n'))
                line[len - 1] = '\0';
        }
        free(prompt);

        if (gSignal == SIGINT)
        {
            setConfigExitCode(conf, gSignal + 128);
            gSignal = 0;
        }

        execLine(conf, line);
        gSignal = 0; // In case it was changed in this call.
        free(line);
    }
}