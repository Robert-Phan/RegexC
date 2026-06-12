#include "_regex.h"
#include <string.h>

LIST_DEFINE(MatchSrc, SrcList);

#define LOOP                          \
    SrcList *results = new_SrcList(); \
    MatchSrc src;                     \
    for (int i = 0; i < srcs->n; i++) \
    {                                 \
        SrcList_get(srcs, i, &src);

#define LOOP_END }

SrcList *match_dispatch(Expr *expr, SrcList *srcs);

SrcList *match_run(Expr *expr, SrcList *srcs)
{
    char *val = expr->token.value;
    int val_len = strlen(val);

    LOOP;

    char *src_at_pos = src.source + src.pos;

    bool successful = strncmp(src_at_pos, val, val_len) == 0;

    if (!successful)
        continue;

    src.pos += val_len;
    SrcList_add(results, src);

    LOOP_END

    return results;
}

SrcList *match_dot(Expr *expr, SrcList *srcs)
{
    LOOP;

    src.pos++;
    SrcList_add(results, src);

    LOOP_END

    return results;
}

bool match_set_one(Expr *expr, MatchSrc *src)
{
    char *val = expr->token.value;
    int val_len = strlen(val);
    char srcchr = *(src->source + src->pos);

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
                    src->pos++;
                    return true;
                }

        no_range_match:
            i += 2;
            continue;
        }

        if (srcchr == val[i])
        {
            src->pos++;
            return true;
        }
    }

    return false;
}

SrcList *match_set(Expr *expr, SrcList *srcs)
{
    LOOP;
    bool successful = match_set_one(expr, &src);

    if (successful)
        SrcList_add(results, src);
    LOOP_END

    return results;
}

bool match_negset_one(Expr *expr, MatchSrc *src)
{
    char *val = expr->token.value;
    int val_len = strlen(val);
    char srcchr = *(src->source + src->pos);

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

    src->pos++;
    return true;
}

SrcList *match_negset(Expr *expr, SrcList *srcs)
{
    LOOP

        bool successful = match_negset_one(expr, &src);

    if (successful)
        SrcList_add(results, src);
    LOOP_END

    return results;
}

SrcList *match_primary(Expr *expr, SrcList *srcs)
{
    SrcList *results;

    switch (expr->token.type)
    {
    case RUN:
        results = match_run(expr, srcs);
        break;
    case DOT:
        results = match_dot(expr, srcs);
        break;
    case SET:
        results = match_set(expr, srcs);
        break;
    case NEGSET:
        results = match_negset(expr, srcs);
        break;

    default:
        break;
    }

    return results;
}

SrcList *match_concat(Expr *expr, SrcList *srcs)
{
    SrcList *srcs_after_left = match_dispatch(expr->left, srcs);
    SrcList *results = match_dispatch(expr->right, srcs_after_left);

    free_SrcList(srcs_after_left);

    return results;
}

SrcList *match_pipe(Expr *expr, SrcList *srcs)
{
    SrcList *left_results = match_dispatch(expr->left, srcs);
    SrcList *right_results = match_dispatch(expr->right, srcs);

    SrcList *results = new_SrcList();

    SrcList_addall(results, left_results);
    SrcList_addall(results, right_results);

    free_SrcList(left_results);
    free_SrcList(right_results);

    return results;
}

typedef struct
{
    int min;
    int max
} CharRange;

CharRange parse_char_range(char *val)
{
    int comma_loc = 0;

    while (val[comma_loc] != ',' && comma_loc < strlen(val))
        comma_loc++;

    // parse min
    int min = 0;
    if (comma_loc > 0)
    {
        char *min_str = malloc(comma_loc + 1);
        memcpy(min_str, val, comma_loc);
        min_str[comma_loc] = '\0';

        min = atoi(min_str);
        free(min_str);
    }

    // parse max
    int max = -1;
    if (comma_loc < (strlen(val) - 1))
    {
        char *max_str = val + comma_loc + 1;
        max = atoi(max_str);
    }

    return (CharRange){min, max};
}

SrcList *match_range(Expr *expr, SrcList *srcs, CharRange range)
{
    SrcList *results = new_SrcList();
    SrcList *new_srcs = new_SrcList();
    SrcList_addall(new_srcs, srcs);

    int min = range.min;
    int max = range.max;

    while (new_srcs->n > 0 && max != 0)
    {
        if (min <= 0)
            SrcList_addall(results, new_srcs);

        SrcList *old_srcs = new_srcs;
        new_srcs = match_dispatch(expr->left, new_srcs);
        free_SrcList(old_srcs);

        max--;
        min--;
    }

    free_SrcList(new_srcs);
    return results;
}

SrcList *match_quantifiers(Expr *expr, SrcList *srcs)
{
    CharRange range;

    switch (expr->token.type)
    {
    case RANGE:
        range = parse_char_range(expr->token.value);
        break;
    case PLUS:
        range = (CharRange){1, -1};
        break;
    case QMARK:
        range = (CharRange){0, 1};
        break;
    case STAR:
        range = (CharRange){0, -1};
        break;

    default:
        break;
    }

    SrcList *result = match_range(expr, srcs, range);
    return result;
}

void SrcList_uniques(SrcList *srcs)
{
    LOOP
        bool skip = false;
        for (size_t j = 0; j < results->n; j++)
        {
            MatchSrc existing = results->array[j];
            if (src.pos == existing.pos)
                skip = true;

            if (skip)
                continue;
        }

        if (skip)
            continue;

        SrcList_add(results, src);

    LOOP_END

    SrcList_clear(srcs);
    SrcList_addall(srcs, results);
    free_SrcList(results);
}

void filter_ending_srcs(SrcList *srcs)
{
    LOOP if (src.pos < strlen(src.source))
        SrcList_add(results, src);
    LOOP_END

    SrcList_clear(srcs);
    SrcList_addall(srcs, results);
    free_SrcList(results);
}

SrcList *match_dispatch(Expr *expr, SrcList *srcs)
{
    SrcList_uniques(srcs);

    filter_ending_srcs(srcs);

    SrcList *results;

    switch (expr->token.type)
    {
    case RUN:
    case DOT:
    case SET:
    case NEGSET:
        results = match_primary(expr, srcs);
        break;

    case CONCAT:
        results = match_concat(expr, srcs);
        break;
    case PIPE:
        results = match_pipe(expr, srcs);
        break;

    case RANGE:
    case QMARK:
    case PLUS:
    case STAR:
        results = match_quantifiers(expr, srcs);
        break;

    default:
        break;
    }

    return results;
}

Regex compile_regex(char *pattern) {
    TokenList *tokens = scan(pattern);
    Expr *tree = parse(tokens);

    return (Regex){tokens, tree};
}

char *regex_match(Regex regex, char *source)
{
    Expr* expr = regex.tree;

    if (expr == NULL)
        return NULL;

    SrcList *srcs = new_SrcList();
    MatchSrc init_src = {source, 0};
    SrcList_add(srcs, init_src);

    SrcList *results = match_dispatch(expr, srcs);

    int reslen = 0;
    MatchSrc src;
    for (size_t i = 0; i < results->n; i++)
    {
        SrcList_get(results, i, &src);
        if (src.pos > reslen)
            reslen = src.pos;
    }

    char *resstr = malloc(reslen + 1);
    memcpy(resstr, source, reslen);
    resstr[reslen] = '\0';

    free_SrcList(srcs);
    free_SrcList(results);

    return resstr;
}

void free_regex(Regex regex) {
    free_token_strs(regex.tokens);
    free_TokenList(regex.tokens);
    free_expr(regex.tree);
}