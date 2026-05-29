#include "token.h"

typedef struct {
    int current;
    int start;
    char *source;
    TokenList *list;
} Scanner;

TokenList *scan(char *source);