// shell.h
/////////////////////////////////////////////
//      makefile for os mini-project     ///
//      made with <3 by Gopal Kataria   ///
/////////////////////////////////////////

#ifndef SHELL_H
#define SHELL_H

// enable posix and x/open features
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <stdio.h>    // standard input/output functions
#include <stdlib.h>   // for malloc, free, exit
#include <string.h>   // for strcpy, strlen, strtok
#include <unistd.h>   // for fork, execvp, chdir, getcwd
#include <sys/wait.h> // for waitpid
#include <errno.h>    // for errno and perror
#include <pwd.h>      // for getpwuid, getpwnam
#include <limits.h>   // for path_max, host_name_max
#include <signal.h>   // for signal handling
#include <fcntl.h>    // for open, dup2, close
#include <ctype.h>    // for isspace





// SETTINGS
// global variables for shell state
// maximum length of a command input
#define MAX_COMMAND_LENGTH 1024
// maximum number of arguments for a command
#define MAX_ARGS 64
// maximum number of commands to store in history
#define MAX_HISTORY_SIZE 20
// name of the history file, stored in the user's home directory
#define HISTORY_FILE_NAME "/.bropesh_history"




// pointer to the user's home directory path
extern char *home_dir;
// pointer to the previous working directory path, used for 'cd -'
extern char *prev_dir;
// process id of the currently running foreground child process
extern pid_t foreground_child_pid;
// array to store history commands
extern char *history_commands[MAX_HISTORY_SIZE];
// current count of commands in history (not necessarily actual stored, but total entered)
extern int history_count;
// file pointer for the history file
extern FILE *history_file_ptr;

// utility functions
// trims leading and trailing whitespace from a string
char *trim_whitespace(char *str);
// tokenizes the input string into arguments, handles redirection and background flag
char **tokenize_input(char *input, int *num_args, int *is_background, char **input_file, char **output_file);
// frees memory allocated for tokens
void free_tokens(char **tokens);

// prompt functions
// displays the shell prompt
void display_prompt();

// built-in command functions
// checks if a command is built-in and executes it, returns 1 if handled, 0 otherwise
int execute_builtin_command(char **args);
// implements the 'echo' command
void builtin_echo(char **args);
// implements the 'pwd' command
void builtin_pwd();
// implements the 'cd' command
int builtin_cd(char **args);
// implements the HELP command
int builtin_help();
// implements the 'history' command
void builtin_history();
// handles ctrl+d (end of file) signal, performs cleanup and exits
void handle_ctrl_d();

// process management functions
// executes an external command in foreground or background with optional redirection
void execute_external_command(char **args, int is_background, char *input_file, char *output_file);

// history management functions
// loads command history from the history file into memory
void load_history();
// saves current command history from memory to the history file
void save_history();
// adds a new command to the in-memory history
void add_to_history(char *command);

// signal handling functions
// sets up custom signal handlers for sigint (ctrl+c) and sigchld (child process status change)
void setup_signal_handlers();
// signal handler for sigint (ctrl+c)
void handle_sigint(int signum);
// signal handler for sigchld (child process status change), reaps background processes
void handle_sigchld(int signum);

#endif // SHELL_H
