#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 1024

int main() {
    int pipe_fd[2];
    pid_t child_pid;

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    child_pid = fork();
    if (child_pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // This is the child (subscriber)
        close(pipe_fd[1]);

        char message[MAX_MESSAGE_SIZE];
        int bytes_read;

        while ((bytes_read = read(pipe_fd[0], message, MAX_MESSAGE_SIZE)) > 0)
            write(STDOUT_FILENO, message, bytes_read);

        close(pipe_fd[0]);
    } else {
        // This is the parent (publisher)
        close(pipe_fd[0]);

        char message[MAX_MESSAGE_SIZE];

        while (1) {
            if (fgets(message, MAX_MESSAGE_SIZE, stdin) == NULL) break;
            write(pipe_fd[1], message, strlen(message));
        }
        close(pipe_fd[1]);
    }
    return 0;
}

