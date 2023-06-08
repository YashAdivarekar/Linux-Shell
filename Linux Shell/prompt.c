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

void prompt(int newdirlen, int ogdirlen, long int t, char username[], char hostname[], char currdir[])
{
    if (newdirlen < ogdirlen)
    {
        if (t == 0)
        {
            printf(YELLOW);
            printf("<%s@%s:%s> ", username, hostname, currdir);
            printf(RESET);
        }
        else
        {
            printf(YELLOW);
            printf("<%s@%s:%s took %lds> ", username, hostname, currdir, t);
            printf(RESET);
        }
    }
    else
    {
        if (t == 0)
        {
            printf(YELLOW);
            printf("<%s@%s:~%s> ", username, hostname, (currdir + ogdirlen));
            printf(RESET);
        }
        else
        {
            printf(YELLOW);
            printf("<%s@%s:~%s took %lds> ", username, hostname, (currdir + ogdirlen), t);
            printf(RESET);
        }
    }
}