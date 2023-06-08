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
#include "discover.h"
#include "stringsort.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void progdisc(char *subcommand, char **ptr2, char *basedir, char *currdir)
{
    char targetdir[100], targetfile[100], temp[100];
    int flags = 0, dircount = 0, filecount = 0;

    while (1)
    {
        subcommand = strtok_r(NULL, " \t\n", ptr2);
        if (subcommand == NULL)
        {
            break;
        }
        if (strcmp("-d", subcommand) == 0)
        {
            if (flags == 0)
            {
                flags = 1;
            }
            else if (flags == 2 || flags == 3)
            {
                flags = 3;
            }
        }
        else if (strcmp("-f", subcommand) == 0)
        {
            if (flags == 0)
            {
                flags = 2;
            }
            else if (flags == 1 || flags == 3)
            {
                flags = 3;
            }
        }
        else
        {
            if (subcommand[0] == '"')
            {
                char temp[100];

                for (int i = 1; i < (strlen(subcommand) - 1); i++)
                {
                    temp[i - 1] = subcommand[i];
                    if (i == (strlen(subcommand) - 2))
                    {
                        temp[i] = '\0';
                    }
                }
                strcpy(targetfile, temp);
                // printf("%s\n",targetfile);
                filecount++;
            }

            else if (strcmp("~", subcommand) == 0)
            {
                strcpy(targetdir, basedir);
                dircount++;
            }
            else
            {
                strcpy(targetdir, subcommand);
                dircount++;
            }
        }
    }

    if (dircount == 0)
    {
        strcpy(targetdir, ".");
        dircount++;
    }

    if (filecount == 0)
    {
        discoverdf(targetdir, flags);
    }
    else
    {
        discoverfile(targetdir, flags, targetfile);
    }
}

void discoverdf(char arrin[], int flags)
{
    int file = access(arrin, F_OK);

    struct dirent *ptrdirentry;
    DIR *dir = opendir(arrin);
    int count = 0;

    if (file == 0 && dir == NULL)
    {
        if (flags == 0 || flags == 3 || flags == 2)
        {
            printf("%s\n", arrin);
        }
        return;
    }

    if (file != 0 && dir == NULL)
    {
        printf("No such file or directory\n");
        return;
    }

    if (flags == 1 || flags == 3 || flags == 0)
    {
        printf("%s\n", arrin);
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

    // for (int i = 0; i < count; i++)
    // {
    //     printf("%s\n",arr[i]);
    // }

    // printf("=====================\n");

    stringsort(arr, count);

    for (int i = 0; i < count; i++)
    {
        char temp[1000];
        strcpy(temp, arrin);
        temp[strlen(arrin)] = '/';
        temp[(strlen(arrin) + 1)] = '\0';
        strcat(temp, arr[i]);
        discoverdf(temp, flags);
    }

    // for (int i = 0; i < count; i++)
    // {
    //     printf("%s\n", arr[i]);
    // }

    return;
}

void discoverfile(char arrin[], int flags, char targetfile[])
{
    int file = access(arrin, F_OK);

    struct dirent *ptrdirentry;
    DIR *dir = opendir(arrin);
    int count = 0;

    if (file == 0 && dir == NULL)
    {
        char tempstr[100];
        int point;
        for (int i = (strlen(arrin) - 1); i >= 0; i--)
        {
            if (arrin[i] == '/')
            {
                count = i;
                break;
            }
        }

        for (int i = count + 1, j = 0; i <= strlen(arrin); i++, j++)
        {
            tempstr[j] = arrin[i];
        }

        if (strcmp(tempstr, targetfile) == 0)
        {
            printf("%s\n", arrin);
        }
        return;
    }

    if (file != 0 && dir == NULL)
    {
        printf("No such file or directory\n");
        return;
    }

    // if (flags == 1 || flags == 3 || flags == 0)
    // {
    //     printf("%s\n", arrin);
    // }
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

    // for (int i = 0; i < count; i++)
    // {
    //     printf("%s\n",arr[i]);
    // }

    // printf("=====================\n");

    stringsort(arr, count);

    for (int i = 0; i < count; i++)
    {
        char temp[1000];
        strcpy(temp, arrin);
        temp[strlen(arrin)] = '/';
        temp[(strlen(arrin) + 1)] = '\0';
        strcat(temp, arr[i]);
        discoverfile(temp, flags, targetfile);
    }

    // for (int i = 0; i < count; i++)
    // {
    //     printf("%s\n", arr[i]);
    // }

    return;
}