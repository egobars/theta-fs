#include <fcntl.h>
#include <ncurses.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#include "io.h"

void print(int rows, int cols, int start, int end, char *cwd, int cwd_len, int cursor_position, char *cursor_object,
           int *cursor_type, long *max_count, const char *copy_path, bool is_cut, bool is_display_hidden) {
    attron(COLOR_PAIR(1));
    printw("T %*s %10s %27s\n", -cols + 4 + 38, "Name", "Size", "Date modified");
    struct objects_list dirs = get_objects_from_dir(cwd, cwd_len, true, is_display_hidden);
    struct objects_list files = get_objects_from_dir(cwd, cwd_len, false, is_display_hidden);
    long dirs_len = dirs.act_len;
    long files_len = files.act_len;
    for (int i = MAX(start, 0); i < MIN(dirs_len, end); ++i) {
        if (i == cursor_position) {
            attron(COLOR_PAIR(2));
        } else {
            attron(COLOR_PAIR(1));
        }
        char *cur_name = calloc(NAME_MAX, 1);
        int ptr = 0;
        while (1) {
            cur_name[ptr] = dirs.objects[i].name[ptr];
            if (cur_name[ptr] == '\0') {
                break;
            }
            ++ptr;
        }
        int name_len = 0;
        while (cur_name[name_len] != '\0') {
            ++name_len;
        }
        if (dir_is_back(cur_name)) {
            printw("D %*s\n", -cols + 3, cur_name);
        } else {
            int russians_letter_count = 0;
            for (int j = 0; j < name_len; j += 2) {
                if ((int) cur_name[j] < 0) {
                    ++russians_letter_count;
                }
            }
            char last_mod_time[30];
            strftime(last_mod_time, 26, "%b %d %Y", &dirs.objects[i].last_mod_time);
            if (name_len > cols - 4 - 27 + russians_letter_count) {
                cur_name[cols - 4 - 27 + russians_letter_count] = '\0';
                cur_name[cols - 4 - 27 + russians_letter_count - 1] = '.';
                cur_name[cols - 4 - 27 + russians_letter_count - 2] = '.';
                cur_name[cols - 4 - 27 + russians_letter_count - 3] = '.';
            }
            printw("D %*s %30s\n", -cols + 4 + 27 - russians_letter_count, cur_name, last_mod_time);
        }
        if (i == cursor_position) {
            for (int j = 0; j < name_len; ++j) {
                cursor_object[j] = cur_name[j];
            }
            cursor_object[name_len] = '\0';
            *cursor_type = DIR_TYPE;
        }
        free(cur_name);
    }
    for (int i = MAX(start - dirs_len, 0); i < MIN(files_len, end - dirs_len); ++i) {
        if (dirs_len + i == cursor_position) {
            if (files.objects[i].type == FILE_TYPE) {
                attron(COLOR_PAIR(2));
            } else if (files.objects[i].type == LINK_TYPE) {
                attron(COLOR_PAIR(6));
            } else if (files.objects[i].type == FIFO_TYPE) {
                attron(COLOR_PAIR(8));
            }
        } else {
            if (files.objects[i].type == FILE_TYPE) {
                attron(COLOR_PAIR(1));
            } else if (files.objects[i].type == LINK_TYPE) {
                attron(COLOR_PAIR(5));
            } else if (files.objects[i].type == FIFO_TYPE) {
                attron(COLOR_PAIR(7));
            }
        }
        char *cur_name = calloc(NAME_MAX, 1);
        int ptr = 0;
        while (1) {
            cur_name[ptr] = files.objects[i].name[ptr];
            if (cur_name[ptr] == '\0') {
                break;
            }
            ++ptr;
        }
        int name_len = 0;
        while (cur_name[name_len] != '\0') {
            ++name_len;
        }
        if (is_cut) {
            add_name_to_path(cwd, cur_name);
            bool is_sim = true;
            ptr = 0;
            while (1) {
                if (cwd[ptr] != copy_path[ptr]) {
                    is_sim = false;
                    break;
                }
                if (cwd[ptr] == '\0') {
                    break;
                }
                ++ptr;
            }
            if (is_sim) {
                if (dirs_len + i == cursor_position) {
                    attron(COLOR_PAIR(4));
                } else {
                    attron(COLOR_PAIR(3));
                }
            }
            cwd[cwd_len] = '\0';
        }
        int russians_letter_count = 0;
        for (int j = 0; j < name_len; j += 2) {
            if ((int) cur_name[j] < 0) {
                ++russians_letter_count;
            }
        }
        char suffix[3];
        int suffix_len = 0;
        get_round_size_and_suffix(&files.objects[i].size, suffix, &suffix_len);
        char last_mod_time[30];
        strftime(last_mod_time, 26, "%b %d %Y", &files.objects[i].last_mod_time);
        if (name_len > cols - 4 - 38 - suffix_len + russians_letter_count) {
            cur_name[cols - 4 - 38 - suffix_len + russians_letter_count] = '\0';
            cur_name[cols - 4 - 38 - suffix_len + russians_letter_count - 1] = '.';
            cur_name[cols - 4 - 38 - suffix_len + russians_letter_count - 2] = '.';
            cur_name[cols - 4 - 38 - suffix_len + russians_letter_count - 3] = '.';
        }
        printw("F %*s %10d%s %30s\n", -cols + 4 + 38 + suffix_len - russians_letter_count, cur_name,
               files.objects[i].size, suffix, last_mod_time);
        if (i == cursor_position - dirs_len) {
            for (int j = 0; j < name_len; ++j) {
                cursor_object[j] = cur_name[j];
            }
            cursor_object[name_len] = '\0';
            *cursor_type = FILE_TYPE;
        }
        free(cur_name);
    }
    *max_count = dirs_len + files_len;
    clear_list(&dirs);
    clear_list(&files);
}

int
get_symbol(int rows, int cols, int *start, int *end, char *cwd, int *cwd_len, int *cursor_position, char *cursor_object,
           int cursor_type, long max_count, char *copy_path, char *copy_name, bool *is_copy, bool *is_cut,
           struct extension_command *extension_command_list, int ecl_size, bool *is_display_hidden) {
    int key = getch();
    if (key == 'q') {
        return 1;
    }
    if (key == 258 && *cursor_position < max_count - 1) {
        ++(*cursor_position);
        if (*cursor_position == *end) {
            ++(*start);
            ++(*end);
        }
    } else if (key == 259 && *cursor_position > 0) {
        --(*cursor_position);
        if (*cursor_position < *start) {
            --(*start);
            --(*end);
        }
    } else if (key == 10) {
        if (cursor_type == DIR_TYPE) {
            if (dir_is_back(cursor_object)) {
                printw("%s\n", cwd);
                while (*cwd_len > 0 && cwd[*cwd_len] != '/') {
                    --(*cwd_len);
                }
                if (*cwd_len != 0) {
                    cwd[*cwd_len] = '\0';
                } else {
                    cwd[1] = '\0';
                    *cwd_len = 1;
                }
                printw("%s\n", cwd);
            } else {
                add_name_to_path(cwd, cursor_object);
                while (cwd[*cwd_len] != '\0') {
                    ++(*cwd_len);
                }
            }
            *cursor_position = 0;
            *start = 0;
            *end = rows;
        } else {
            char extension[10];
            char command[10];
            bool found = false;
            get_extension(cursor_object, extension);
            for (int i = 0; i < ecl_size; ++i) {
                if (is_same(extension_command_list[i].extension, extension)) {
                    found = true;
                    for (int j = 0; j < 10; ++j) {
                        command[j] = extension_command_list[i].command[j];
                    }
                }
            }
            if (found && fork() == 0) {
                add_name_to_path(cwd, cursor_object);
                execlp(command, "1", cwd, NULL);
            }
        }
    } else if (key == 'd') {
        if (cursor_type == FILE_TYPE) {
            bool want_delete = true;
            clear();
            attron(COLOR_PAIR(1));
            printw("Are you sure? (y/n)");
            refresh();
            while (1) {
                key = getch();
                if (key == 'n') {
                    want_delete = false;
                }
                if (key == 'y' || key == 'n') {
                    break;
                }
            }
            if (want_delete) {
                add_name_to_path(cwd, cursor_object);
                remove(cwd);
                cwd[*cwd_len] = '\0';
                if (*cursor_position == max_count - 1) {
                    --(*cursor_position);
                }
            }
        }
    } else if (key == 'c') {
        if (cursor_type == FILE_TYPE) {
            add_name_to_path(cwd, cursor_object);
            int ptr = 0;
            while (cwd[ptr] != '\0') {
                ++ptr;
            }
            ++ptr;
            for (int i = 0; i < ptr; ++i) {
                copy_path[i] = cwd[i];
            }
            copy_path[ptr] = '\0';
            cwd[*cwd_len] = '\0';
            ptr = 0;
            while (cursor_object[ptr] != '\0') {
                ++ptr;
            }
            ++ptr;
            for (int i = 0; i < ptr; ++i) {
                copy_name[i] = cursor_object[i];
            }
            copy_name[ptr] = '\0';
            *is_cut = false;
            *is_copy = true;
        }
    } else if (key == 'x') {
        if (cursor_type == FILE_TYPE) {
            add_name_to_path(cwd, cursor_object);
            int ptr = 0;
            while (cwd[ptr] != '\0') {
                ++ptr;
            }
            ++ptr;
            for (int i = 0; i < ptr; ++i) {
                copy_path[i] = cwd[i];
            }
            copy_path[ptr] = '\0';
            cwd[*cwd_len] = '\0';
            ptr = 0;
            while (cursor_object[ptr] != '\0') {
                ++ptr;
            }
            ++ptr;
            for (int i = 0; i < ptr; ++i) {
                copy_name[i] = cursor_object[i];
            }
            copy_name[ptr] = '\0';
            *is_cut = true;
        }
    } else if (key == 'v') {
        if (*is_copy || *is_cut) {
            int src_file = open(copy_path, O_RDONLY);
            struct stat src_file_stat;
            stat(copy_path, &src_file_stat);
            add_name_to_path(cwd, copy_name);
            if (access(cwd, F_OK) == 0) {
                clear();
                attron(COLOR_PAIR(1));
                printw("File with this name already exists. Do you want to rewrite it? (y/n)");
                refresh();
                bool want_rewrite = true;
                while (1) {
                    key = getch();
                    if (key == 'n') {
                        want_rewrite = false;
                    }
                    if (key == 'y' || key == 'n') {
                        break;
                    }
                }
                if (!want_rewrite) {
                    cwd[*cwd_len] = '\0';
                    close(src_file);
                    return 0;
                }
            }
            int dst_file = creat(cwd, src_file_stat.st_mode);
            cwd[*cwd_len] = '\0';
            char buffer[4096];
            while (1) {
                long cnt = read(src_file, buffer, 4096);
                write(dst_file, buffer, cnt);
                if (cnt < 4096) {
                    break;
                }
            }
            close(src_file);
            close(dst_file);
            if (*is_cut) {
                remove(copy_path);
                *is_cut = false;
            }
        }
    } else if (key == 'h') {
        *is_display_hidden = !(*is_display_hidden);
        *cursor_position = 0;
        *start = 0;
        *end = rows;
    }
    return 0;
}
