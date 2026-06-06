#include "nfa.h"

LIST_DEFINE(Edge, EdgeList);
LIST_DEFINE(State *, StatePtrList);

NFA make_nfa(Expr *tree) {
    StatePtrList *states = new_StatePtrList();
    StatePtrList *final_states = new_StatePtrList();

    NFA nfa = { states, final_states };
}
