#include <limits.h>
#include <stdlib.h>

#include "objects_list.h"

void init_list(struct objects_list *list) {
    list->act_len = 0;
    list->rev_len = LIST_REV_SIZE;
    list->objects = calloc(list->rev_len, sizeof(struct object));
}

void realloc_list(struct objects_list *list) {
    list->rev_len *= 2;
    list->objects = realloc(list->objects, list->rev_len * sizeof(struct object));
}

void push_back_list(struct objects_list *list, struct object obj) {
    if (list->act_len == list->rev_len) {
        realloc_list(list);
    }
    list->objects[list->act_len] = obj;
    ++list->act_len;
}

struct object create_object(const char *name, long int size, struct tm last_mod_time, int type) {
    struct object obj;
    obj.name = calloc(NAME_MAX, sizeof(char));
    int ptr = 0;
    while (name[ptr] != '\0') {
        obj.name[ptr] = name[ptr];
        ++ptr;
    }
    obj.name[ptr] = '\0';
    obj.size = size;
    obj.last_mod_time = last_mod_time;
    obj.type = type;
    return obj;
}

void clear_list(struct objects_list *list) {
    for (int i = 0; i < list->act_len; ++i) {
        free(list->objects[i].name);
    }
    free(list->objects);
}
