#include <stdlib.h>

/* Used for declaring list identifiers in headers */

#define LIST_DECLARE(T, Name) \
\
typedef struct { \
    int a_size; \
    int n; \
    T *array; \
} Name; \
\
Name *new_##Name(); \
\
int Name##_get(Name *list, int i, T *el); \
\
void Name##_add(Name *list, T el); \
\
void free_##Name(Name *list);

/* Used for defining code in source files */

#define LIST_DEFINE(T, Name)\
\
Name *new_##Name() {\
    int def_a_size = 10;\
\
    Name *list = malloc(sizeof(Name));\
    *list = (Name) {\
        def_a_size, 0, calloc(def_a_size, sizeof(T))\
    };\
\
    return list;\
}\
\
int Name##_get(Name *list, int i, T *el) {\
    if (i < 0 || i >= list->n)\
        return 0;\
\
    *el = list->array[i];\
    return 1;\
}\
\
static void expand_##Name(Name *list) {\
    if (list->n < (0.8f * list->a_size))\
        return;\
\
    size_t new_size = list->a_size * 2;\
\
    T *new_array =\
        realloc(list->array, new_size * sizeof(T));\
\
    if (new_array == NULL) {\
        \
        return;\
    }\
\
    list->array = new_array;\
    list->a_size = new_size;\
}\
\
void Name##_add(Name *list, T el) {\
    expand_##Name(list);\
    \
    list->array[list->n] = el;\
    list->n++;\
}\
