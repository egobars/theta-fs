#include <time.h>

#include "constants.h"

struct object {
    char *name;
    long int size;
    struct tm last_mod_time;
    int type;
};

struct objects_list {
    struct object *objects;
    long int rev_len;
    long int act_len;
};

void init_list(struct objects_list *list);

void realloc_list(struct objects_list *list);

void push_back_list(struct objects_list *list, struct object obj);

struct object create_object(const char *name, long int size, struct tm last_mod_time, int type);

void clear_list(struct objects_list *list);
