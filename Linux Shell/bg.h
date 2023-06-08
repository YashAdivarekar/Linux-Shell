#ifndef BGDEF
#define BGDEF

void bg(char tempsubinput1[]);
void bgender();
void jobs(int n);
void sig(int index, int sigcode);
void bgkill(int index);
void fgkill(int index);
void bgadd(int pid, char fgcommand[]);

#endif