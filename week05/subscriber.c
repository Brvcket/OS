#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_MESSAGE_SIZE 1024

void signal_handler() {
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <sub_index>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sub_index = atoi(argv[1]);

    char pipe_name[64];
    snprintf(pipe_name, sizeof(pipe_name), "/tmp/ex1/s%d", sub_index);

    // Open the named pipe for reading
    int pipe_fd = open(pipe_name, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char message[MAX_MESSAGE_SIZE];
    int read_bytes_num;

    signal(SIGINT, signal_handler);

    while (1) {
        read_bytes_num = read(pipe_fd, message, MAX_MESSAGE_SIZE);

        if (read_bytes_num == 0) {
            break;
        } else if (read_bytes_num == -1) {
            perror("read");
            break;
        } else {
            write(STDOUT_FILENO, message, read_bytes_num);
        }
    }
    close(pipe_fd);

    return 0;
}

