#include "parser.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Edge Edge;

LIST_DECLARE(Edge, EdgeList);

typedef struct {
    EdgeList *transitions;
} State;

typedef bool (*MatchFunction) (void);

struct Edge {
    State *in_state;
    MatchFunction match_fn;
};

LIST_DECLARE(State *, StatePtrList);

typedef struct {
    StatePtrList *states;
    StatePtrList *final_states;
    State *initial;
} NFA;
