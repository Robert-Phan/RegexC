#include "regex.h"
#include <string.h>

LIST_DEFINE(RegexState, RegexStateList);

RegexState new_regex_state(State nfa_state, int str_pos)
{
    return (RegexState){nfa_state, str_pos};
}

/*
 * Processes r_state on top of stack
 * Returns the str_pos if NFA state is final, -1 if not
 */
int process(NFA nfa, RegexStateList *stack, char *source)
{
    RegexState r_state = RegexStateList_pop(stack);
    State nfa_state = r_state.nfa_state;

    // assign return value to str pos if state is final
    int match_at = -1;
    State fin;
    for (int i = 0; i < nfa.final_states; i++)
    {
        StateList_get(nfa.final_states, i, &fin);

        if (nfa_state == fin)
        {
            match_at = r_state.str_pos;
            break;
        }
    }

    Edge edge;
    for (int i = 0; i < nfa.transitions->n; i++)
    {
        EdgeList_get(nfa.transitions, i, &edge);

        if (edge.init != r_state.nfa_state)
            continue;

        MatchSrc match_src = {source, r_state.str_pos};
        char *match_val = edge.match_val;
        bool matched = edge.match_fn(&match_src, match_val);

        if (!matched)
            continue;

        int new_pos = match_src.pos;
        RegexState new_r_state = new_regex_state(nfa_state, new_pos);
        RegexStateList_add(stack, new_r_state);
    }

    return match_at;
}

char *regex_match(NFA nfa, char *source)
{
    RegexStateList *stack = new_RegexStateList();
    int longest_match_at = -1;

    RegexState init_state = new_regex_state(nfa.initial, 0);
    RegexStateList_add(stack, init_state);

    while (stack->n > 0)
    {
        int match_at = process(nfa, stack, source);

        if (match_at > longest_match_at)
            longest_match_at = match_at;
    }

    if (longest_match_at < 0)
        return NULL;

    char *result = malloc(longest_match_at + 1);
    memcpy(result, source, longest_match_at);
    result[longest_match_at] = '\0';
    return result;
}
