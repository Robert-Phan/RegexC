#include "regex.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    // TokenList *list = scan("[^abc]\\def|ghi*|jk[Lm-o]+p{,6}");
    TokenList *list = scan("[^abc]def|ghi|jk[Lm-o]\\p");
    Expr* tree = parse(list);
    NFA nfa = make_nfa(tree);

    Token test;
    for (int i = 0; i < list->n; i++) {
        TokenList_get(list, i, &test);
        printf("%i %s\n", test.type, test.value);
    }

    return 0;
}
