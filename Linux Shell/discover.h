#ifndef DISCDEF
#define DISCDEF

void progdisc(char *subcommand, char **ptr2, char *basedir,char * currdir);

void discoverdf(char dir[],int flags);

void discoverfile(char arrin[], int flags,char targetfile[]);

#endif