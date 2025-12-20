#include "../include/lexer.h"
#include "../include/common.h"
#include "../include/error.h"
#include <stdbool.h>
#include <stddef.h>

static void skipWhitespace(char* line, size_t *index)
{
    while (isspace(line[*index]))
        (*index)++;
}

static void addRedirect(TokenObj* obj, char* line, size_t* index)
{
    size_t size = strlen(line);
    
    if (line[*index] == '>')
    {
        if ((*index != size - 1) && (line[*index + 1] == '>'))
        {
            addToken(obj, &line[*index], 2, T_RE_DR);
            (*index)++;
        }
        else
            addToken(obj, &line[*index], 1, T_RE_R);
    }
    else if (line[*index] == '<')
    {
        if ((*index != size - 1) && (line[*index + 1] == '<'))
        {
            addToken(obj, &line[*index], 2, T_RE_DL);
            (*index)++;
        }
        else
            addToken(obj, &line[*index], 1, T_RE_L);
    }
    (*index)++;
}

static void addSingleString(TokenObj* obj, char* line, size_t* index)
{
    size_t size = 0;
    while (line[size] != '\0')
    {
        (*index)++;
        if (line[size] == '\'')
            break;
        size++;
    }

    addToken(obj, line, size, T_STR);
}

static char* expandInPlace(sConfig* conf, char* line, size_t *size)
{
    char* final = line;
    char* temp;
    while (((temp = strchr(final, '$')) != NULL)
            && (temp < final + *size))
    {
        // Idea:
        // Split quote around variable: [Welcome, ][$var1][, to this house.]
        // Replace variable: [Welcome, ][Malcolm][, to this house.]
        // Join back together: Welcome, Malcom, to this house.
        
        // We should probably add some kind of strReplace function
        // to do this properly and independently.

        size_t index = temp - final;
        char* first = strndup(final, temp - final);
        char* insert = expandEnv(conf, temp + 1, size, &index);
        char* part = strjoin(first, insert);
        final = strjoin(part, final + index);
        free(first);
        free(insert);
        free(part);
    }

    return final;
}

// Temporarily: no expansion of $ variables.
static void addDoubleString(sConfig* conf, TokenObj* obj, char* line, size_t* index)
{    
    size_t size = 0;
    while (line[size] != '\0')
    {
        (*index)++;
        if (line[size] == '"')
            break;
        size++;
    }

    char* final = expandInPlace(conf, line, &size);
    addToken(obj, final, size, T_STR);
    if (final != line) // TokenObj does not own the string.
        free(final);
}

static void addWordToken(sConfig* conf, TokenObj* obj, char* line, size_t* index)
{    
    const char* commands[] = {
        "echo", "cd", "pwd", "export",
        "unset", "env", "exit", NULL
    };

    size_t size = 0;
    while (isWordChar(line[size]))
    {
        (*index)++;
        size++;
    }

    char* final = expandInPlace(conf, line, &size);

    for (int i = 0; commands[i] != NULL; i++)
    {
        if (strlen(commands[i]) != size)
            continue;
        if (!strncmp(final, commands[i], size))
        {
            addToken(obj, final, size, (TokType) i);
            if (final != line)
                free(final);
            return;
        }
    }

    addToken(obj, final, size, T_WORD);
    if (final != line)
        free(final);
}

TokenObj* getTokens(sConfig* conf, char* line)
{
    TokenObj* obj = initTokenObj();
    if (obj == NULL)
        return NULL;

    size_t size = strlen(line);
    for (size_t i = 0; i < size;)
    {
        skipWhitespace(line, &i);
        switch (line[i])
        {
            case '>':
            case '<':   addRedirect(obj, line, &i);             break;
            case '|':   addToken(obj, &line[i++], 1, T_PIPE);   break;
            case '=':   addToken(obj, &line[i++], 1, T_EQUAL);  break;
            case '\'':  addSingleString(obj, &line[++i], &i);   break;
            case '"':   addDoubleString(conf, obj, &line[++i], &i);   break;
            case '-':
            {
                if (isalpha(line[i + 1]))
                {
                    addToken(obj, &line[i], 2, T_OPTION);
                    i += 2;
                }
                else
                    addWordToken(conf, obj, &line[i], &i);
                break;
            }
            default:
            {
                if (isWordChar(line[i]))
                    addWordToken(conf, obj, &line[i], &i);
                else
                {
                    setConfigExitCode(conf, GEN_ERROR);
                    reportError("Token Error", "Unrecognized token '%c'.", line[i]);
                    // Must free and return NULL here to signal an 
                    // error in main().
                    freeTokenObj(&obj);
                    return NULL;
                }
            }
        }
    }

    addToken(obj, NULL, 0, T_NULL);
    return obj;
}