#include "_regex.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    // TokenList *list = scan("[^abc]\\def|ghi*|jk[Lm-o]+p{,6}");
    TokenList *list = scan("[^abc]def|ghi|jk[Lm-o]\\p");
    Expr* tree = parse(list);
    char *match = regex_match(tree, "jkop");
    // NFA nfa = make_nfa(tree);

    printf("%s\n", match);

    Token test;
    for (int i = 0; i < list->n; i++) {
        TokenList_get(list, i, &test);
        printf("%i %s\n", test.type, test.value);
    }

    return 0;
}
