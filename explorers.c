#include <dirent.h>
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "explorers.h"

struct objects_list get_objects_from_dir(char *path, int path_len, bool need_dir, bool need_hidden) {
    DIR *cur_dir = opendir(path);
    struct dirent *data;
    struct objects_list objects;
    init_list(&objects);
    while (1) {
        data = readdir(cur_dir);
        if (data == NULL) {
            break;
        }
        if (!need_hidden && is_hidden(data->d_name)) {
            continue;
        }
        if (dir_is_dot(data->d_name)) {
            continue;
        }
        if (dir_is_back(data->d_name) && path_len == 1) {
            continue;
        }
        if ((data->d_type == DT_DIR) == need_dir) {
            struct stat obj_stat;
            add_name_to_path(path, data->d_name);
            stat(path, &obj_stat);
            path[path_len] = '\0';
            int type;
            if (data->d_type == DT_DIR) {
                type = DIR_TYPE;
            }
            if (data->d_type == DT_REG) {
                type = FILE_TYPE;
            }
            if (data->d_type == DT_LNK) {
                type = LINK_TYPE;
            }
            if (data->d_type == DT_FIFO) {
                type = FIFO_TYPE;
            }
            push_back_list(&objects, create_object(data->d_name, obj_stat.st_size, *localtime(&obj_stat.st_mtime), type));
        }
    }
    closedir(cur_dir);
    qsort(objects.objects, objects.act_len, sizeof(objects.objects[0]), compare_objects);
    return objects;
}
