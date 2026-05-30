#include "scanner.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    /* code */
    TokenList *list = scan("[abc]ef|(ghi*)\\jklm{2,}");

    Token test;
    for (int i = 0; i < list->n; i++) {
        list_get(list, i, &test);
        printf("%x %s\n", test.type, test.value);
    }

    return 0;
}
