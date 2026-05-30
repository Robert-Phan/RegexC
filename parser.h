#include "scanner.h"

typedef struct {
    int current;
    TokenList *tokens;
} Parser;

typedef struct expr Expr;

struct expr {
    Token token;
    Expr *left;
    Expr *right;
};

Expr *parse(TokenList *tokens);

void free_expr(Expr *expr);

char *expr_str(Expr *expr);