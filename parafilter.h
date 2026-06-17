#ifndef PARAFILTER_H
#define PARAFILTER_H

#include <errno.h>
#include <fcntl.h>  // per open
#include <libgen.h> // per basename
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> // per pid_t
#include <sys/wait.h>  // per wait
#include <unistd.h>    // per fork, read, write, close
void cerca(char *keyword, char *nomefile, int pipe_write);

#endif
