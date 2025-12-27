#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum TokType {
    T_ECHO,
    T_CD,
    T_PWD,
    T_EXPORT,
    T_UNSET,
    T_ENV,
    T_EXIT,
    T_OPTION,   // -n, -b
    T_WORD,     // VAR
    T_EQUAL,    // =
    T_STR,      // "Hello" 'world!'
    T_PIPE,     // |
    T_RE_R,     // >
    T_RE_L,     // <
    T_RE_DR,    // >>
    T_HEREDOC,    // <<
    T_NULL
} TokType;

#define IS_COMMAND(type)    (((type) >= T_ECHO) && ((type) <= T_EXIT))
#define IS_REDIRECT(type)   (((type) >= T_RE_R) && ((type) <= T_RE_DR))
#define IS_START(type) \
    (IS_COMMAND(type) || (type == T_WORD) || (type == T_STR) || IS_REDIRECT(type))
#define IN_REDIRECT(type)   (((type) == T_RE_L))
#define OUT_REDIRECT(type)  (((type) == T_RE_R) || ((type) == T_RE_DR))
// For here-documents.
#define IS_DELIM(type) \
    (((type) == T_WORD) || ((type) == T_STR) || IS_COMMAND(type))
#define IS_SYMBOL(type) (((type) >= T_PIPE) && ((type) <= T_HEREDOC))

typedef struct TokenObj {
    char**      tokStrs;
    TokType*    tokTypes;
    size_t      count;
    size_t      capacity;
} TokenObj;

TokenObj*   initTokenObj();
void        freeTokenObj(TokenObj** obj);
void        addToken(TokenObj* obj, const char* text, size_t size, TokType type);

#endif