// history.c
// functions for managing command history

#include "shell.h"

void load_history() {
    char history_file_path[PATH_MAX];
    if (home_dir == NULL) {
        fprintf(stderr, "bropesh: history: home directory not set, cannot load history.\n");
        return;
    }
    snprintf(history_file_path, sizeof(history_file_path), "%s%s", home_dir, HISTORY_FILE_NAME);

    history_file_ptr = fopen(history_file_path, "r");
    if (history_file_ptr == NULL) {
        return;
    }

    char line[MAX_COMMAND_LENGTH];
    while (fgets(line, MAX_COMMAND_LENGTH, history_file_ptr) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            // add to history, using the circular buffer logic
            int idx;
            if (history_count < MAX_HISTORY_SIZE) {
                idx = history_count;
            } else {
                idx = history_count % MAX_HISTORY_SIZE;
                // free the oldest command if we are overwriting
                if (history_commands[idx] != NULL) {
                    free(history_commands[idx]);
                }
            }
            history_commands[idx] = strdup(line);
            if (history_commands[idx] == NULL) {
                perror("bropesh: strdup failed for history command during load");
                break;
            }
            history_count++;
        }
    }
    fclose(history_file_ptr);
    history_file_ptr = NULL; // reset pointer, will be reopened for writing by save_history
}

void save_history() {
    char history_file_path[PATH_MAX];
    if (home_dir == NULL) {
        fprintf(stderr, "bropesh: history: home directory not set, cannot save history.\n");
        return;
    }
    snprintf(history_file_path, sizeof(history_file_path), "%s%s", home_dir, HISTORY_FILE_NAME);

    history_file_ptr = fopen(history_file_path, "w"); // overwrite file if it exists
    if (history_file_ptr == NULL) {
        perror("bropesh: failed to open history file for writing");
        return;
    }

    int actual_stored_count = (history_count < MAX_HISTORY_SIZE) ? history_count : MAX_HISTORY_SIZE;
    int start_idx = (history_count > MAX_HISTORY_SIZE) ? (history_count % MAX_HISTORY_SIZE) : 0;

    for (int i = 0; i < actual_stored_count; i++) {
        int current_idx = (start_idx + i) % MAX_HISTORY_SIZE;
        if (history_commands[current_idx] != NULL) {
            fprintf(history_file_ptr, "%s\n", history_commands[current_idx]);
        }
    }
    fclose(history_file_ptr);
    history_file_ptr = NULL;
}

void add_to_history(char *command) {
    if (command == NULL || strlen(command) == 0) {
        return;
    }

    int current_idx;
    if (history_count < MAX_HISTORY_SIZE) {
        current_idx = history_count;
    } else {
        current_idx = history_count % MAX_HISTORY_SIZE;
        if (history_commands[current_idx] != NULL) {
            free(history_commands[current_idx]);
        }
    }

    history_commands[current_idx] = strdup(command);
    if (history_commands[current_idx] == NULL) {
        perror("bropesh: strdup failed for history command");
        return;
    }
    history_count++;
}
