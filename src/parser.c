#include "../include/parser.h"
#include "../include/common.h"
#include "../include/error.h"
#include "../include/sighandle.h"
#include "../include/strbuf.h"
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "get_next_line.h"

/* Commands. */

static Command* initCommand()
{
    Command* comm = malloc(sizeof(Command));
    if (comm != NULL)
    {
        comm->commType = T_NULL;
        comm->name = NULL;
        comm->args = NULL;
        comm->heredoc = NULL;
        comm->argCount = 0;
        comm->argCapacity = 0;
        comm->redirectIn = -1;
        comm->redirectOut = -1;
    }

    return comm;
}

static void reallocCommArgs(Command* comm, int newSize)
{
    char** newArgs = calloc(newSize, sizeof(char*));
    if (!newArgs)
        return;

    for (int i = 0; i < comm->argCount; i++)
        newArgs[i] = comm->args[i]; // We take ownership of the argument strings.

    free(comm->args);
    comm->args = newArgs;
    comm->argCapacity = newSize;
}

static void addCommandArg(Command* comm, char* arg)
{
    // Extra +1 for NULL terminator so that we can safely
    // pass this array to execve.
    while (comm->argCapacity < comm->argCount + 2)
    {
        reallocCommArgs(comm, comm->argCapacity == 0 ?
            8 : comm->argCapacity * 2);
    }

    comm->args[comm->argCount++] = strdup(arg); // We do not take ownership of the passed string.
}

static void freeCommand(Command** comm)
{
    if (comm == NULL)
        return;
    free((*comm)->name);
    for (int i = 0; i < (*comm)->argCount; i++)
        free((*comm)->args[i]);
    free((*comm)->args);
    free((*comm)->heredoc);
    free(*comm);
    *comm = NULL;
}

/* Command lists. */

static CommList* initCommList()
{
    CommList* list = malloc(sizeof(CommList));
    if (list != NULL)
    {
        list->comms = NULL;
        list->count = 0;
        list->capacity = 0;
    }

    return list;
}

void freeCommList(CommList** list)
{
    if (list == NULL)
        return;
    for (size_t i = 0; i < (*list)->count; i++)
        freeCommand(&((*list)->comms[i]));
    free((*list)->comms);
    free(*list);
    *list = NULL;
}

static void reallocCommList(CommList* list, size_t newSize)
{
    Command** newComms = calloc(newSize, sizeof(Command*));
    if (!newComms)
        return;

    for (size_t i = 0; i < list->count; i++)
        newComms[i] = list->comms[i]; // We take ownership of the Command* objects.

    free(list->comms);
    list->comms = newComms;
    list->capacity = newSize;
}

static void addCommand(CommList* list, Command* comm)
{
    while (list->capacity < list->count + 1)
        reallocCommList(list, list->capacity == 0 ?
            8 : list->capacity * 2);

    list->comms[list->count++] = comm; // We take ownership of the Command* object.
}

// Returns NULL on error.
// Otherwise returns the heredoc body as
// a heap-allocated string.
static char* consumeHereDocBody(sConfig* conf, char* delim, bool quoteDelim)
{
    strbuf* buf = initBuf();
    if (buf == NULL)
        return NULL;
    
    while (true)
    {
        write(1, "> ", 2);
        gSignal = SIG_ATOMIC_MIN;
        char *line = get_next_line(0);
        if (gSignal == SIGINT)
        {
            freeBuf(&buf, FREE_CHARS);
            free(line);
            return NULL;
        }
        
        gSignal = 0;
        if (line == NULL) // Assuming for simplicity this is not due to an error.
        {
            setConfigExitCode(conf, GEN_ERROR);
            reportError("Input Error",
                "Input to here document was not terminated.");
            freeBuf(&buf, FREE_CHARS);
            return NULL;
        }
        
        char* stop;
        if ((stop = strstr(line, delim)) != NULL)
        {
            appendBuf(buf, line, stop - line);
            free(line);
            break;
        }
        else
        {
            appendBuf(buf, line, -1);
            free(line);
        }
    }

    size_t size = buf->count;
    char* temp = freeBuf(&buf, NO_FREE_CHARS);
    char* body = temp;
    if (!quoteDelim)
        body = expandInPlace(conf, temp, &size);
    
    if (body != temp)
        free(temp);
    return body;
}

// Start points to the position of the heredoc token.
// Returns false on error; otherwise returns true.
static bool parseHereDoc(sConfig* conf, Command* comm,
    TokenObj* tokens, size_t* start)
{
    if (!IS_DELIM(tokens->tokTypes[*start + 1]))
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Syntax Error",
            "Expect delimiter after heredoc ('<<') token.");
        return false;
    }
    else if ((tokens->tokTypes[*start + 2] != T_NULL)
            && !IS_SYMBOL(tokens->tokTypes[*start + 2]))
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Syntax Error",
            "Delimiter can only be a single token.");
        return false;
    }

    char* newHereDoc = consumeHereDocBody(conf, tokens->tokStrs[*start + 1],
        (tokens->tokTypes[*start] == T_STR));
    if (newHereDoc == NULL)
        return false;

    if (comm->heredoc != NULL)
        free(comm->heredoc);
    comm->heredoc = newHereDoc;
    return true;
}

// Returns the associated file descriptor on success.
// Returns -1 on error.
static int parseRedirect(sConfig* conf, Command* comm,
    TokenObj* tokens, size_t* start)
{
    TokType redirect = tokens->tokTypes[(*start)++];
    if (tokens->tokTypes[*start] == T_NULL)
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Syntax Error",
            "Expect file name after redirect token.");
        return -1;
    }

    const char* path = tokens->tokStrs[(*start)];
    int flags;
    if (redirect == T_RE_R)
    {
        flags = O_WRONLY | O_CREAT;
        if (comm->redirectIn != -1)
            close(comm->redirectIn);
    }
    else if (redirect == T_RE_DR)
    {
        flags = O_WRONLY | O_CREAT | O_APPEND;
        if (comm->redirectIn != -1)
            close(comm->redirectIn);
    }
    else if (redirect == T_RE_L)
    {
        flags = O_RDONLY;
        if (comm->redirectOut != -1)
            close(comm->redirectOut);
    }
    int fd = open(path, flags);
    if (fd != -1)
        IN_REDIRECT(redirect) ? (comm->redirectIn = fd) :
            (comm->redirectOut = fd);
    else
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Argument Error", "File not found or could not be opened.");
    }
    return fd;
}

// Returns false on syntax error.
static bool parseNewCommand(sConfig* conf, CommList* list,
    TokenObj* tokens, size_t* start)
{
    if (!IS_COMMAND(tokens->tokTypes[*start])
        && (tokens->tokTypes[*start] != T_WORD))
    {
        setConfigExitCode(conf, GEN_ERROR);
        reportError("Syntax Error", "Unexpected token '%s'.",
            tokens->tokStrs[*start]);
        return false;
    }

    Command* comm = initCommand();
    if (!comm)
        return false;
    comm->commType = tokens->tokTypes[*start];
    comm->name = strdup(tokens->tokStrs[(*start)]);
    while (*start < tokens->count) // Stops at T_NULL.
    {
        if (tokens->tokTypes[*start] == T_PIPE)
        {
            (*start)++;
            break;
        }
        else if (IS_REDIRECT(tokens->tokTypes[*start]))
        {
            if (parseRedirect(conf, comm, tokens, start) == -1)
                return false;
        }
        else if (tokens->tokTypes[*start] == T_HEREDOC)
        {
            if (!parseHereDoc(conf, comm, tokens, start))
                return false;
        }
        else
            addCommandArg(comm, tokens->tokStrs[*start]); // The command is its own first argument.
        (*start)++;
    }

    addCommand(list, comm);
    return true;
}

CommList* parseCommands(sConfig* conf, TokenObj* tokens)
{
    CommList* list = initCommList();
    if (!list)
        return NULL;
    for (size_t start = 0; start < tokens->count;)
    {
        if (parseNewCommand(conf, list, tokens, &start))
            continue;
        freeCommList(&list);
        return NULL;
    }

    return list;
}