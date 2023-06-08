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
#include "temp.h"
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

char histstring[10000], temphiststring1[10000];
time_t t;
pid_t pid;
char fgcommandgoingon[100];

int fggoingon;

void ctrlchandler()
{
    if (fggoingon == 0)
    {
        return;
    }
    else
    {
        kill(pid, SIGKILL);
        fggoingon = 0;
        return;
    }
}

void ctrlzhandler()
{
    if (fggoingon == 0)
    {
        return;
    }
    else
    {
        bgadd(pid, fgcommandgoingon);
        fggoingon = 0;
        kill(pid, SIGTSTP);
        return;
    }
}

int main()
{
    signal(SIGINT, ctrlchandler);
    signal(SIGTSTP, ctrlzhandler);

    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    char basedir[PATH_MAX], currdir[PATH_MAX], prevdir[PATH_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);

    getcwd(basedir, PATH_MAX);
    int ogdirlen = strlen(basedir);

    while (1) // loop for shell prompt
    {
        // shell prompt code
        getcwd(currdir, PATH_MAX);
        int newdirlen = strlen(currdir);

        signal(SIGCHLD, bgender);

        fggoingon = 0;

        prompt(newdirlen, ogdirlen, t, username, hostname, currdir);

        // input code for shell prompt
        char inbuffer[10000];

        // signal(SIGCHLD, bgender);

        // fgets(inbuffer, 10000, stdin);
        // printf("Reached here\n");
        in(inbuffer, newdirlen, ogdirlen, t, username, hostname, currdir);
        printf("\n");
        // printf("\n%s\n", inbuffer);
        time_t start = time(NULL);
        // printf("%s\n", inbuffer);
        // inbuffer[strlen(inbuffer) - 1] = '\0';
        fggoingon = 1;

        // printf("%s", inbuffer);

        strcpy(histstring, inbuffer);
        strcpy(temphiststring1, histstring);

        // printf("history creation\n")
        // printf("Hist=%s",histstring);
        char *temphiststring = strtok(temphiststring1, " \t\n");
        if (temphiststring != NULL)
        {
            createhistory(histstring, basedir);
        }

        // printf("history created\n");

        // breaking shell input into subinputs
        char *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
        char *subinput = strtok_r(inbuffer, ";", &ptr1);
        // printf("%s\n", subinput);
        //////////////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////////////////////
        while (subinput != NULL)
        {
            int andcount = 0;
            for (int i = 0; subinput[i] != '\0'; i++)
            {
                if (subinput[i] == '&')
                {
                    andcount++;
                }
            }
            char *subinput1 = (char *)malloc(sizeof(char) * (strlen(subinput) + 2));
            strcpy(subinput1, subinput);

            char *tempsubinput1 = strtok_r(subinput1, "&", &ptr3);
            // printf("%d\n",andcount);

            for (int i = 0; i < andcount; i++)
            {
                if (i != (andcount))
                {
                    // printf("Called %s\n", tempsubinput1);
                    bg(tempsubinput1);
                }
                tempsubinput1 = strtok_r(NULL, "&", &ptr3);
            }

            if (tempsubinput1 == NULL)
            {
                subinput = strtok_r(NULL, ";", &ptr1);
                getcwd(currdir, PATH_MAX);
                continue;
            }

            /////////////////////////////////////////////////////////////////
            int pipecount = 0;
            for (int i = 0; i < strlen(tempsubinput1); i++)
            {
                if (tempsubinput1[i] == '|')
                {
                    pipecount++;
                }
            }

            if (pipecount > 0)
            {
                execute(fgcommandgoingon, pid, subinput, currdir, basedir, prevdir, tempsubinput1, pipecount, ptr4, ptr3, ptr2, ptr1);

                subinput = strtok_r(NULL, ";", &ptr1);
                getcwd(currdir, PATH_MAX);
                continue;
            }

            /////////////////////////////////////////////////////////////////

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
                    subinput = strtok_r(NULL, ";", &ptr1);
                    getcwd(currdir, PATH_MAX);
                    continue;
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
                    continue;
                }

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
            }

            /////////////////////////////////////////////////////////////////////////////

            char *tempsubinput = (char *)malloc(sizeof(char) * (strlen(tempsubinput1) + 2));
            strcpy(tempsubinput, tempsubinput1);
            char *subcommand = strtok_r(tempsubinput, " \t\n", &ptr2);

            // printf("%s\n", subcommand);
            // printf("%s\n", tempsubinput1);

            // char subcommand[100];

            if (subcommand == NULL)
            {
                subinput = strtok_r(NULL, ";", &ptr1);
                getcwd(currdir, PATH_MAX);
                continue;
            }
            // printf("%s\n", tempsubinput1);
            // printf("%s\n", subcommand);
            /////////////////////////////////////////createhistory

            char *arrayexec[10];

            arrayexec[0] = (char *)malloc(sizeof(char) * 100);

            strcpy(arrayexec[0], subcommand);

            if (strcmp("pwd", subcommand) == 0)
            {
                printf("%s\n", currdir);
            }
            else if (strcmp("echo", subcommand) == 0)
            {
                while (subcommand != NULL)
                {
                    subcommand = strtok_r(NULL, " \t\n", &ptr2);
                    if (subcommand != NULL)
                    {
                        printf("%s ", subcommand);
                    }
                }
                printf("\n");
            }
            else if (strcmp("cd", subcommand) == 0)
            {
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                if (subcommand == NULL)
                {
                    if (chdir(basedir) != 0)
                    {
                        perror("cd failed:");
                    }
                    else
                    {
                        strcpy(prevdir, currdir);
                    }
                }
                else
                {
                    char *checktemp = strtok_r(NULL, " \t\n", &ptr2);

                    if (checktemp != NULL)
                    {
                        printf("Error: Too many arguments\n");
                    }
                    else
                    {
                        if (strcmp(".", subcommand) == 0)
                        {
                            strcpy(prevdir, currdir);
                        }
                        else if (subcommand[0] == '~')
                        {
                            char temp0string[1000];
                            strcpy(temp0string, basedir);
                            strcat(temp0string, &(subcommand[1]));
                            if (chdir(temp0string) != 0)
                            {
                                perror("cd failed:");
                            }
                            else
                            {
                                strcpy(prevdir, currdir);
                            }
                        }
                        else if (strcmp("-", subcommand) == 0)
                        {
                            if (chdir(prevdir) != 0)
                            {
                                perror("cd failed:");
                            }
                            else
                            {
                                strcpy(prevdir, currdir);
                                getcwd(currdir, PATH_MAX);
                                printf("%s\n", currdir);
                            }
                        }
                        else if (strcmp("..", subcommand) == 0)
                        {
                            int temp;
                            char tempstr[1000];
                            for (int i = strlen(currdir) - 1; i >= 0; i--)
                            {
                                if (currdir[i] == '/')
                                {
                                    temp = i;
                                    break;
                                }
                            }
                            strncpy(tempstr, currdir, temp);
                            tempstr[temp] = '\0';
                            if (chdir(tempstr) != 0)
                            {
                                perror("cd failed:");
                            }
                            else
                            {
                                strcpy(prevdir, currdir);
                            }
                        }

                        else
                        {
                            if (chdir(subcommand) != 0)
                            {
                                perror("cd failed:");
                            }
                            else
                            {
                                strcpy(prevdir, currdir);
                            }
                        }
                    }
                }
            }

            else if (strcmp("ls", subcommand) == 0)
            {
                progls(subcommand, &ptr2, basedir);
            }
            else if (strcmp("discover", subcommand) == 0)
            {
                progdisc(subcommand, &ptr2, basedir, currdir);
            }
            else if (strcmp("history", subcommand) == 0)
            {
                history(basedir);
            }
            else if (strcmp("pinfo", subcommand) == 0)
            {
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                progpid(subcommand, basedir);
            }
            else if (strcmp("jobs", subcommand) == 0)
            {
                int n;
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                if (subcommand == NULL)
                {
                    n = 0;
                }
                else if (strcmp("-r", subcommand) == 0)
                {
                    n = 2;
                }
                else if (strcmp("-s", subcommand) == 0)
                {
                    n = 1;
                }
                else
                {
                    n = 0;
                }

                jobs(n);
            }

            else if (strcmp("sig", subcommand) == 0)
            {
                int a, b;
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                a = atoi(subcommand);
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                b = atoi(subcommand);

                sig(a, b);
            }
            else if (strcmp("fg", subcommand) == 0)
            {
                int a;
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                a = atoi(subcommand);

                fgkill(a);
            }
            else if (strcmp("bg", subcommand) == 0)
            {
                int a;
                subcommand = strtok_r(NULL, " \t\n", &ptr2);
                a = atoi(subcommand);

                bgkill(a);
            }

            else if (strcmp("exit", subcommand) == 0)
            {
                return 0;
            }

            else
            {

                strcpy(fgcommandgoingon, subcommand);
                // printf("Reached Here\n");
                pid = fork();

                if (pid == -1)
                {
                    perror("Child Not formed\n");
                    continue;
                }
                else if (pid == 0)
                {
                    subcommand = strtok_r(NULL, " \t\n", &ptr2);
                    setpgid(0, 0);
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
                    execvp(arrayexec[0], arrayexec);
                    // printf("%s %s\n", arrayexec[0], arrayexec[1]);
                }
                else
                {
                    int stat;
                    waitpid(pid, &stat, WUNTRACED);
                }

                // printf("Error: NO such command found\n");
            }

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

            subinput = strtok_r(NULL, ";", &ptr1);
            // if(subinput!=NULL)
            //     printf("%s\n", subinput);
            // else
            //     printf("NULL\n");
            getcwd(currdir, PATH_MAX);
        }

        // end = clock();
        // t = (clock() - t) / CLOCKS_PER_SEC;
        // printf("%ld", t);
        time_t end = time(NULL);
        // printf("%ld\n",end-start);
        t = end - start;
    }
}