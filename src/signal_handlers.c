// signal_handlers.c
// functions for handling operating system signals

#include "shell.h"

void setup_signal_handlers() {
    struct sigaction sa_int;
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART; // restart interrupted system calls automatically
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("bropesh: sigaction for sigint failed");
    }

    struct sigaction sa_chld;
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART; // restart interrupted system calls
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("bropesh: sigaction for sigchld failed");
    }
}

//  (ctrl+c) handling
void handle_sigint(int signum) {
    (void)signum;

    if (foreground_child_pid != -1) {
        if (kill(foreground_child_pid, SIGINT) == -1 && errno != ESRCH) {
             perror("bropesh: failed to send sigint to foreground child");
        }
    }
    printf("\n");
    display_prompt();
}

void handle_sigchld(int signum) {
    (void)signum;

    int status;
    pid_t child_pid;
    // wnohang means do not block; return 0 immediately if no child is exited
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // if the exited child was a background process, report its completion
        if (child_pid != foreground_child_pid) {
            printf("\nbackground process %d finished.\n", child_pid);
            display_prompt();
        }
    }
    if (child_pid == -1 && errno != ECHILD) {
        perror("bropesh: waitpid error in sigchld handler");
    }
}

// (ctrl+d )handling
void handle_ctrl_d() {
    printf("exit\n"); // print "exit" message before actual exit
    // save history to file
    save_history();
    // free all dynamically allocated global resources
    // not doing a NULL check can cost lives :)
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
    exit(0);
}
