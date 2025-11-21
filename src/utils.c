// utils.c
// utility functions for string manipulation and parsing

#include "shell.h"

// trims leading and trailing whitespace from a string
char *trim_whitespace(char *str) {
    if (str == NULL) return NULL;

    char *end;

    // trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) // all spaces?
        return str;

    // trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // write new null terminator
    *(end + 1) = 0;

    return str;
}

// helper function to add a token to the args array
// returns 0 on success, -1 on error
int add_token(char **args, int *num_args, char *token) {
    if (*num_args >= MAX_ARGS - 1) {
        fprintf(stderr, "myshell: too many arguments.\n");
        return -1;
    }
    args[*num_args] = strdup(token);
    if (args[*num_args] == NULL) {
        perror("myshell: strdup failed");
        return -1;
    }
    (*num_args)++;
    args[*num_args] = NULL;
    return 0;
}

// tokenizes the input string into arguments
// handles quotes "", background '&', input '<', and output '>' redirection
char **tokenize_input(char *input, int *num_args, int *is_background, char **input_file, char **output_file) {
    char **args = (char **)malloc(MAX_ARGS * sizeof(char *));
    if (args == NULL) {
        perror("myshell: malloc failed for args");
        *num_args = 0;
        return NULL;
    }

    *num_args = 0;
    *is_background = 0;
    *input_file = NULL;
    *output_file = NULL;

    // temporary list to hold raw tokens before processing redirection
    char *raw_tokens[MAX_ARGS];
    int raw_count = 0;

    char *p = input;
    char buffer[MAX_COMMAND_LENGTH];
    int buf_idx = 0;
    int in_quote = 0;

    while (*p) {
        // skip leading whitespace if not in quote
        while (!in_quote && isspace((unsigned char)*p)) p++;

        if (*p == '\0') break;

        buf_idx = 0;
        // parse a single token
        while (*p) {
            if (*p == '"') {
                in_quote = !in_quote; // toggle quote mode
                p++;
                continue; // do not add the quote char itself to the buffer
            }

            if (!in_quote && isspace((unsigned char)*p)) {
                break; // end of token found
            }

            buffer[buf_idx++] = *p++;
        }
        buffer[buf_idx] = '\0';

        if (raw_count < MAX_ARGS) {
            raw_tokens[raw_count++] = strdup(buffer);
        }
    }

    // process raw tokens to handle redirection and background
    for (int i = 0; i < raw_count; i++) {
        if (strcmp(raw_tokens[i], "&") == 0) {
            // check if it is the last token
            if (i == raw_count - 1) {
                *is_background = 1;
            } else {
                // if & is strictly treated as a separator in standard shells,
                // this might need different logic, but for this assignment
                // we usually assume it's at the end.
                // treating it as a syntax error if not at end based on previous logic.
                fprintf(stderr, "myshell: syntax error: '&' must be the last argument.\n");

                // cleanup
                for(int k=0; k<raw_count; k++) free(raw_tokens[k]);
                free_tokens(args);
                *num_args = 0;
                return NULL;
            }
        } else if (strcmp(raw_tokens[i], "<") == 0) {
            if (i + 1 < raw_count) {
                if (*input_file != NULL) {
                    fprintf(stderr, "myshell: multiple input files specified.\n");
                    // cleanup
                    for(int k=0; k<raw_count; k++) free(raw_tokens[k]);
                    free_tokens(args);
                    *num_args = 0;
                    return NULL;
                }
                *input_file = strdup(raw_tokens[i + 1]);
                i++; // skip the filename token
            } else {
                fprintf(stderr, "myshell: syntax error: no input file specified.\n");
                for(int k=0; k<raw_count; k++) free(raw_tokens[k]);
                free_tokens(args);
                *num_args = 0;
                return NULL;
            }
        } else if (strcmp(raw_tokens[i], ">") == 0) {
            if (i + 1 < raw_count) {
                if (*output_file != NULL) {
                    fprintf(stderr, "myshell: multiple output files specified.\n");
                    for(int k=0; k<raw_count; k++) free(raw_tokens[k]);
                    free_tokens(args);
                    *num_args = 0;
                    return NULL;
                }
                *output_file = strdup(raw_tokens[i + 1]);
                i++; // skip the filename token
            } else {
                fprintf(stderr, "myshell: syntax error: no output file specified.\n");
                for(int k=0; k<raw_count; k++) free(raw_tokens[k]);
                free_tokens(args);
                *num_args = 0;
                return NULL;
            }
        } else {
            // normal argument
            if (add_token(args, num_args, raw_tokens[i]) == -1) {
                for(int k=0; k<raw_count; k++) free(raw_tokens[k]);
                free_tokens(args);
                *num_args = 0;
                return NULL;
            }
        }
    }

    // free the raw intermediate tokens
    for (int i = 0; i < raw_count; i++) {
        free(raw_tokens[i]);
    }

    return args;
}

// frees memory allocated for tokens
void free_tokens(char **tokens) {
    if (tokens == NULL) return;
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
