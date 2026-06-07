#include "parser.h"
#include <stdbool.h>

typedef struct Edge Edge;

LIST_DECLARE(Edge, EdgeList);

typedef int State;

State new_state();

typedef struct {
    char *source;
    int pos;
} MatchSrc;

/*
* Takes a match_val and a struct
* that represents a source string and its position
* Returns the success of the match and
* Modify the struct if successful
*/
typedef bool (*MatchFunction) (MatchSrc *, char *);

struct Edge {
    State init;
    State fin;
    char *match_val;
    MatchFunction match_fn;
};

Edge new_edge(State init, State fin, char *match_val, MatchFunction match_fn);

LIST_DECLARE(State, StateList);

typedef struct {
    StateList *states;
    StateList *final_states;
    EdgeList *transitions;
    State initial;
} NFA;
