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
#include "ls.h"
#include "stringsort.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void progls(char *subcommand, char **ptr2, char *basedir)
{
    char *commands[100];
    for (int i = 0; i < 100; i++)
    {
        commands[i] = (char *)malloc(sizeof(char) * 100);
    }

    int flags = 0, dircount = 0;

    while (1)
    {
        subcommand = strtok_r(NULL, " \t\n", ptr2);
        if (subcommand == NULL)
        {
            break;
        }
        if (strcmp("-a", subcommand) == 0)
        {
            if (flags == 0 || flags == 1)
            {
                flags = 1;
            }
            else if (flags == 2 || flags == 3)
            {
                flags = 3;
            }
        }
        else if (strcmp("-l", subcommand) == 0)
        {
            if (flags == 0 || flags == 2)
            {
                flags = 2;
            }
            else if (flags == 1 || flags == 3)
            {
                flags = 3;
            }
        }
        else if (strcmp("-al", subcommand) == 0 || strcmp("-la", subcommand) == 0)
        {
            flags = 3;
        }

        else
        {
            if (strcmp("~", subcommand) == 0)
            {
                strcpy(commands[dircount], basedir);
                dircount++;
            }
            else
            {
                strcpy(commands[dircount], subcommand);
                dircount++;
            }
        }
    }

    if (dircount == 0)
    {
        strcpy(commands[dircount], ".");
        dircount++;
    }

    if (flags == 0) // ls
    {
        // printf("ls\n");
        // ls .
        ls(commands, dircount);
    }
    else if (flags == 1) // ls -a
    {
        // printf("lsa\n");
        lsa(commands, dircount);
    }
    else if (flags == 2) // ls -l
    {
        // printf("lsl\n");
        // for (int i = 0; i < dircount; i++)
        // {
        //     printf("%s\n", commands[i]);
        // }

        lsl(commands, dircount);
    }
    else // ls -al
    {
        // printf("lsal\n");
        lsal(commands, dircount);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ls(char *arrin[], int n)
{
    for (int w = 0; w < n; w++)
    {
        int file = access(arrin[w], F_OK);

        struct dirent *ptrdirentry;
        DIR *dir = opendir(arrin[w]);
        int count = 0;

        if (file == 0 && dir == NULL)
        {
            struct stat sb;
            char temp[10000];
            strcpy(temp, arrin[w]);
            lstat(temp, &sb);

            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arrin[w]);
            }
            continue;
        }

        if (file != 0 && dir == NULL)
        {
            printf("No such file or directory\n");
            continue;
        }

        while ((ptrdirentry = readdir(dir)) != NULL)
        {
            if (ptrdirentry->d_name[0] != '.')
            {
                count++;
            }
        }

        char *arr[count];

        for (int i = 0; i < count; i++)
        {
            arr[i] = (char *)malloc(sizeof(char) * 100);
        }

        rewinddir(dir);

        if (dir == NULL)
        {
            printf("Could not open current directory\n");
            continue;
        }

        for (int i = 0; i < count; i)
        {
            ptrdirentry = readdir(dir);
            if (ptrdirentry->d_name == NULL)
            {
                printf("NULL reached\n");
                break;
            }
            // printf("%s\n", ptrdirentry->d_name);
            if (ptrdirentry->d_name[0] != '.')
            {
                strcpy(arr[i], ptrdirentry->d_name);
                i++;
            }
        }

        closedir(dir);

        // for (int i = 0; i < count; i++)
        // {
        //     printf("%s\n",arr[i]);
        // }

        // printf("=====================\n");

        stringsort(arr, count);

        for (int i = 0; i < count; i++)
        {
            struct stat sb;
            char temp[10000];
            strcpy(temp, arrin[w]);
            strcat(temp, "/");
            strcat(temp, arr[i]);
            lstat(temp, &sb);

            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arr[i]);
            }
        }
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void lsa(char *arrin[], int n)
{
    for (int w = 0; w < n; w++)
    {
        int file = access(arrin[w], F_OK);

        struct dirent *ptrdirentry;
        DIR *dir = opendir(arrin[w]);
        int count = 0;

        if (file == 0 && dir == NULL)
        {
            struct stat sb;
            char temp[10000];
            strcpy(temp, arrin[w]);
            lstat(temp, &sb);

            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arrin[w]);
            }
            continue;
        }

        if (file != 0 && dir == NULL)
        {
            printf("No such file or directory\n");
            continue;
        }

        while ((ptrdirentry = readdir(dir)) != NULL)
        {
            count++;
        }

        char *arr[count];

        for (int i = 0; i < count; i++)
        {
            arr[i] = (char *)malloc(sizeof(char) * 100);
        }

        rewinddir(dir);

        if (dir == NULL)
        {
            printf("Could not open current directory\n");
            return;
        }

        for (int i = 0; i < count; i)
        {
            ptrdirentry = readdir(dir);
            if (ptrdirentry->d_name == NULL)
            {
                printf("NULL reached\n");
                break;
            }
            // printf("%s\n", ptrdirentry->d_name);
            strcpy(arr[i], ptrdirentry->d_name);
            i++;
        }

        closedir(dir);

        // for (int i = 0; i < count; i++)
        // {
        //     printf("%s\n",arr[i]);
        // }

        // printf("=====================\n");

        stringsort(arr, count);

        for (int i = 0; i < count; i++)
        {
            struct stat sb;
            char temp[10000];
            strcpy(temp, arrin[w]);
            strcat(temp, "/");
            strcat(temp, arr[i]);
            lstat(temp, &sb);

            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arr[i]);
            }
        }
    }
    return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void lsl(char *arrin[], int n)
{
    for (int w = 0; w < n; w++)
    {
        int file = access(arrin[w], F_OK);

        struct dirent *ptrdirentry;
        DIR *dir = opendir(arrin[w]);
        int count = 0;

        if (file == 0 && dir == NULL)
        {
            // printf("%s\n", arrin[w]);
            int total = 0;
            struct stat sb;
            if (lstat(arrin[w], &sb) == -1)
            {
                perror("lstat");
                exit(EXIT_FAILURE);
            }

            total += (sb.st_blocks / 2);
            printf("total %d\n", total);

            if (S_ISDIR(sb.st_mode))
            {
                printf("d");
            }
            else
            {
                printf("-");
            }
            /////////////////////////////////////////////////////////////////////////
            if (sb.st_mode & S_IRUSR)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }
            ///////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IRGRP)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWGRP)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXGRP)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }

            //////////////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IROTH)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWOTH)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXOTH)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }
            char time[20];
            strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&sb.st_mtime));

            printf(" %-4ld %s %s %-8ld %s ", sb.st_nlink, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, sb.st_size, time);

            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arrin[w]);
            }

            continue;
        }

        if (file != 0 && dir == NULL)
        {
            printf("No such file or directory\n");
            continue;
        }

        while ((ptrdirentry = readdir(dir)) != NULL)
        {
            if (ptrdirentry->d_name[0] != '.')
            {
                count++;
            }
        }

        char *arr[count];

        for (int i = 0; i < count; i++)
        {
            arr[i] = (char *)malloc(sizeof(char) * 100);
        }

        rewinddir(dir);

        if (dir == NULL)
        {
            printf("Could not open current directory\n");
            return;
        }

        for (int i = 0; i < count; i)
        {
            ptrdirentry = readdir(dir);
            if (ptrdirentry->d_name == NULL)
            {
                printf("NULL reached\n");
                break;
            }
            // printf("%s\n", ptrdirentry->d_name);
            if (ptrdirentry->d_name[0] != '.')
            {
                strcpy(arr[i], ptrdirentry->d_name);
                i++;
            }
        }

        closedir(dir);
        stringsort(arr, count);

        int total = 0;

        for (int i = 0; i < count; i++)
        {
            struct stat sb;

            char temp[400];
            strcpy(temp, arrin[w]);
            temp[strlen(arrin[w])] = '/';
            temp[(strlen(arrin[w]) + 1)] = '\0';
            strcat(temp, arr[i]);

            // printf("%s\n", temp);
            if (lstat(temp, &sb) == -1)
            {
                perror("lstat");
                exit(EXIT_FAILURE);
            }

            total += (sb.st_blocks / 2);
        }
        printf("total %d\n", total);

        for (int i = 0; i < count; i++)
        {
            struct stat sb;
            char temp[400];
            strcpy(temp, arrin[w]);
            temp[strlen(arrin[w])] = '/';
            temp[(strlen(arrin[w]) + 1)] = '\0';
            strcat(temp, arr[i]);

            if (lstat(temp, &sb) == -1)
            {
                perror("lstat");
                exit(EXIT_FAILURE);
            }

            if (S_ISDIR(sb.st_mode))
            {
                printf("d");
            }
            else
            {
                printf("-");
            }
            /////////////////////////////////////////////////////////////////////////
            if (sb.st_mode & S_IRUSR)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }
            ///////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IRGRP)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWGRP)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXGRP)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }

            //////////////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IROTH)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWOTH)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXOTH)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }
            char time[20];
            strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&sb.st_mtime));

            printf(" %-4ld %s %s %-8ld %s ", sb.st_nlink, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, sb.st_size, time);
            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arr[i]);
            }
        }
    }
    return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void lsal(char *arrin[], int n)
{
    for (int w = 0; w < n; w++)
    {
        int file = access(arrin[w], F_OK);

        struct dirent *ptrdirentry;
        DIR *dir = opendir(arrin[w]);
        int count = 0;

        if (file == 0 && dir == NULL)
        {
            // printf("%s\n", arrin[w]);
            int total = 0;
            struct stat sb;
            if (lstat(arrin[w], &sb) == -1)
            {
                perror("lstat");
                exit(EXIT_FAILURE);
            }

            total += (sb.st_blocks / 2);
            printf("total %d\n", total);

            if (S_ISDIR(sb.st_mode))
            {
                printf("d");
            }
            else
            {
                printf("-");
            }
            /////////////////////////////////////////////////////////////////////////
            if (sb.st_mode & S_IRUSR)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }
            ///////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IRGRP)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWGRP)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXGRP)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }

            //////////////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IROTH)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWOTH)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXOTH)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }
            char time[20];
            strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&sb.st_mtime));

            printf(" %-4ld %s %s %-8ld %s ", sb.st_nlink, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, sb.st_size, time);

            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arrin[w]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arrin[w]);
            }

            continue;
        }

        if (file != 0 && dir == NULL)
        {
            printf("No such file or directory\n");
            continue;
        }

        while ((ptrdirentry = readdir(dir)) != NULL)
        {
            count++;
        }

        char *arr[count];

        for (int i = 0; i < count; i++)
        {
            arr[i] = (char *)malloc(sizeof(char) * 100);
        }

        rewinddir(dir);

        if (dir == NULL)
        {
            printf("Could not open current directory\n");
            return;
        }

        for (int i = 0; i < count; i)
        {
            ptrdirentry = readdir(dir);
            if (ptrdirentry->d_name == NULL)
            {
                printf("NULL reached\n");
                break;
            }
            // printf("%s\n", ptrdirentry->d_name);
            strcpy(arr[i], ptrdirentry->d_name);
            i++;
        }

        closedir(dir);
        stringsort(arr, count);

        int total = 0;

        for (int i = 0; i < count; i++)
        {
            struct stat sb;
            char temp[400];
            strcpy(temp, arrin[w]);
            temp[strlen(arrin[w])] = '/';
            temp[(strlen(arrin[w]) + 1)] = '\0';
            strcat(temp, arr[i]);
            if (lstat(temp, &sb) == -1)
            {
                perror("lstat");
                exit(EXIT_FAILURE);
            }

            total += (sb.st_blocks / 2);
        }
        printf("total %d\n", total);

        for (int i = 0; i < count; i++)
        {
            struct stat sb;

            char temp[400];
            strcpy(temp, arrin[w]);
            temp[strlen(arrin[w])] = '/';
            temp[(strlen(arrin[w]) + 1)] = '\0';
            strcat(temp, arr[i]);

            if (lstat(temp, &sb) == -1)
            {
                perror("lstat");
                exit(EXIT_FAILURE);
            }

            if (S_ISDIR(sb.st_mode))
            {
                printf("d");
            }
            else
            {
                printf("-");
            }
            /////////////////////////////////////////////////////////////////////////
            if (sb.st_mode & S_IRUSR)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXUSR)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }
            ///////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IRGRP)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWGRP)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXGRP)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }

            //////////////////////////////////////////////////////////////////////////////////////////////

            if (sb.st_mode & S_IROTH)
            {
                printf("r");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IWOTH)
            {
                printf("w");
            }
            else
            {
                printf("-");
            }

            if (sb.st_mode & S_IXOTH)
            {
                printf("x");
            }
            else
            {
                printf("-");
            }
            char time[20];
            strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&sb.st_mtime));

            printf(" %-4ld %s %s %-8ld %s ", sb.st_nlink, getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name, sb.st_size, time);
            if (S_ISDIR(sb.st_mode))
            {
                printf(BLUE);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else if (sb.st_mode & S_IXUSR)
            {
                printf(GREEN);
                printf("%s\n", arr[i]);
                printf(RESET);
            }
            else
            {
                printf("%s\n", arr[i]);
            }
        }
    }
    return;
}