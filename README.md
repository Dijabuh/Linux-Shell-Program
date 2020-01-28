# Shell
COP4610 Project 1: Implementing a Shell

## How to compile with makefile
- Use command *"make"* to build program *"shell"*

### Contents of Tar and Files
##### README
- The project team members' names and division of labor
- List who worked on each part of the project
- The contents of your tar archive and a brief description of each file
- How to compile your executables using your Makefile
- Known bugs and unfinished portions of the project
- Special considerations or anything I should know when grading your solution
- Any completed extra credit must be documented in the README to receive credit
##### makefile
- Compiles c shell program into *"shell"* by default
- Uses *"make clean"* to remove executables and object files
##### shell.c
- Is the c shell program driver
##### background.c
- Code for handling background execution queue
##### background.h
- Header file for background.c
##### builtins.c
- Code for the 4 builtin commands
##### builtins.h
- Header file for builtins.c
##### Execution.c
- Code for executing commands
##### Execution.h
- Header file for Execution.c
##### file_res.c
- Code for resolving pathnames
##### file_res.h
- Header file for file_res.c
##### parser.c
- Parser for program
##### parser.h
- Header file for parser.c
##### piping.c
- Code for executing piped commands
##### piping.h
- Header file for piping.c
##### redirection.c
- Code for executing io redirected commands
##### redirection.h
- Header file for redirection.c

### Group Members
- Garett Anderson
- Daniel Brown
##### Garett Division of Labor
- makefile
- gitignore
- readme (title, group members, this)
- external execution of commands
- IO redirection
- Makefile package and variables
##### Daniel Division of Labor
- Parsing
- Resolving Path Shortcuts
- User Prompt
