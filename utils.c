// utils.c
// utility functions for string manipulation and parsing

#include "shell.h"

char *trim_whitespace(char *str) {
    if (str == NULL) return NULL;

    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) // all spaces ??
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = 0;

    return str;
}

char **tokenize_input(char *input, int *num_args, int *is_background, char **input_file, char **output_file) {
    char *input_copy = strdup(input);
    if (input_copy == NULL) {
        perror("bropesh: strdup failed in tokenize_input");
        *num_args = 0;
        return NULL;
    }

    char **args = (char **)malloc(MAX_ARGS * sizeof(char *));
    if (args == NULL) {
        perror("bropesh: malloc failed for args in tokenize_input");
        free(input_copy);
        *num_args = 0;
        return NULL;
    }

    char *token;
    *num_args = 0;
    *is_background = 0;
    *input_file = NULL;
    *output_file = NULL;

    token = strtok(input_copy, " \t"); // handle spaces and tabs

    while (token != NULL) {
        if (*num_args >= MAX_ARGS - 1) { // leave space for null terminator
            fprintf(stderr, "bropesh: too many arguments. maximum %d.\n", MAX_ARGS - 1);
            break;
        }

        if (strcmp(token, "&") == 0) {
            *is_background = 1;

            token = strtok(NULL, " \t");
            if (token != NULL) {
                fprintf(stderr, "bropesh: syntax error: '&' must be the last argument.\n");
                free_tokens(args);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
            break;
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t");
            if (token == NULL) {
                fprintf(stderr, "bropesh: redirection error: no input file specified after '<'.\n");
                free_tokens(args);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
            if (*input_file != NULL) {
                 fprintf(stderr, "bropesh: redirection error: multiple input files specified.\n");
                free_tokens(args);

                if (*input_file) free(*input_file);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
            *input_file = strdup(token);
            if (*input_file == NULL) {
                perror("bropesh: strdup failed for input_file");
                free_tokens(args);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t"); //
            if (token == NULL) {
                fprintf(stderr, "bropesh: redirection error: no output file specified after '>'.\n");
                free_tokens(args);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
            if (*output_file != NULL) {
                fprintf(stderr, "bropesh: redirection error: multiple output files specified.\n");
                free_tokens(args);
                if (*output_file) free(*output_file);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
            *output_file = strdup(token);
            if (*output_file == NULL) {
                perror("bropesh: strdup failed for output_file");
                free_tokens(args);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
        } else {
            args[*num_args] = strdup(token);
            if (args[*num_args] == NULL) {
                perror("bropesh: strdup failed for argument");
                for (int k = 0; k < *num_args; k++) free(args[k]);
                free(args);
                free(input_copy);
                *num_args = 0;
                return NULL;
            }
            (*num_args)++;
        }
        token = strtok(NULL, " \t");
    }
    args[*num_args] = NULL;

    free(input_copy);
    return args;
}


void free_tokens(char **tokens) {
    // not doing a NULL check can cost lives :)
    if (tokens == NULL) return;
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
