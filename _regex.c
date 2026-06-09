#include "nfa.h"
#include <string.h>

bool match_dispatch(Expr *expr, MatchSrc *match_src);

bool match_run(Expr *expr, MatchSrc *match_src)
{
    char *val = expr->token.value;
    int val_len = strlen(val);
    char *source_at_pos = match_src->source + match_src->pos;
    bool successful = strncmp(source_at_pos, val, val_len) == 0;

    if (successful)
        match_src->pos += val_len;

    return successful;
}

bool match_dot(Expr *expr, MatchSrc *match_src)
{
    match_src->pos++;
    return true;
}

bool match_set(Expr *expr, MatchSrc *match_src)
{
    char *val = expr->token.value;
    int val_len = strlen(val);
    char srcchr = *(match_src->source + match_src->pos);

    for (int i = 0; i < val_len; i++)
    {
        // for matching character ranges
        bool has_range = val[i + 1] == '-' && i + 2 < val_len;
        if (has_range)
        {
            // end of char range is higher than start
            if (val[i + 2] < val[i])
                goto no_range_match;

            for (char j = val[i]; j <= val[i + 2]; j++)
                if (srcchr == j)
                {
                    match_src->pos++;
                    return true;
                }

        no_range_match:
            i += 2;
            continue;
        }

        if (srcchr == val[i])
        {
            match_src->pos++;
            return true;
        }
    }

    return false;
}

bool match_negset(Expr *expr, MatchSrc *match_src)
{
    char *val = expr->token.value;
    int val_len = strlen(val);
    char srcchr = *(match_src->source + match_src->pos);

    for (int i = 0; i < val_len; i++)
    {
        // for matching character ranges
        bool has_range = val[i + 1] == '-' && i + 2 < val_len;
        if (has_range)
        {
            // end of char range is higher than start
            if (val[i + 2] < val[i])
                goto no_range_match;

            for (char j = val[i]; j < val[i + 2]; j++)
                if (srcchr == j)
                    return false;

        no_range_match:
            i += 2;
            continue;
        }

        if (srcchr == val[i])
            return false;
    }

    match_src->pos++;
    return true;
}

bool match_primary(Expr *expr, MatchSrc *match_src)
{
    MatchSrc new_src = *match_src;
    bool result;

    switch (expr->token.type)
    {
    case RUN:
        result = match_run(expr, &new_src);
        break;
    case DOT:
        result = match_dot(expr, &new_src);
        break;
    case SET:
        result = match_set(expr, &new_src);
        break;
    case NEGSET:
        result = match_negset(expr, &new_src);
        break;

    default:
        break;
    }

    *match_src = new_src;

    return result;
}

bool match_concat(Expr *expr, MatchSrc *match_src)
{
    MatchSrc left_src = *match_src;
    bool result = match_dispatch(expr->left, &left_src);

    if (!result)
        return false;

    MatchSrc right_src = left_src;
    result = match_dispatch(expr->right, &right_src);

    if (result)
        *match_src = right_src;

    return result;
}

bool match_pipe(Expr *expr, MatchSrc *match_src)
{
    MatchSrc left_src = *match_src;
    MatchSrc right_src = *match_src;

    bool left_result = match_dispatch(expr->left, &left_src);
    bool right_result = match_dispatch(expr->right, &right_src);

    if (!left_result)
    {
        *match_src = right_src;

        return right_result;
    }

    *match_src = left_src;
    return left_result;
}

bool match_dispatch(Expr *expr, MatchSrc *match_src)
{
    MatchSrc new_src = *match_src;
    bool result;

    switch (expr->token.type)
    {
    case RUN:
    case DOT:
    case SET:
    case NEGSET:
        /* code */
        result = match_primary(expr, &new_src);
        break;

    case CONCAT:
        result = match_concat(expr, &new_src);
        break;
    case PIPE:
        result = match_pipe(expr, &new_src);
        break;

    default:
        break;
    }

    if (result)
        *match_src = new_src;

    return result;
}

char *regex_match(Expr *expr, char *source)
{
    MatchSrc match_src = {source, 0};

    bool successful = match_dispatch(expr, &match_src);

    if (!successful)
        return NULL;

    int len = match_src.pos;
    char *result = malloc(len + 1);
    memcpy(result, source, len);
    result[len] = '\0';

    return result;
}