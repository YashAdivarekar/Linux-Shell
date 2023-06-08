#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include "ls.h"
#include "discover.h"
#include "history.h"
#include "bg.h"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

int pids[100] = {-2};
char *bgcommands[100];

int flag = 0;

// void ctrlchandle()
// {
//     printf("over\n");
//     kill(FGPROCESS_ID, SIGINT);
// }
void bgadd(int pid, char fgcommand[])
{
    if (flag == 0)
    {
        flag = 1;
        for (int i = 0; i < 100; i++)
        {
            pids[i] = -2;
        }
    }
    int num;
    for (int i = 0; i < 100; i++)
    {
        if (pids[i] == -2)
        {
            pids[i] = pid;
            if (fgcommand == NULL)
            {
                printf(" NULL HAI\n");
            }

            // printf("%s\n",arrayexec[0]);
            bgcommands[i] = (char *)malloc(sizeof(char) * 100);
            strcpy(bgcommands[i], fgcommand);
            num = i;
            break;
        }
    }
    printf("[%d] %d\n", num, pid);
}

void bg(char tempsubinput1[])
{
    if (flag == 0)
    {
        flag = 1;
        for (int i = 0; i < 100; i++)
        {
            pids[i] = -2;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    char *ptr4;
    char *redirectstring1 = (char *)malloc(strlen(tempsubinput1) * sizeof(char) + 2);
    strcpy(redirectstring1, tempsubinput1);
    char *redirectstring = strtok_r(redirectstring1, " \t\n", &ptr4);

    char *commandredir = NULL, inputfilename[100], outputfilename[100], appendfilename[100];
    int inputred = 0, outputred = 0, appendred = 0, commandtaken = 0, outputter = 0;
    int stopreadafterred = 0;
    while (redirectstring != NULL)
    {
        // printf("%s\n", redirectstring);
        if (strcmp(redirectstring, "<") == 0)
        {
            stopreadafterred = 1;
            inputred = 1;
            redirectstring = strtok_r(NULL, " \t\n", &ptr4);
            strcpy(inputfilename, redirectstring);
        }
        else if (strcmp(redirectstring, ">") == 0)
        {
            stopreadafterred = 1;
            outputred = 1;
            redirectstring = strtok_r(NULL, " \t\n", &ptr4);
            strcpy(outputfilename, redirectstring);
        }
        else if (strcmp(redirectstring, ">>") == 0)
        {
            stopreadafterred = 1;
            appendred = 1;
            redirectstring = strtok_r(NULL, " \t\n", &ptr4);
            strcpy(appendfilename, redirectstring);
        }
        else
        {
            if (stopreadafterred == 0)
            {
                if (commandtaken == 0)
                {
                    commandtaken = 1;
                    commandredir = (char *)malloc(sizeof(char) * 500);
                    strcpy(commandredir, redirectstring);
                }
                else
                {
                    strcat(commandredir, " ");
                    strcat(commandredir, redirectstring);
                }
            }
            // printf("Loop : %s\n", commandredir);
            redirectstring = strtok_r(NULL, " \t\n", &ptr4);
        }
    }

    int inputto = 0, outputto = 0;
    int stdinclone, stdoutclone;
    if (inputred == 1 || outputred == 1 || appendred == 1)
    {
        if (commandredir == NULL)
        {
            return;
        }
        strcpy(tempsubinput1, commandredir);
        // printf("Here:%s\n", commandredir);

        if (inputred == 1)
        {
            inputto = open(inputfilename, O_RDONLY);
        }
        if (outputred == 1)
        {
            outputter = 1;
            outputto = open(outputfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (appendred == 1)
        {
            outputter = 2;
            outputto = open(appendfilename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }

        if (inputto < 0 || outputto < 0)
        {
            perror("Could not open file\n");
            return;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////

    // printf("reached here %s\n", tempsubinput1);
    char *ptr1;
    char *tempsubinput = (char *)malloc(sizeof(char) * (strlen(tempsubinput1) + 2));
    strcpy(tempsubinput, tempsubinput1);
    char *subcommand = strtok_r(tempsubinput, " \t\n", &ptr1);

    // char subcommand[100];
    char *arrayexec[10];

    if (subcommand == NULL)
    {
        return;
    }
    pid_t pid = fork();
    setpgrp();

    arrayexec[0] = (char *)malloc(sizeof(char) * 100);

    strcpy(arrayexec[0], subcommand);

    subcommand = strtok_r(NULL, " \t\n", &ptr1);
    if (subcommand == NULL)
    {
        arrayexec[1] = NULL;
    }
    else
    {
        arrayexec[1] = (char *)malloc(sizeof(char) * 100);
        strcpy(arrayexec[1], subcommand);
        arrayexec[2] = (char *)malloc(sizeof(char) * 100);
        arrayexec[2] = NULL;
    }
    if (pid == -1)
    {
        perror("Child not formed\n");
        return;
    }
    else if (pid == 0)
    {

        // printf("bg for %s %s\n", arrayexec[0], arrayexec[1]);

        ////////////////////////////////////////////////////////
        if (inputto)
        {
            stdinclone = dup(0);
            dup2(inputto, 0);
        }
        if (outputto)
        {
            stdoutclone = dup(1);
            dup2(outputto, 1);
        }
        /////////////////////////////////////////////////////

        execvp(arrayexec[0], arrayexec);

        ////////////////////////////////////////////////////
        if (inputto)
        {
            close(inputto);
            dup2(stdinclone, 0);
            close(stdinclone);
        }
        if (outputto)
        {
            close(outputto);
            dup2(stdoutclone, 1);
            close(stdoutclone);
        }
        //////////////////////////////////////////////////////////////
        exit(0);
    }
    else
    {
        printf("pid=%d\n", pid);
        int num;
        for (int i = 0; i < 100; i++)
        {
            if (pids[i] == -2)
            {
                pids[i] = pid;
                if (arrayexec[0] == NULL)
                {
                    printf(" NULL HAI\n");
                }

                // printf("%s\n",arrayexec[0]);
                bgcommands[i] = (char *)malloc(sizeof(char) * 100);
                strcpy(bgcommands[i], arrayexec[0]);
                num = i;
                break;
            }
        }
        printf("[%d] %d\n", num, pid);

        //     int stat;
        //     pid_t returnpid = waitpid(pid, &stat, WNOHANG);

        //     if (WIFEXITED(stat))
        //     {
        //         printf("%s with pid=%d exited normally\n", arrayexec[0], returnpid);
        //     }
        //     else
        //     {
        //         printf("%s with pid=%d exited normally\n", arrayexec[0], returnpid);
        //     }
    }
}

void bgender()
{
    int status;
    pid_t pidnew = waitpid(-1, &status, WNOHANG);
    int index;

    for (int i = 0; i < 100; i++)
    {
        if (pids[i] == pidnew)
        {
            pids[i] = -2;
            index = i;
            if (WIFEXITED(status))
            {
                printf("\n%s with pid=%d exited normally\n", bgcommands[index], pidnew);
            }
            else
            {
                printf("\n%s with pid=%d exited abnormally\n>", bgcommands[index], pidnew);
            }
            // free(bgcommands[index]);
            fflush(stdout);
            break;
        }
    }
}

void jobs(int n)
{
    if (flag == 0)
    {
        flag = 1;
        for (int i = 0; i < 100; i++)
        {
            pids[i] = -2;
        }
    }
    char *jobcommands[100];
    int jobind[100];
    char filename[2000];
    char status;
    int count = 0;

    for (int i = 0; i < 100; i++)
    {
        if (pids[i] != -2)
        {
            jobcommands[count] = bgcommands[i];
            jobind[count] = i;
            count++;
        }
    }

    for (int i = count; i >= 0; i--)
    {
        for (int j = 0; j < i - 1; j++)
        {
            // printf("%d %d\n", j, j + 1);
            if (strcmp(jobcommands[j], jobcommands[j + 1]) > 0)
            {
                char *temp;
                int tempind;
                temp = jobcommands[j + 1];
                jobcommands[j + 1] = jobcommands[j];
                jobcommands[j] = temp;
                tempind = jobind[j + 1];
                jobind[j + 1] = jobind[j];
                jobind[j] = tempind;
            }
        }
    }

    // for (int i = 0; i < count; i++)
    // {
    //     printf("%d %s\n", jobind[i], bgcommands[jobind[i]]);
    // }

    for (int i = 0; i < count; i++)
    {
        sprintf(filename, "/proc/%d/stat", pids[jobind[i]]);

        FILE *fp = fopen(filename, "r");
        if (fp != NULL)
        {
            int tempnum;
            char tempchar[100];
            fscanf(fp, "%d %s %c", &tempnum, tempchar, &status);

            if (status == 'T' && (n == 0 || n == 1))
            {
                printf("[%d] %s %s [%d]\n", jobind[i], "Stopped", bgcommands[jobind[i]], pids[jobind[i]]);
            }
            else if (status != 'T' && (n == 0 || n == 2))
            {
                printf("[%d] %s %s [%d]\n", jobind[i], "Running", bgcommands[jobind[i]], pids[jobind[i]]);
            }
        }
        else
        {
            perror("Error");
            return;
        }
        fclose(fp);
    }
    return;
}

void sig(int index, int sigcode)
{
    printf("%d %d\n", index, sigcode);
    int ret = kill(pids[index], sigcode);
    // pids[index] = -2;
    if (ret < 0)
    {
        perror("Error");
        return;
    }
    return;
}

void bgkill(int index)
{
    int ret = kill(pids[index], SIGCONT);
    if (ret < 0)
    {
        perror("Error");
        return;
    }
    return;
}

void fgkill(int index)
{
    if (pids[index] == -2)
    {
        perror("Error");
        return;
    }
    printf("pid=%d\n", pids[index]);
    int status;
    setpgid(pids[index], getpgid(0));
    int stat;

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    kill(pids[index], SIGCONT);

    waitpid(pids[index], &stat, WUNTRACED);

    pids[index] = -2;
    // printf("reached here\n");
    tcsetpgrp(0, pids[index]);

    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    if (WIFSTOPPED(status))
    {
        bgadd(pids[index], bgcommands[index]);
        return;
    }

    // if (WIFEXITED(status))
    // {
    //     printf("\n%s with pid=%d exited normally\n", bgcommands[index], pids[index]);
    // }
    // else
    // {
    //     printf("\n%s with pid=%d exited abnormally\n>", bgcommands[index], pids[index]);
    // }
    // // free(bgcommands[index]);
    // fflush(stdout);

    return;
}