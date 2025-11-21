// prompt.c
// functions for displaying the shell prompt

#include "shell.h"

// displays the shell prompt
void display_prompt() {
    char hostname[HOST_NAME_MAX + 1];
    char current_dir[PATH_MAX];
    char *username_str = NULL;
    struct passwd *pw = getpwuid(getuid());

    if (pw != NULL) {
        username_str = pw->pw_name;
    } else {
        username_str = "unknown";
    }

    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("bropesh: gethostname failed");
        strcpy(hostname, "unknown_host");
    }
    hostname[HOST_NAME_MAX] = '\0';

    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("bropesh: getcwd failed");
        strcpy(current_dir, "error_dir");
    }

    char display_dir[PATH_MAX];
    // support ~
    if (home_dir != NULL && strncmp(current_dir, home_dir, strlen(home_dir)) == 0) {
        if (strlen(current_dir) == strlen(home_dir)) {
            strcpy(display_dir, "~");
        } else {
            snprintf(display_dir, sizeof(display_dir), "~%s", current_dir + strlen(home_dir));
        }
    } else {
        strcpy(display_dir, current_dir);
    }

    printf("\033[1;36m<\033[1;32m%s@%s:\033[1;35m\033[1;33m %s\033[1;36m>\033[0m ",
           username_str,
           hostname,
           display_dir);

    // printf("<%s@%s:%s> ", username_str, hostname, display_dir);
    fflush(stdout); // ensure prompt is displayed immediately
}
