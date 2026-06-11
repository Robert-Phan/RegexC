#include "_regex.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    // TokenList *list = scan("[^abc]\\def|ghi*|jk[Lm-o]+p{,6}");
    TokenList *list = scan("[^abc]+def|\\ghi*|jk[Lm-o]?p{2,6}");
    Expr* tree = parse(list);
    char *match = regex_match(tree, "01234defgdef");

    printf("%s\n", match == NULL ? "nothin" : match);

    return 0;
}
