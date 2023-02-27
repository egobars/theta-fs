#!/bin/bash

gcc main.c explorers.c io.c objects_list.c utils.c constants.h explorers.h io.h objects_list.h utils.h -lncursesw
./a.out
