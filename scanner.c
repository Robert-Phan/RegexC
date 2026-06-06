#include "scanner.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static bool is_at_end(Scanner *sc)
{
    return sc->current >= strlen(sc->source);
}

static char advance(Scanner *sc)
{
    return *(sc->source + sc->current++);
}

static char peek(Scanner *sc)
{
    if (is_at_end(sc))
        return '\0';

    return *(sc->source + sc->current);
}

static char peek_next(Scanner *sc)
{
    int next = sc->current + 1;

    if (next >= strlen(sc->source))
        return '\0';

    return *(sc->source + next);
}

static bool match(Scanner *sc, char *cs)
{
    if (is_at_end(sc))
        return false;

    int i = 0;
    while (cs[i] != '\0')
    {
        if (peek(sc) == cs[i])
        {
            sc->current++;
            return true;
        }

        i++;
    }

    return false;
}

static char *get_value_custom(Scanner *sc, int start, int end) {
    int len = end - start;
    char *suf = sc->source + start;

    char *value = malloc(len + 1);
    memcpy(value, suf, len);
    value[len] = '\0';

    return value;
}

static void add_token_custom(Scanner *sc, TokenType type, int start, int end) {
    char *value = get_value_custom(sc, start, end);
    Token token = new_token(type, value);
    TokenList_add(sc->list, token);
}

static void add_token(Scanner *sc, TokenType type)
{
    add_token_custom(sc, type, sc->start, sc->current);
}

static bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool is_escape(char c) {
    return strchr("wWdDsSbBtnr", c) != NULL;
}

static void scan_escape(Scanner *sc)
{
    TokenType type = is_escape(peek(sc)) ? ESCAPE : RUN;

    advance(sc);

    add_token_custom(sc, type, sc->start+1, sc->current);
}

static bool is_quant(char c) {
    char *quant = "{}+?*";
    return strchr(quant, c) != NULL;
}

static bool is_base(char c)
{
    char *not_base = "{}[]()^$\\+?*|";
    return strchr(not_base, c) == NULL;
}

static void scan_run(Scanner *sc)
{
    while (is_base(peek(sc))) {
        if (is_quant(peek_next(sc)))
            break;
        
        advance(sc);
    }

    add_token(sc, RUN);
}

static void scan_set(Scanner *sc)
{
    bool is_negset = false;

    if (match(sc, "^"))
        is_negset = true;

    while (peek(sc) != ']' || is_at_end(sc))
    {
        advance(sc);
    }

    if (is_at_end(sc))
    {
        // TODO Error Handling
        return;
    }

    advance(sc);

    add_token_custom(sc, is_negset ? NEGSET : SET, sc->start+1, sc->current-1);
}

static void scan_range(Scanner *sc)
{
    while (is_digit(peek(sc)))
        advance(sc);

    if (peek(sc) == ',')
    {
        advance(sc);

        while (is_digit(peek(sc)))
            advance(sc);
    }

    if (is_at_end(sc))
    {
        // TODO Error Handling
        return;
    }

    advance(sc);

    add_token_custom(sc, RANGE, sc->start+1, sc->current-1);
}

static void scan_token(Scanner *sc)
{
    char c = advance(sc);

    switch (c)
    {
    case '\\':
        scan_escape(sc);
        break;
    case '.':
        add_token(sc, DOT);
        break;

    case '^':
        add_token(sc, CARET);
        break;
    case '$':
        add_token(sc, DOLLAR);
        break;

    case '(':
        add_token(sc, LEFT_PAREN);
        break;
    case ')':
        add_token(sc, RIGHT_PAREN);
        break;
    case '|':
        add_token(sc, PIPE);
        break;

    case '+':
        add_token(sc, PLUS);
        break;
    case '?':
        add_token(sc, QMARK);
        break;
    case '*':
        add_token(sc, STAR);
        break;

    case '[':
        scan_set(sc);
        break;
    case '{':
        scan_range(sc);
        break;

    default:
        scan_run(sc);
        break;
    }
}

TokenList *scan(char *source)
{
    TokenList *list = new_TokenList();
    Scanner scanner = {0, 0, source, list};

    while (!is_at_end(&scanner))
    {
        scanner.start = scanner.current;
        scan_token(&scanner);
    }

    return list;
}