#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_MESSAGE_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <max_num_subs>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Create a directory for the pipes
    const char* dir_path = "/tmp/ex1";
    if ((mkdir(dir_path, 0777) == -1) && (errno != EEXIST)) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    // Create named pipes for subscribers
    int max_num_subs = atoi(argv[1]);
    char pipe_names[max_num_subs][64];
    int pipe_fds[max_num_subs];

    for (int i = 0; i < max_num_subs; i++) {
        snprintf(pipe_names[i], sizeof(pipe_names[i]), "%s/s%d", dir_path, i + 1);
        mkfifo(pipe_names[i], 0666);

        pipe_fds[i] = open(pipe_names[i], O_WRONLY);
        if (pipe_fds[i] == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    char message[MAX_MESSAGE_SIZE];

    // Read messages and send them to subscribers
    while (1) {
        if (fgets(message, MAX_MESSAGE_SIZE, stdin) == NULL) break;
        // Send the messages to subscribers using their named pipes
        for (int i = 0; i < max_num_subs; i++)
            write(pipe_fds[i], message, strlen(message));
    }

    for (int i = 0; i < max_num_subs; i++)
        close(pipe_fds[i]);

    return 0;
}

