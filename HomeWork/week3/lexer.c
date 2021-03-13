#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*printf("%02d:%-20s %-20s\n", i, tokens[i], typeName[types[i]]);
 *原程式
 *char *typeName[5] = {"Id", "Int", "Keyword", "Literal", "Op"};
 *char code[TMAX], *p;
 *char strTable[TMAX], *strTableEnd=strTable;
 *char *tokens[TMAX], tokenTop=0, tokenIdx=0, token[SMAX];
 *TokenType types[TMAX], type;
 *分析詞彙，內容複製到strTable中，tokens陣列儲存各內容起始地址，types陣列為各內容類型
 *其中types陣列長度為TMAX:100000但tokenTop為char類型由此可見可解析長度最大為127
 */

char *typeName[5] = {"Id", "Int", "Keyword", "Literal", "Op"};

lexer_token lexer_next(char *string){
    TokenType type;
    lexer_token token;
    char *p = string;
    char *start;
    while (isspace(*p))
        p++;
    start = p;
    // TokenType type;
    if (*p == '\0'){
        token.len = 0;
        return token;
    }
    if (*p == '"') {
        p++;
        while (*p != '"') p++;
        p++;
        type = Literal;
    } else if (*p >='0' && *p <='9') { // 數字
        while (*p >='0' && *p <='9') p++;
        type = Int;
    } else if (isalpha(*p) || *p == '_') { // 變數名稱或關鍵字
        while (isalpha(*p) || isdigit(*p) || *p == '_') p++;
        type = Id;
    } else if (strchr("+-*/%%&|<>!=", *p) >= 0) {
        char c = *p++;
        if (*p == '=') p++; // +=, ==, <=, !=, ....
        else if (strchr("+-&|", c) >= 0 && *p == c) p++; // ++, --, &&, ||
        type = Op;
    } else {
        p++;
        type = Op;
    }
    token.start = start;
    token.len = p-start;
    token.Type = type;
    return token;
}

void lexDump(char *string) {
    int i = 0;
    lexer_token token;
    char *p = string;
    char *str = NULL;
    printf("========== lexDump ==============\n");
    while (1) {
        token = lexer_next(p);
        if(token.len == 0)return;
        str = (char*)malloc(token.len+1);
        assert(str != NULL);
        memcpy(str,token.start,token.len);
        *(str + token.len) = '\0';
        printf("%02d:%-20s %-20s  %d\n", i, str, typeName[token.Type],token.len);
        free(str);
        p = token.start + token.len;
        i++;
    }
}