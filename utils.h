#include <ctype.h>
#include <ncurses.h>

#include "objects_list.h"

struct extension_command {
    char extension[10];
    char command[10];
};

int compare_objects(const void *first_object_ptr, const void *second_object_ptr);

bool dir_is_dot(const char *name);

bool dir_is_back(const char *name);

bool is_hidden(const char *name);

void add_name_to_path(char *path, const char *name);

void get_round_size_and_suffix(long int *size, char *suffix, int *suffix_len);

void add_extension_and_command(struct extension_command *ext_com, const char *extension, const char *command);

void get_extension(const char *name, char *extension);

bool is_same(const char *first, const char *second);