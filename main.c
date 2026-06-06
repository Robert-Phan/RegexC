#include "regex.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    /* code */
    TokenList *list = scan("[abc]ef|(ghi*)\\jklm{2,}");
    Expr* tree = parse(list);

    Token test;
    for (int i = 0; i < list->n; i++) {
        TokenList_get(list, i, &test);
        printf("%i %s\n", test.type, test.value);
    }

    return 0;
}
