#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include "stringsort.h"

int cmpfunc(const void *a, const void *b)
{
    return strcmp(*(const char**)a,*(const char**)b);   
}

void stringsort(char *arr[], int n)
{
    qsort(arr, n, sizeof(char *), cmpfunc);
}
