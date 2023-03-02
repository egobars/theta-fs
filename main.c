#include <ncurses.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "io.h"

int main() {
    setlocale(LC_ALL, "");
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    int cwd_len = 0;
    for (int i = 0; i < PATH_MAX; ++i) {
        if (cwd[i] == '\0') {
            cwd_len = i;
            break;
        }
    }
    int ext_size = 0;
    FILE *ext_file = fopen("extensions.txt", "ab+");
    while (!feof(ext_file)) {
        ++ext_size;
        char extension[EXT_LEN];
        char library[LIB_LEN];
        fscanf(ext_file, "%s %s", extension, library);
    }
    fclose(ext_file);
    struct extension_library ext_lib;
    struct extension_library *extension_library_list = calloc(ext_size, sizeof(ext_lib));
    ext_file = fopen("extensions.txt", "r");
    int ptr = 0;
    while (!feof(ext_file)) {
        ++ext_size;
        char extension[EXT_LEN];
        char library[LIB_LEN];
        fscanf(ext_file, "%s %s", extension, library);
        add_extension_and_library(&ext_lib, extension, library);
        extension_library_list[ptr] = ext_lib;
        ++ptr;
    }
    fclose(ext_file);
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_WHITE);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_WHITE);
    init_pair(7, COLOR_GREEN, COLOR_BLACK);
    init_pair(8, COLOR_GREEN, COLOR_WHITE);
    raw();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    --rows;
    if (cols < 47) {
        clear();
        printw("Terminal is too small!\n");
        getch();
        return 0;
    }
    int cursor_position = 0;
    int start = 0;
    int end = rows;
    char cursor_object[NAME_MAX];
    int cursor_type;
    long max_count;
    char copy_path[PATH_MAX];
    char copy_name[NAME_MAX];
    bool is_copy = false;
    bool is_cut = false;
    bool is_display_hidden = false;
    while (1) {
        clear();
        print(rows, cols, start, end, cwd, cwd_len, cursor_position, cursor_object, &cursor_type, &max_count, copy_path,
              is_cut, is_display_hidden);
        int val = get_symbol(rows, cols, &start, &end, cwd, &cwd_len, &cursor_position, cursor_object, cursor_type,
                             max_count, copy_path, copy_name, &is_copy, &is_cut, extension_library_list, ext_size,
                             &is_display_hidden);
        if (val == 1) {
            break;
        }
        refresh();
    }
    endwin();
    free(extension_library_list);
    return 0;
}
