// process.c
// functions for process management and external command execution

#include "shell.h"

// executes an external command
void execute_external_command(char **args, int is_background, char *input_file, char *output_file) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("bropesh: fork failed");
    } else if (pid == 0) {
        // child process
        // reset signal handlers to default for child process
        signal(SIGINT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        // handle i/o redirection for input
        if (input_file != NULL) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("bropesh: failed to open input file");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("bropesh: failed to redirect stdin");
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }
        // handle i/o redirection for output
        if (output_file != NULL) {
            // create file if not exists, write-only, truncate if exists, permissions 0644
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("bropesh: failed to open output file");
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("bropesh: failed to redirect stdout");
                exit(EXIT_FAILURE);
            }
            close(fd_out);
        }

        // execute the command
        execvp(args[0], args);

        fprintf(stderr, "bropesh: error running command \"%s\": %s\n", args[0], strerror(errno));

        exit(EXIT_FAILURE);
    } else {
        // parent process
        if (is_background) {
            printf("[%d] %d\n", pid, pid);
        } else {
            foreground_child_pid = pid; // track the fg child
            int status;
            // wait for the fg child to finish
            if (waitpid(pid, &status, 0) == -1) {
                if (errno != EINTR) {
                    perror("bropesh: waitpid failed for foreground process");
                }
            }
            foreground_child_pid = -1;
        }
    }

    // not doing a NULL check can cost lives :)
    if (input_file != NULL) {
        free(input_file);
    }
    if (output_file != NULL) {
        free(output_file);
    }
}
