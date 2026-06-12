#include "_regex.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    Regex regex = compile_regex("[^abc]+def|\\ghi*|jk[Lm-o]?p{2,6}");
    char *match = regex_match(regex, "ghiidefgdef");

    printf("%s\n", match == NULL ? "nothin" : match);

    free_regex(regex);
    free(match);

    return 0;
}
