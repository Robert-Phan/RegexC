#include "parser.h"
#include <stdbool.h>

typedef struct 
{
    TokenList *tokens;
    Expr *tree;
} Regex;

Regex compile_regex(char *pattern);

char *regex_match(Regex regex, char *source);

void free_regex(Regex regex);

typedef struct
{
    char *source;
    int pos;
} MatchSrc;

LIST_DECLARE(MatchSrc, SrcList);

/*
 * Takes a match_val and a struct
 * that represents a source string and its position
 * Returns the success of the match and
 * Modify the struct if successful
 */
typedef SrcList *(*MatchFunction)(Expr *, SrcList *);