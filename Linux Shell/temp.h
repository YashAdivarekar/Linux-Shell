#ifndef TEMPDEF
#define TEMPDEF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "prompt.h"
#include "input.h"
#include "ls.h"
#include "discover.h"
#include "history.h"
#include "bg.h"
#include "pid.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void execute(char fgcommandgoingon[], pid_t pid, char subinput[], char currdir[], char basedir[], char prevdir[], char andtempsubinput1[], int pipecount, char *ptr4, char *ptr3, char *ptr2, char *ptr1);

#endif