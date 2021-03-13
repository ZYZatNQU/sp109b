#ifndef __LEXER_H__
#define __LEXER_H__

#include "util.h"

typedef enum { Id, Int, Keyword, Literal, Op } TokenType;

typedef struct{
    char *start;
    int len;
    TokenType Type;
}lexer_token;

extern char *typeName[5];

extern lexer_token lexer_next(char *string);
extern void lexDump(char *string);

#endif