#include <ctype.h>
#include <limits.h>
#include <ncurses.h>
#include <unistd.h>

#include "utils.h"

int compare_objects(const void *first_object_ptr, const void *second_object_ptr) {
    char *first_string = ((struct object *)first_object_ptr)->name;
    char *second_string = ((struct object *)second_object_ptr)->name;
    int ptr = 0;
    while (1) {
        if (tolower(first_string[ptr]) != tolower(second_string[ptr])) {
            return tolower(first_string[ptr]) - tolower(second_string[ptr]);
        }
        ++ptr;
    }
}

bool dir_is_dot(const char *name) {
    if (name[0] == '.' && name[1] == '\0') {
        return true;
    }
    return false;
}

bool dir_is_back(const char *name) {
    if (name[0] == '.' && name[1] == '.' && name[2] == '\0') {
        return true;
    }
    return false;
}

bool is_hidden(const char *name) {
    if (!dir_is_dot(name) && !dir_is_back(name) && name[0] == '.') {
        return true;
    }
    return false;
}

void add_name_to_path(char *path, const char *name) {
    int path_ptr = 0;
    while (path[path_ptr] != '\0') {
        ++path_ptr;
    }
    if (path_ptr == 1) {
        path_ptr = 0;
    }
    path[path_ptr] = '/';
    ++path_ptr;
    int name_ptr = 0;
    while (name[name_ptr] != '\0') {
        path[path_ptr] = name[name_ptr];
        ++path_ptr;
        ++name_ptr;
    }
    path[path_ptr] = '\0';
}

void get_round_size_and_suffix(long int *size, char *suffix, int *suffix_len) {
    if (*size < 1024) {
        suffix[0] = 'b';
        suffix[1] = '\0';
        *suffix_len = 1;
    } else if (*size < 1024 * 1024) {
        *size /= 1024;
        suffix[0] = 'k';
        suffix[1] = 'b';
        suffix[2] = '\0';
        *suffix_len = 2;
    } else if (*size < 1024 * 1024 * 1024) {
        *size /= 1024 * 1024;
        suffix[0] = 'M';
        suffix[1] = 'b';
        suffix[2] = '\0';
        *suffix_len = 2;
    } else {
        *size /= 1024 * 1024 * 1024;
        suffix[0] = 'G';
        suffix[1] = 'b';
        suffix[2] = '\0';
        *suffix_len = 2;
    }
}

void add_extension_and_library(struct extension_library *ext_com, const char *extension, const char *command) {
    int ptr = 0;
    while (1) {
        ext_com->extension[ptr] = extension[ptr];
        if (ext_com->extension[ptr] == '\0') {
            break;
        }
        ++ptr;
    }
    ptr = 0;
    while (1) {
        ext_com->library[ptr] = command[ptr];
        if (ext_com->library[ptr] == '\0') {
            break;
        }
        ++ptr;
    }
}

void get_extension(const char *name, char *extension) {
    int len;
    int ptr = 0;
    while (1) {
        if (name[ptr] == '\0') {
            break;
        }
        ++ptr;
    }
    len = ptr + 1;
    while (name[ptr] != '.') {
        if (ptr == 0) {
            extension[0] = '\0';
            return;
        }
        --ptr;
    }
    if (ptr == 0) {
        extension[0] = '\0';
        return;
    }
    for (int i = ptr + 1; i < len; ++i) {
        extension[i - ptr - 1] = name[i];
    }
    extension[len - ptr] = '\0';
}

bool is_same(const char *first, const char *second) {
    int ptr = 0;
    while (1) {
        if (first[ptr] != second[ptr]) {
            return false;
        }
        if (first[ptr] == '\0') {
            break;
        }
        ++ptr;
    }
    return true;
}

void add_to_filename(char *cwd, int cwd_len) {
    int to_add = 0;
    while (1) {
        int now = to_add;
        char array[NAME_MAX];
        int ptr = 0;
        while (now > 0) {
            array[ptr] = '0' + now % 10;
            ++ptr;
            now /= 10;
        }
        array[ptr] = '\0';
        for (int i = 0; i < ptr + 1; ++i) {
            cwd[cwd_len + i] = array[i];
        }
        if (access(cwd, F_OK) != 0) {
            break;
        }
        ++to_add;
    }
}
