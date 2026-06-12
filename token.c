#include "token.h"

Token new_token(TokenType type, char *value) {
    return (Token) {type, value};
}

LIST_DEFINE(Token, TokenList);

void free_token_strs(TokenList *list) {
    for (int i = 0; i < list->n; i++) {
        free(list->array[i].value);
    }
}