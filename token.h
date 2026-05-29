#include <stdlib.h>

typedef enum {
    ESCAPE,
    LEFT_PAREN,
    RIGHT_PAREN,
    PIPE,
    RUN,
    PLUS,
    QMARK,
    STAR,
    RANGE,
    SET,
    NEGSET,
    CARET,
    DOLLAR
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

Token new_token(TokenType type, char *value);

typedef struct {
    int a_size;
    int n;
    Token *array;
} TokenList;

TokenList *new_list();

int list_get(TokenList *list, int i, Token *token);

void list_add(TokenList *list, Token token);

void free_list(TokenList *list);