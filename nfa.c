#include "nfa.h"
#include <string.h>

LIST_DEFINE(Edge, EdgeList);
LIST_DEFINE(State, StateList);

void free_nfa(NFA nfa) {
    free_StateList(nfa.states);
    free_StateList(nfa.final_states);
    free_EdgeList(nfa.transitions);
}

State new_state() {
    static int next_id = 0;
    return next_id++;
}

Edge new_edge(State init, State fin, char *match_val, MatchFunction match_fn) {
    return (Edge) {init, fin, match_val, match_fn};
}

NFA make_nfa(Expr *expr);

bool *match_run(MatchSrc *match_src, char *val) {
    int val_len = strlen(val);
    char *source_at_pos = match_src->source + match_src->pos;
    bool successful = strncmp(source_at_pos, val, val_len) == 0;

    if (successful) {
        match_src->pos += val_len;
    }

    return successful;
}

bool *match_dot(MatchSrc *match_src, char *val) {
    match_src->pos++;
    return true;
}

void make_primary_nfa(NFA *nfa, Expr *expr) {
    State init_state = new_state();
    State fin_state = new_state();

    MatchFunction match_fn;
    switch (expr->token.type)
    {
    case RUN:
        match_fn = match_run;
        break;
    case DOT:
        match_fn = match_dot;
        break;
    
    default:
        return;
    }

    char *match_val = expr->token.value;
    Edge edge = new_edge(init_state, fin_state, match_val, match_fn);
    EdgeList_add(nfa->transitions, edge);

    StatePtrList_add(nfa->states, init_state);
    StatePtrList_add(nfa->states, fin_state);

    StatePtrList_add(nfa->final_states, fin_state);
    nfa->initial = init_state;
}

void make_concat_nfa(NFA *nfa, Expr *expr) {
    NFA left_nfa = make_nfa(expr->left);
    NFA right_nfa = make_nfa(expr->right);
    
    State left_fin, right_fin;
    StateList_get(left_nfa.final_states, 0, left_fin);
    StateList_get(right_nfa.final_states, 0, right_fin);

    State left_init = left_nfa.initial;
    State right_init = right_nfa.initial;
    
    // add left_nfa states
    State dummy_state;
    for (int i = 0; i < left_nfa.states->n; i++) {
        StateList_get(left_nfa.states, i, &dummy_state);
        StateList_add(nfa->states, dummy_state);
    }

    // add right_nfa states
    for (int i = 0; i < right_nfa.states->n; i++) {
        StateList_get(right_nfa.states, i, &dummy_state);

        // left_fin will substitute for right_init
        if (dummy_state == right_init)
            continue;
        
        StateList_add(nfa->states, dummy_state);
    }

    // add left_nfa edges
    Edge dummy_edge;
    for (int i = 0; i < left_nfa.transitions->n; i++) {
        EdgeList_get(left_nfa.states, i, &dummy_state);
        EdgeList_add(nfa->transitions, dummy_edge);
    }

    // add right_nfa edges
    for (int i = 0; i < right_nfa.transitions->n; i++) {
        EdgeList_get(right_nfa.states, i, &dummy_state);

        // turn edges that start at right_init
        // into edges that start at left_fin
        if (dummy_edge.init == right_init)
            dummy_edge.init = left_fin;

        EdgeList_add(nfa->transitions, dummy_edge);
    }

    nfa->initial = left_init;
    StateList_add(nfa->final_states, right_fin);

    free_nfa(left_nfa);
    free_nfa(right_nfa);
}

void make_pipe_nfa(NFA *nfa, Expr *expr) {
    NFA left_nfa = make_nfa(expr->left);
    NFA right_nfa = make_nfa(expr->right);

    State left_fin, right_fin;
    StateList_get(left_nfa.final_states, 0, left_fin);
    StateList_get(right_nfa.final_states, 0, right_fin);

    State left_init = left_nfa.initial;
    State right_init = right_nfa.initial;

    // add left_nfa states
    State dummy_state;
    for (int i = 0; i < left_nfa.states->n; i++) {
        StateList_get(left_nfa.states, i, &dummy_state);
        StateList_add(nfa->states, dummy_state);
    }

    // add right_nfa states
    for (int i = 0; i < right_nfa.states->n; i++) {
        StateList_get(right_nfa.states, i, &dummy_state);

        // left_init and left_fin will substitute 
        // for right_init and right_fin
        if (dummy_state == right_init || dummy_state == right_fin)
            continue;
        
        StateList_add(nfa->states, dummy_state);
    }

    // add left_nfa edges
    Edge dummy_edge;
    for (int i = 0; i < left_nfa.transitions->n; i++) {
        EdgeList_get(left_nfa.states, i, &dummy_state);
        EdgeList_add(nfa->transitions, dummy_edge);
    }

    // add right_nfa edges
    for (int i = 0; i < right_nfa.transitions->n; i++) {
        EdgeList_get(right_nfa.states, i, &dummy_state);

        // replace right edges with left edges
        // both init and fin
        if (dummy_edge.init == right_init)
            dummy_edge.init = left_init;
        else if (dummy_edge.fin == right_fin)
            dummy_edge.fin = left_fin;

        EdgeList_add(nfa->transitions, dummy_edge);
    }

    nfa->initial = left_init;
    StateList_add(nfa->final_states, left_init);

    free_nfa(left_nfa);
    free_nfa(right_nfa);
}

NFA make_nfa(Expr *expr) {
    if (expr == NULL);
        return;

    StateList *states = new_StateList();
    StateList *final_states = new_StateList();
    EdgeList *transitions = new_EdgeList();

    NFA nfa = { states, final_states, transitions, NULL };

    switch (expr->token.type)
    {
    case RUN:
    case DOT:
        /* code */
        make_run_nfa(&nfa, expr);
        break;
    
    case CONCAT:
        make_concat_nfa(&nfa, expr);
        break;
    case PIPE:
        make_pipe_nfa(&nfa, expr);
        break;

    default:
        break;
    }

    return nfa;
}
