1. bg.c corresponds to code regarding background processes
2. discover.c corresponds to code regarding discover process
3. history.c correseponds to code for  creating and showing history
4. ls.c corresponds to code for ls command including all required flags 
5. pid.c corresponds to code for pinfo 
6. stringsort.c corresponds to code which sorts strings in lexicographical order

1. enter exit to quit the terminal
2. pwd prints the absolute path from the root directory of the sysytem.
3. echo prints the characters in front of it ommiting extra spaces and tabs
4. cd ~ changes the current directory to the directory in which the shell has been invoked
5. cd . remains in the same directory
6. cd .. changes to the parent directory of the current directory
7. cd - changes to the previous directory
8. if multiple foreground processes are input in the terminal, then the termainal will give an input
   prompt after completing all process
9. time for completion will be the combined time required for all the forground processes to complete
10. after a background process is completed, the program will notify that the process has finished. To continue with inputting ENTER has to be pressed before inputing the next command
11. Background process with parameters having 2 words are considered.
12. for discover command, there iss no '-df' flag. for displaying both files and directories, no flag is required or ' -d -f' can be used.

/////////////////////////////////////////////////////////////////////////////////////////////////////

13. i/o redirection has been added.

14. the input to input redirection should be space separated. This is valid for piping with i/o    redirection as well. Input and output file names should be valid
   For example:-
      cat < inputfile.txt > outputfile.txt

/////////////////////////////////////////////////////////////////////////////////////////////////////
15. Pipes have been implemented.Pipes have to be space separated without any empty space between pipes. This is valid for piping with i/o redirection as well. Pipes not handled for background processes.

16. jobs can be used indiidually or with either -r or -s flags only.

17. sig should have two parameters along with it 
   For example:-
      sig 2 9

18. fg should have one parameter with it.
   For example:-
      fg 0
      
19. bg should have one parameter with it.
   For example:- 
      bg 0

20. CTRL-Z pushes any currently running foreground job into the background, and changes its state from ‘running’ to ‘stopped’.

21. CTRL-C interrupts any currently running foreground job, by sending it the SIGINT signal.

22. CTRL-D logs out of shell.

23. Pressing TAB when no input is provided prints all files and directories in the current directory also indicating their type. BLUE for directories, GREEN for executables and WHITE for files.

24. After giving a command, there should be a space first before another TAB is pressed.
   For example:-
      Prompt>cd<space><TAB>
      NOT :- Prompt>cd<TAB>

25. Only files and directories in the current directory are implemented for auto-complete.


