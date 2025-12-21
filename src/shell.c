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
    else if (comm->commType == T_WORD)
        runExec(comm->name, comm->args, conf);
    // No other option since the parser would have raised an
    // error.
}

// Note: a redirect overrides a pipe.
// E.g., echo "test" > output.txt | cat
// will put "test" in output.txt and pipe
// nothing to cat.
// Thus, we should probably do any necessary piping
// first, so it can be overriden by a redirect if needed.

// Redirects (per command basis):
// 1. Save the stdin and stdout file descriptors.
// 2. Duplicate both to the stored FDs in the command object.
// 3. After running the command, duplicate back to the original FDs.

static void setUpFDs(Command* comm, int inputFD, int pipeFD[], bool pipeUsed)
{
    // Set up input file descriptor.
    
    int finalIn = (comm->redirectIn != -1 ? comm->redirectIn : inputFD);

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
    if ((inputFD != STDIN_FILENO) && (inputFD != finalIn))
        close(inputFD);
}

static void singleCommand(CommList* list, Command* comm, sConfig* conf)
{
    int stdinFD = dup(STDIN_FILENO);
    int stdoutFD = dup(STDOUT_FILENO);

    if (comm->redirectIn != -1)
        dup2(comm->redirectIn, STDIN_FILENO);
    if (comm->redirectOut != -1)
        dup2(comm->redirectOut, STDOUT_FILENO);

    runCommand(list, comm, conf);

    if (comm->redirectIn != -1)
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

static void mapCommands(CommList* list, sConfig* conf)
{
    if (list->count == 1) // No pipes.
    {
        singleCommand(list, list->comms[0], conf);
        return;
    }
    
    int inputFD = STDIN_FILENO;
    pid_t* processIDs = malloc(list->count);

    for (size_t i = 0; i < list->count; i++)
    {   
        int pipeFD[2];
        bool pipeUsed = false;
        Command* comm = list->comms[i];

        if ((i != list->count - 1) && (comm->redirectOut == -1))
        {
            pipe(pipeFD);
            pipeUsed = true;
        }

        pid_t id = fork();
        processIDs[i] = id;

		if (id == 0) // Child process logic.
		{
			setUpFDs(comm, inputFD, pipeFD, pipeUsed);
            runCommand(list, comm, conf);

            if (comm->redirectIn != -1)
                close(comm->redirectIn);
            if (comm->redirectOut != -1)
                close(comm->redirectOut);

            exit(0); // Temporarily.
		}
        else
        {
            if (pipeUsed)
                close(pipeFD[1]);
            if ((inputFD != STDIN_FILENO) && (inputFD != comm->redirectIn))
                close(inputFD);
            inputFD = (pipeUsed ? pipeFD[0] : STDIN_FILENO);
        }
    }

    int status;
    for (size_t i = 0; i < list->count; i++)
        waitpid(processIDs[i], &status, 0);
    free(processIDs);
    conf->exitCode = WEXITSTATUS(status);
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

// Returns true if successful (or if no here-document is found).
// Returns false on EOF error (i.e., delimiter was never reached).
// static bool handleHereDoc(TokenObj* tokens, char* line)
// {
    // 1. Check if there string '<<' exists in the line.
    // 2. If so, make sure you take its *last* occurrence.
    // If it doesn't, return true.
    // 3. Collect everything after the '<<' as the delimiter.
    // 4. Perform any expansion in place on the delimiter (maybe tokenize it?).
    // 5. Make a string buffer.
    // 6. Keep running readline() until the delimiter is found.
    // - Every time we read a line (that doesn't contain the
    // delimiter), append it to the buffer and manually add a newline
    // (we will have to update the lexer to properly handle newlines).
    // - If you tokenized the delimiter, compare tokens and check if
    // the delimiter tokens are *contained within* the input line tokens.
    // If so, remove any extra tokens after it (and cut off any text on
    // the last valid token if needed).
    // - If you didn't tokenize it, use strstr directly with the input
    // and delimiter. If it returns non-NULL, cut off everything from
    // the pointer it returns onwards and append only that much to the
    // buffer.
    // - If at any point readline() hits EOF (check the usual conditions),
    // report a warning and return false.
    // 7. Tokenize the current string in the buffer.
    // 8. Replace all the tokens before the *first* redirect (since we
    // don't use anything up to that as input) in the token object with
    // these tokens.
// }

static void checkEOF(sConfig* conf, char* line)
{
    if (line == NULL)
    {
        if ((errno == 0) && isatty(STDIN_FILENO)) // Hit EOF, not an error.
        {
            freeConfig(&conf);
            resetTerminal();
            rl_clear_history();
            exit(0);
        }
        else
        {
            setConfigExitCode(conf, GEN_ERROR);
            reportError("Internal Error", "Failed to read input.");
        }
    }
}

static void execLine(sConfig* conf, char* line)
{
    checkEOF(conf, line);

    if ((line != NULL) && (line[0] != '\0'))
    {
        add_history(line);
        TokenObj* tokens = getTokens(conf, line);
        if (tokens != NULL)
        {
            CommList* list = parseCommands(conf, tokens);
            freeTokenObj(&tokens);
            if (!list)
            {
                if (conf->exitCode == 0)
                {
                    setConfigExitCode(conf, GEN_ERROR);
                    reportError("Internal Error", "Failred to parse input.");
                }
                return;
            }
            mapCommands(list, conf);
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
