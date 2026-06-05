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

static Expr *expression(Parser *pr);

static Expr *primary(Parser *pr) {
    if (match(pr, 7, RUN, ESCAPE, DOT, SET, NEGSET, CARET, DOLLAR)) {
        return new_expr(previous(pr));
    }

    if (match(pr, 1, LEFT_PAREN)) {
        Expr *expr = expression(pr);
        check_error(consume(pr, RIGHT_PAREN))
        return expr;
    
        fail_state(expr)
    }

    return NULL;
}

static Expr *quant(Parser *pr) {
    Expr *sub = primary(pr);

    if (match(pr, 4, PLUS, QMARK, STAR, RANGE)) {
        Expr *expr = new_expr(previous(pr));
        check_error(sub)
        expr->left = sub;
        return expr;

        fail_state(expr)
    }

    return sub;
}

static bool is_concat_end(Parser *pr) {
    if (is_at_end(pr))
        return true;
    
    Token peeked = peek(pr);
    TokenType ends[] = {PIPE, RIGHT_PAREN};
    int n = 2;

    for (int i = 0; i < n; i++) {
        if (check(pr, ends[i]))
            return true;
    }

    return false;
}

static Expr *concat(Parser *pr) {
    Expr *left = quant(pr);

    if (!is_concat_end(pr)) {
        Token concat_token = { CONCAT, "" };
        Expr *expr = new_expr(concat_token);
        check_error(left)
        expr->left = left;

        Expr *right = concat(pr);
        check_error(right);
        expr->right = right;

        return expr;

        fail_state(expr)
    }

    return left;
}

static Expr *alternate(Parser *pr)  {
    Expr *left = concat(pr);

    if (match(pr, 1, PIPE)) {
        Expr *expr = new_expr(previous(pr));
        check_error(left)
        expr->left = left;

        Expr *right = alternate(pr);
        check_error(right);
        expr->right = right;

        return expr;

        fail_state(expr)
    }

    return left;
}

static Expr *expression(Parser *pr) {
    return alternate(pr);
}

Expr *parse(TokenList *tokens) {
    Parser parser = {0, tokens};

    return expression(&parser);
}
