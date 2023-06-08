#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "ls.h"
#include "stringsort.h"
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
#include "prompt.h"
#include "ls.h"
#include "stringsort.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void die(const char *s)
{
    // perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */

int finder(char arrin[], char input[], char *output[])
{
    int file = access(arrin, F_OK);

    struct dirent *ptrdirentry;
    DIR *dir = opendir(arrin);
    int count = 0;

    if (dir == NULL)
    {
        printf("No such file or directory\n");
        return 0;
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
        return 0;
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

    int counter = 0;
    for (int i = 0; i < count; i++)
    {
        //     struct stat sb;
        //     char temp[10000];
        //     strcpy(temp, arrin);
        //     strcat(temp, "./");
        //     strcat(temp, arr[i]);

        if (strncmp(input, arr[i], strlen(input)) == 0)
        {
            struct stat sb;
            char tempstr[1000];
            strcpy(tempstr, "./");
            strcat(tempstr, arr[i]);
            lstat(tempstr, &sb);

            if (S_ISDIR(sb.st_mode))
            {
                int len = strlen(arr[i]);
                strcat(arr[i], "/");
                arr[i][(len + 1)] = '\0';
            }

            strcpy(output[counter], arr[i]);
            counter++;
        }
    }

    // for (int i = 0; i < counter; i++)
    // {
    //     printf("%s\n", output[i]);
    // }

    return counter;
}

void tab(char *inp)
{
    // int pos = 0;
    // for (int i = (strlen(inp) - 1); i >= 0; i--)
    // {
    //     if (inp[i] == '/')
    //     {
    //         pos = i;
    //         break;
    //     }
    // }

    char dir[1000], temp[1000], *output[1000];
    for (int i = 0; i < 1000; i++)
    {
        output[i] = (char *)malloc(sizeof(char) * 100);
    }

    char same[100];

    // strncpy(temp, inp, (pos));
    // strcpy(dir, "./");
    // strcat(dir, temp);

    int ret = finder(".", inp, output);

    if (ret == 0)
    {
        perror("Error: Directory not found");
        return;
    }
    else if (ret == 1)
    {
        strcpy(inp, (output[0]));
        int file = access(output[0], F_OK);
        if (file == 0)
        {
            strcat(inp, " ");
        }

        return;
    }

    else
    {
        int index = 0, flag = 0;
        for (int i = 1; i < 1000; i++)
        {
            for (int j = 0; j < ret - 1; j++)
            {
                if (strncmp(output[j], output[j + 1], i) < 0)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                break;
            }
            index = i;
        }
        // printf("i==%d\n", index);
        strncpy(inp, (output[0]), (index));
        inp[index] = '\0';
        if (inp[strlen(inp)-1]=='/')
        {
            inp[strlen(inp)-1]='\0';
        }
        
        // printf("input==%s\n",inp);
        for (int i = 0; i < ret; i++)
        {
            printf("%s\n", output[i]);
        }
    }
}

void in(char inbuffer[], int newdirlen, int ogdirlen, long int t, char username[], char hostname[], char currdir[])
{
    char *inp = malloc(sizeof(char) * 100);
    char c;
    // while (1)
    // {
    setbuf(stdout, NULL);
    enableRawMode();
    // printf("Prompt>");
    memset(inp, '\0', 100);
    int pt = 0;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10)
                break;
            else if (c == 27)
            {
                char buf[3];
                buf[2] = 0;
                if (read(STDIN_FILENO, buf, 2) == 2)
                { // length of escape code
                    printf("\rarrow key: %s", buf);
                }
            }
            else if (c == 127)
            { // backspace
                if (pt > 0)
                {
                    if (inp[pt - 1] == 9)
                    {
                        for (int i = 0; i < 7; i++)
                        {
                            printf("\b");
                        }
                    }
                    inp[--pt] = '\0';
                    printf("\b \b");
                }
            }
            else if (c == 9)
            { // TAB character
                // printf("\nTab Taken\n");
                printf("\n");
                char tempstring[1000];

                int index = 0;
                for (int i = pt - 1; i >= 0; i--)
                {
                    if (inp[i] == ' ')
                    {
                        index = i;
                        break;
                    }
                }
                if (index == 0)
                {
                    char *string = ".";
                    lsa(&string, 1);
                    prompt(newdirlen, ogdirlen, t, username, hostname, currdir);
                }

                else if (index != 0)
                {
                    strcpy(tempstring, (&inp[index + 1]));
                    inp[index] = '\0';

                    // printf("input==%s\n", inp);
                    // char *ptr;

                    // char *tempstrstring = strtok_r(tempstring, " ", &ptr);
                    // strcpy(inp, tempstrstring);
                    // tempstrstring = strtok_r(NULL, " ", &ptr);

                    // printf("%s\n", tempstring);
                    int len = strlen(inp);
                    tab(tempstring);
                    // printf("new == %s\n", tempstring);
                    strcat(inp, " ");
                    strcat(inp, tempstring);
                    inp[(len + 1 + strlen(tempstring))] = '\0';

                    // printf("input==%s\n", inp);

                    prompt(newdirlen, ogdirlen, t, username, hostname, currdir);
                    for (int i = 0; i < strlen(inp); i++)
                    {
                        printf("%c", inp[i]);
                    }
                    pt = strlen(inp);
                }

                // inp[pt + 1] = '\0';
                // for (int i = 0; i < 8; i++)
                // { // TABS should be 8 spaces
                //     printf(" ");
                // }
            }
            else if (c == 4)
            {
                exit(0);
            }
            else
            {
                printf("%d\n", c);
            }
        }
        else
        {
            inp[pt++] = c;
            printf("%c", c);
        }
    }
    disableRawMode();

    strcpy(inbuffer, inp);
    // printf("\nInput Read: [%s]\n", inp);
    // }
    return;
}