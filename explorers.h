#include <dirent.h>
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "utils.h"

struct objects_list get_objects_from_dir(char *path, int path_len, bool need_dir, bool need_hidden, bool *cant_open);
