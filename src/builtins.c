// builtins.c
// implementation of shell built-in commands

#include "shell.h"
#include <limits.h> // for path_max
#include <stdio.h>

int execute_builtin_command(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        save_history();
        if (home_dir != NULL) free(home_dir);
        if (prev_dir != NULL) free(prev_dir);
        for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
            if (history_commands[i] != NULL) free(history_commands[i]);
        }
        if (history_file_ptr != NULL) fclose(history_file_ptr);
        exit(0);
    } else if (strcmp(args[0], "echo") == 0) {
        builtin_echo(args + 1);
        return 1;
    } else if (strcmp(args[0], "pwd") == 0) {
        if (args[1] != NULL) {
            fprintf(stderr, "bropesh: pwd: too many arguments\n");
        } else {
            builtin_pwd();
        }
        return 1;
    } else if (strcmp(args[0], "cd") == 0) {
        return builtin_cd(args);
    } else if (strcmp(args[0], "history") == 0) {
        if (args[1] != NULL) {
            fprintf(stderr, "bropesh: history: too many arguments\n");
        } else {
            builtin_history();
        }
        return 1;
    }
    return 0; // not a built-in command
}

void builtin_echo(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        printf("%s%s", args[i], (args[i+1] != NULL ? " " : "")); // print space between arguments
    }
    printf("\n");
}

void builtin_pwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("bropesh: pwd failed");
    }
}

// displays help information
void builtin_help() {
    printf("\n--- Bropesh Shell Help ---\n");
    printf(" built with <3 by Gopal Kataria 2023112006 \n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following commands are built-in:\n");
    printf("  cd [dir]    : Change directory (supports ~, .., -)\n");
    printf("  pwd         : Print current working directory\n");
    printf("  echo [arg]  : Display text\n");
    printf("  history     : Display last 20 commands\n");
    printf("  help        : Display this information\n");
    printf("  exit        : Exit the shell\n");
    printf("\n");
    printf("Supported Features:\n");
    printf("  - External commands (e.g., ls, grep)\n");
    printf("  - I/O Redirection (< input_file, > output_file)\n");
    printf("  - Background execution (end command with &)\n");
    printf("--------------------------\n\n");
}

int builtin_cd(char **args) {
    char old_cwd[PATH_MAX];
    if (getcwd(old_cwd, sizeof(old_cwd)) == NULL) {
        perror("bropesh: getcwd failed for old_cwd in cd");
        return 1; // indicate an error
    }

    // if argument or  "~" , change to home directory
    if (args[1] == NULL || strcmp(args[1], "~") == 0) {
        if (chdir(home_dir) != 0) {
            perror("bropesh: cd failed to change to home directory");
            return 1;
        }
    } else if (strcmp(args[1], "..") == 0) {
        // cd ..: go one level up in the directory structure
        if (chdir("..") != 0) {
            perror("bropesh: cd failed to go up one directory");
            return 1;
        }
    } else if (strcmp(args[1], "-") == 0) {
        // cd -: go to previous directory
        if (prev_dir == NULL) {
            fprintf(stderr, "bropesh: cd: no previous directory.\n");
            return 1;
        }
        char temp_prev[PATH_MAX];
        strncpy(temp_prev, prev_dir, PATH_MAX - 1);
        temp_prev[PATH_MAX - 1] = '\0';

        if (chdir(prev_dir) != 0) {
            perror("bropesh: cd failed to change to previous directory");
            return 1;
        }
        printf("%s\n", temp_prev);
    } else if (args[2] != NULL) {
        fprintf(stderr, "bropesh: cd: too many arguments.\n");
        return 1;
    } else {
        // cd [directory_path]: change to specified directory
        if (chdir(args[1]) != 0) {
            perror("bropesh: cd failed to change directory");
            return 1;
        }
    }

    if (prev_dir != NULL) {
        free(prev_dir);
    }
    prev_dir = strdup(old_cwd);
    if (prev_dir == NULL) {
        perror("bropesh: strdup failed for prev_dir update");
    }
    return 1;
}

void builtin_history() {
    if (history_count == 0) {
        printf("no commands in history.\n");
        return;
    }

    // calculate the actual number of commands currently stored (max MAX_HISTORY_SIZE)
    int actual_stored_count = (history_count < MAX_HISTORY_SIZE) ? history_count : MAX_HISTORY_SIZE;

    int num_to_display = (actual_stored_count < 10) ? actual_stored_count : 10;
    int start_index_in_buffer = (history_count - num_to_display + MAX_HISTORY_SIZE) % MAX_HISTORY_SIZE;

    for (int i = 0; i < num_to_display; i++) {
        int current_idx = (start_index_in_buffer + i) % MAX_HISTORY_SIZE;
        if (history_commands[current_idx] != NULL) { // not doing a NULL check can cost lives :)
            printf(" %d  %s\n", i + 1, history_commands[current_idx]);
        }
    }
}
