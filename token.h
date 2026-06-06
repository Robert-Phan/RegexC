#include <stdlib.h>
#include "list.h"

typedef enum {
    ESCAPE,
    LEFT_PAREN,
    RIGHT_PAREN,
    PIPE,
    RUN,
    CONCAT,
    PLUS,
    QMARK,
    STAR,
    RANGE,
    DOT,
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

LIST_DECLARE(Token, TokenList);
