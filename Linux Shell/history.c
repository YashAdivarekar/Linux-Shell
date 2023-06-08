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
#include "history.h"
#include "stringsort.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void createhistory(char input1[], char basedir[])
{
    char input[500];
    strcpy(input, input1);
    int len = strlen(input);
    input[len] = '\n';
    input[(len + 1)] = '\0';

    // printf("Created history\n");
    char store[20][500];
    char *temp;
    size_t sizeoftemp;

    char fileopen[5000];
    strcpy(fileopen, basedir);
    strcat(fileopen, "/history.txt");

    FILE *fp = fopen(fileopen, "r");
    // printf("ok\n");
    int i = 0, flag = 0;
    if (fp == NULL)
    {
        // printf("NULL hai\n");
        // return;
        FILE *tempfp = fopen(fileopen, "w");
        fclose(tempfp);

        fp = fopen(fileopen, "r");
    }
    while ((getline(&temp, &sizeoftemp, fp) != -1) && (i < 20))
    {
        // printf("ok1\n");
        // printf("Create--> %s\n",temp);
        if (i == 0 && strcmp(temp, input) != 0)
        {
            strcpy(store[i], input);
            i++;
            flag++;
        }
        strcpy(store[i], temp);
        i++;
    }

    // for (int x = 0; x<i ; x++)
    // {
    //     printf("%s\n",store[x]);
    // }

    fclose(fp);
    // printf("Created history in create\n");

    fp = fopen(fileopen, "w");
    // printf("Created history\n");
    if (flag == 0 && i == 0)
    {
        // printf("Here: %s\n",input);
        fprintf(fp, "%s", input);
    }

    for (int j = 0; j < i; j++)
    {
        fprintf(fp, "%s", store[j]);
        // printf("%s",store[j]);
    }
    fclose(fp);
    // printf("Created history\n");
}

void history(char basedir[])
{
    char *temp;
    size_t sizeoftemp;
    char store[25][500];
    char fileopen[5000];
    strcpy(fileopen, basedir);
    strcat(fileopen, "/history.txt");
    FILE *fp = fopen(fileopen, "r");
    if (fp == NULL)
    {
        return;
    }

    int i = 0;
    while (getline(&temp, &sizeoftemp, fp) != -1)
    {
        // printf("--> %s", temp);
        strcpy(store[i], temp);
        i++;
    }
    // printf("%d",i);
    if (i > 10)
    {
        i = 10;
    }
    // printf("%d",i);

    for (int j = i - 1; j >= 0; j--)
    {
        printf("%s", store[j]);
    }

    fclose(fp);
}