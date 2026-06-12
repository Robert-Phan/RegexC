#include <stdlib.h>
#include <string.h>

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
T Name##_pop(Name *list); \
\
void free_##Name(Name *list);\
\
void Name##_clear(Name *list);\
\
void Name##_addall(Name *dest, Name *ori);\

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
\
T Name##_pop(Name *list) {\
    T el = list->array[list->n-1];\
    list->n--;\
    return el;\
}\
\
void free_##Name(Name *list) {\
    free(list->array);\
    free(list);\
}\
\
void Name##_addall(Name *dest, Name *ori)\
{\
    for (int i = 0; i < ori->n; i++)\
    {\
        T el = ori->array[i];\
        Name##_add(dest, el);\
    }\
}\
\
void Name##_clear(Name *list) {\
    memset(list->array, 0, list->a_size * sizeof(T));\
    list->n = 0;\
}\

