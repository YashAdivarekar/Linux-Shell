#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "pid.h"
#include "stringsort.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void progpid(char input[], char basedir[])
{
    char temp[200], usetemp[200], conversion[100];

    FILE *fp;
    strcpy(temp, "/proc/");
    int pid;
    if (input != NULL)
    {
        pid = atoi(input);
    }
    else
    {
        pid = getpid();
    }

    // printf("pid : %d\n", pid);
    sprintf(conversion, "%d", pid);

    strcpy(temp, "/proc/");
    strcat(temp, conversion);

    strcat(temp, "/stat");

    fp = fopen(temp, "r");

    if (fp == NULL)
    {
        printf("No process with pid=%d exists\n", pid);
        return;
    }
    else
    {
        printf("pid : %d\n", pid);
    }

    //////////////////////////////////////////////////////////////////////////

    char storage[500];
    char strstore1[500], strstore2[500], strstore3[500];

    for (int i = 0; fscanf(fp, " %s", storage); i++)
    {
        if (i == 2)
        {
            strcpy(strstore3, storage);
            // printf("%s\n",strstore3);
        }
        else if (i == 4)
        {
            strcpy(strstore1, storage);
            // printf("%s\n",strstore1);
        }
        else if (i == 7)
        {
            strcpy(strstore2, storage);
            // printf("%s\n",strstore2);
            break;
        }
    }

    if (strcmp(strstore1, strstore2) == 0)
    {
        printf("process status : %s+\n", strstore3);
    }
    else
    {
        printf("process status : %s\n", strstore3);
    }

    // char *line;
    // size_t length;
    // int flag = 0;
    // char character;

    // for (int i = 0; getline(&line, &length, fp) != -1; i++)
    // {
    //     if (i == 2)
    //     {
    //         character = line[7];
    //         if (line[7] == 'Z')
    //         {
    //             printf("process status : %c\n", line[7]);
    //             flag++;
    //             break;
    //         }
    //     }
    //     if (i == 3)
    //     {
    //         strcpy(strstore1, line);
    //     }
    //     if (i == 5)
    //     {
    //         strcpy(strstore2, line);
    //         break;
    //     }
    // }

    // printf("%s\n%s\n",strstore1,strstore2);

    // if (flag == 0)
    // {
    //     if (strcmp(&(strstore1[4]), &(strstore2[3])) == 0)
    //     {
    //         printf("process status : %c+\n", character);
    //     }
    //     else
    //     {
    //         printf("process status : %c\n", character);
    //     }
    // }

    //////////////////////////////////////////////////////////////////////////

    strcpy(temp, "/proc/");
    strcat(temp, conversion);

    strcat(temp, "/statm");

    fp = fopen(temp, "r");

    char memory[200];

    fscanf(fp, "%s", memory);
    printf("memory : %s {Virtual Memory}\n", memory);

    //////////////////////////////////////////////////////////////////////////////

    strcpy(temp, "/proc/");
    strcat(temp, conversion);

    strcat(temp, "/exe");

    char comparebase[PATH_MAX], execpath[PATH_MAX];

    readlink(temp, execpath, PATH_MAX);

    for (int i = 0; i < strlen(basedir); i++)
    {
        comparebase[i] = execpath[i];
    }
    comparebase[(strlen(basedir))] = '\0';

    if (strcmp(comparebase, basedir) == 0)
    {
        printf("Executable path: ~%s\n", (execpath + strlen(basedir)));
    }
    else
    {
        printf("Executable path: ~%s\n", (execpath));
    }
}