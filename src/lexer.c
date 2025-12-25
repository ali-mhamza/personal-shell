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
            addToken(obj, &line[*index], 2, T_HEREDOC);
            (*index)++;
        }
        else
            addToken(obj, &line[*index], 1, T_RE_L);
    }
    (*index)++;
}

static inline bool isDelim(char c)
{
    return (isspace(c) || (c == '=') || (c == '|')
            || (c == '>') || (c == '<') || (c == '\0'));
}

static int getWordType(char* str, size_t size)
{
    const char* commands[] = {
        "echo", "cd", "pwd", "export",
        "unset", "env", "exit", NULL
    };

    for (int i = 0; commands[i] != NULL; i++)
    {
        if (strlen(commands[i]) != size)
            continue;
        else if (!strncmp(commands[i], str, size))
        {
            return i;
        }
    }

    return -1;
}

static char* consumeSingleString(char* start)
{
    size_t count = 0;
    while ((start[count] != '\'') && (start[count] != '\0'))
        count++;
    return strndup(start, count);
}

static char* consumeDoubleString(sConfig* conf, char* start, size_t* size)
{
    size_t count = 0;
    while ((start[count] != '"') && (start[count] != '\0'))
        count++;
    char* temp = strndup(start, count);
    if (size != NULL)
        (*size) = count;
    char* final = expandInPlace(conf, temp, &count);
    free(temp);
    return final;
}

static char* expandVar(sConfig* conf, char* start, size_t* size)
{
    if ((start[0] != '\0') && (start[0] == '?'))
    {
        if (size != NULL)
            (*size) = 1;
        return itoa(conf->exitCode);
    }
    else if (!isVarChar(start[0]))
    {
        if (size != NULL)
            (*size) = 0;
        return strdup("$");
    }
    
    size_t count = 0;
    while (!isDelim(start[count]) && (start[count] != '$'))
        count++;

    if (size != NULL)
        (*size) = count;

    char* temp = strndup(start, count);
    char* ret = getEnvVar(conf->env, temp);
    free(temp);
    if (ret != NULL)
        return ret;
    else
        return strdup("");
}

#include "../include/strbuf.h"
static void addWordToken(sConfig* conf, TokenObj* obj,
    char* start, size_t* index)
{
    strbuf* buf = initBuf();
    if (buf == NULL)
        return;

    size_t i;
    for (i = 0; !isDelim(start[i]);)
    {
        if (start[i] == '\'')
        {
            i++;
            char* temp = consumeSingleString(&start[i]);
            appendBuf(buf, temp, -1);
            i += strlen(temp) + 1;
            free(temp);
        }
        else if (start[i] == '"')
        {
            i++;
            size_t size = 0;
            char* temp = consumeDoubleString(conf, &start[i], &size);
            appendBuf(buf, temp, -1);
            i += size + 1;
            free(temp);
        }
        else if (start[i] == '$')
        {
            i++;
            size_t size = 0;
            char* temp = expandVar(conf, &start[i], &size);
            appendBuf(buf, temp, -1);
            i += size;
            free(temp);
        }
        else
        {
            appendBuf(buf, &start[i], 1);
            i++;
        }
    }

    size_t size = buf->count;
    char* tokStr = freeBuf(&buf, NO_FREE_CHARS);
    int type = getWordType(tokStr, size);
    addToken(obj, tokStr, size, type == -1 ? T_WORD : (TokType) type);
    (*index) += i;
    free(tokStr);
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
            case '<':   addRedirect(obj, line, &i);                 break;
            case '|':   addToken(obj, &line[i++], 1, T_PIPE);       break;
            case '=':   addToken(obj, &line[i++], 1, T_EQUAL);      break;
            case '-':
            {
                if (isalpha(line[i + 1]) && (i + 2 < size)
                    && isspace(line[i + 2]))
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
                else if (line[i] == '\0')
                    break;
                else
                {
                    setConfigExitCode(conf, GEN_ERROR);
                    reportError("Token Error", "Unrecognized token '%c'.", line[i]);
                    // Must free and return NULL here to signal an 
                    // error in execLine().
                    freeTokenObj(&obj);
                    return NULL;
                }
            }
        }
    }

    addToken(obj, NULL, 0, T_NULL);
    return obj;
}