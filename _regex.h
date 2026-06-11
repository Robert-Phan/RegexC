#include "parser.h"
#include <stdbool.h>

char *regex_match(Expr *expr, char *source);

typedef struct {
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
typedef SrcList *(*MatchFunction) (Expr *, SrcList *);