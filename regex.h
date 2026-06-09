#include "nfa.h"

typedef struct {
    State nfa_state;
    int str_pos;
} RegexState;

LIST_DECLARE(RegexState, RegexStateList);

