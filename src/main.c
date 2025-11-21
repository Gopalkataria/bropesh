// main.c
// main entry point for the os mini-project shell

#include "shell.h"
#include <stdio.h>

// global variables
char *home_dir = NULL;
char *prev_dir = NULL;
pid_t foreground_child_pid = -1;
char *history_commands[MAX_HISTORY_SIZE];
int history_count = 0;
FILE *history_file_ptr = NULL;

int main() {
    char input[MAX_COMMAND_LENGTH];
    char *trimmed_input;
    char **args;
    int num_args;
    int is_background;
    char *input_file = NULL;
    char *output_file = NULL;

    // initialize home directory
    struct passwd *pw = getpwuid(getuid());
    if (pw != NULL) {
        home_dir = strdup(pw->pw_dir);
        if (home_dir == NULL) {
            perror("bropesh: failed to allocate memory for home_dir");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "bropesh: failed to get home directory for user %d\n", getuid());
        // fall back to current directory if home_dir cannot be determined
        home_dir = getcwd(NULL, 0);
        if (home_dir == NULL) {
            perror("bropesh: getcwd failed for home_dir");
            exit(EXIT_FAILURE);
        }
    }

    // initialize prev_dir with current directory
    prev_dir = getcwd(NULL, 0);
    if (prev_dir == NULL) {
        perror("bropesh: getcwd failed for prev_dir initialization");
    }

    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        history_commands[i] = NULL;
    }
    load_history();

    // setup signal handlers
    setup_signal_handlers();
    printf("  █████               2023112006 OSA Project              █████        \n");
    printf("  ▒▒███                                                   ▒▒███         \n");
    printf("   ▒███████  ████████   ██████  ████████   ██████   █████  ▒███████     \n");
    printf("   ▒███▒▒███▒▒███▒▒███ ███▒▒███▒▒███▒▒███ ███▒▒███ ███▒▒   ▒███▒▒███    \n");
    printf("   ▒███ ▒███ ▒███ ▒▒▒ ▒███ ▒███ ▒███ ▒███▒███████ ▒▒█████  ▒███ ▒███    \n");
    printf("   ▒███ ▒███ ▒███     ▒███ ▒███ ▒███ ▒███▒███▒▒▒   ▒▒▒▒███ ▒███ ▒███    \n");
    printf("   ████████  █████    ▒▒██████  ▒███████ ▒▒██████  ██████  ████ █████   \n");
    printf("  ▒▒▒▒▒▒▒▒  ▒▒▒▒▒      ▒▒▒▒▒▒   ▒███▒▒▒   ▒▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒ ▒▒▒▒▒    \n");
    printf("         \\    /\\                ▒███  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("          )  ( ')               ████                                   \n");
    printf("         (  /  )                ▒▒▒▒  Crafted with ♡ by Gopal Kataria \n");
    printf("          \\(__)|      \n\n") ;




    while (1) {
        display_prompt();

        if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // handle ctrl+d (end of file)
            handle_ctrl_d();
            break; // exit loop if ctrl+d detected
        }

        // remove trailing newline character
        input[strcspn(input, "\n")] = 0;

        trimmed_input = trim_whitespace(input);
        if (strlen(trimmed_input) == 0) { // no command case
            continue;
        }

        // do not store duplicate consecutive commands in history
        int last_command_idx = (history_count > 0) ? ((history_count - 1 + MAX_HISTORY_SIZE) % MAX_HISTORY_SIZE) : -1;

        if (last_command_idx != -1 && history_commands[last_command_idx] != NULL && strcmp(history_commands[last_command_idx], trimmed_input) == 0) {
            //  duplicate command, do not add to history
        } else {
            add_to_history(trimmed_input);
        }

        input_file = NULL;
        output_file = NULL;
        args = tokenize_input(trimmed_input, &num_args, &is_background, &input_file, &output_file);

        if (num_args == 0) { // tokenization failed
            // free redirection filenames
            if (input_file) free(input_file);
            if (output_file) free(output_file);
            if (args) free(args); // free the outer array if no tokens
            continue;
        }

        // built-in commands
        if (execute_builtin_command(args)) {
        } else {
            // external command
            execute_external_command(args, is_background, input_file, output_file);
            // redirection file names are freed within execute_external_command,
            // so set them to null and prevent double free.
            input_file = NULL;
            output_file = NULL;
        }

        free_tokens(args); // free memory of arguments
    }

    save_history(); // save history to file
    if (home_dir != NULL) {
        free(home_dir);
    }
    if (prev_dir != NULL) {
        free(prev_dir);
    }
    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        if (history_commands[i] != NULL) {
            free(history_commands[i]);
        }
    }
    if (history_file_ptr != NULL) {
        fclose(history_file_ptr);
    }

    return 0;
}
