#include <stdbool.h>
#include <stdarg.h>
#include "parser.h"

#define check_error(expr) \
    if ((expr) == NULL) \
        goto fail;

#define fail_state(expr) \
    fail: \
        free_expr(expr); \
        return NULL;

static Expr *new_expr(Token token) {
    Expr *expr = malloc(sizeof(Expr));

    *expr = (Expr) {token, NULL, NULL};

    return expr;
}

void free_expr(Expr *expr) {
    if (expr == NULL)
        return;
    
    free_expr(expr->left);
    free_expr(expr->right);
    free(expr);
}

static bool is_at_end(Parser *pr) {
    return pr->current >= pr->tokens->n;
}

static Token peek(Parser *pr) {
    Token token = (Token) {};
    list_get(pr->tokens, pr->current, &token);
    return token;
}

static Token previous(Parser *pr) {
    Token token = (Token) {};
    list_get(pr->tokens, pr->current-1, &token);
    return token;
}

static Token advance(Parser *pr) {
    if (!is_at_end(pr)) 
        pr->current++;

    return previous(pr);
}

static bool check(Parser *pr, TokenType type) {
    return peek(pr).type == type;
}

static bool match(Parser *pr, int count, ...) {
    if (is_at_end(pr)) 
        return false;
    
    va_list types;
    va_start(types, count);

    for (int i = 0; i < count; i++) {
        TokenType type = va_arg(types, TokenType);

        if (check(pr, type))  {
            pr->current++;
            return true;
        }
    }
    
    va_end(types);
    return false;
}

static Expr *consume(Parser *pr, TokenType type) {
    if (match(pr, 1, type)) 
        return new_expr(previous(pr));
    
    return NULL;
}

// static Expr *expression(Parser *pr);

// Expr *parse(TokenList *tokens) {
//     Parser parser = {0, tokens};

//     return expression(&parser);
// }
