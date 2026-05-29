#include "token.h"

Token new_token(TokenType type, char *value) {
    return (Token) {type, value};
}

TokenList *new_list() {
    int def_a_size = 10;

    TokenList *list = malloc(sizeof(TokenList));
    *list = (TokenList) {
        def_a_size, 0, calloc(def_a_size, sizeof(Token))
    };

    return list;
}

int list_get(TokenList *list, int i, Token *token) {
    if (i < 0 || i >= list->n)
        return 0;

    *token = list->array[i];
    return 1;
}

void free_list(TokenList *list) {
    for (int i = 0; i < list->n; i++) {
        free(list->array[i].value);
    }

    free(list->array);
    free(list);
}

void expand_list(TokenList *list) {
    if (list->n < (0.8f * list->a_size))
        return;

    size_t new_size = list->a_size * 2;

    Token *new_array =
        realloc(list->array, new_size * sizeof(Token));

    if (new_array == NULL) {
        // handle allocation failure
        return;
    }

    list->array = new_array;
    list->a_size = new_size;
}

void list_add(TokenList *list, Token token) {
    expand_list(list);
    
    list->array[list->n] = token;
    list->n++;
}