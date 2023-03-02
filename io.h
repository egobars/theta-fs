#include "explorers.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

void print(int rows, int cols, int start, int end, char *cwd, int cwd_len, int cursor_position, char *cursor_object,
           int *cursor_type, long *max_count, const char *copy_path, bool is_cut, bool is_display_hidden);

int
get_symbol(int rows, int cols, int *start, int *end, char *cwd, int *cwd_len, int *cursor_position, char *cursor_object,
           int cursor_type, long max_count, char *copy_path, char *copy_name, bool *is_copy, bool *is_cut,
           struct extension_library *extension_command_list, int ecl_size, bool *is_display_hidden);
